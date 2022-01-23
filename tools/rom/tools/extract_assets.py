#!/usr/bin/env python3
import sys
import os
import json

dir = "./tools"
cwd = os.path.dirname(os.path.realpath(__file__))

def read_asset_map():
    with open(os.path.join(cwd, "assets.json")) as f:
        ret = json.load(f)
    return ret

def read_local_asset_list(f):
    if f is None:
        return []
    ret = []
    for line in f:
        ret.append(line.strip())
    return ret


def asset_needs_update(asset, version):
    if version <= 5 and asset == "textures/spooky/bbh_textures.00800.rgba16.png":
        return True
    if version <= 4 and asset in ["textures/mountain/ttm_textures.01800.rgba16.png", "textures/mountain/ttm_textures.05800.rgba16.png"]:
        return True
    if version <= 3 and asset == "textures/cave/hmc_textures.01800.rgba16.png":
        return True
    if version <= 2 and asset == "textures/inside/inside_castle_textures.09000.rgba16.png":
        return True
    if version <= 1 and asset.endswith(".m64"):
        return True
    if version <= 0 and asset.endswith(".aiff"):
        return True
    return False


def remove_file(fname):
    os.remove(fname)
    print("deleting", fname)
    try:
        os.removedirs(os.path.dirname(fname))
    except OSError:
        pass


def clean_assets(local_asset_file):
    assets = set(read_asset_map().keys())
    assets.update(read_local_asset_list(local_asset_file))
    for fname in list(assets) + [".assets-local.txt"]:
        if fname.startswith("@"):
            continue
        try:
            remove_file(fname)
        except FileNotFoundError:
            pass


def main():
    # In case we ever need to change formats of generated files, we keep a
    # revision ID in the local asset file.
    new_version = 6

    try:
        local_asset_file = open(".assets-local.txt")
        local_asset_file.readline()
        local_version = int(local_asset_file.readline().strip())
    except Exception:
        local_asset_file = None
        local_version = -1

    langs = ['us']

    if len(sys.argv) < 3:
        print(json.dumps({ "status": False, "code": f"Usage: {sys.argv[0]} <path> <baserom_b64>"}))
        sys.exit(1)

    outdir  = sys.argv[1]
    romfile = sys.argv[2]

    asset_map = read_asset_map()
    all_assets = []
    any_missing_assets = False
    for asset, data in asset_map.items():
        if asset.startswith("@"):
            continue
        if os.path.isfile(os.path.join(outdir, asset)):
            all_assets.append((asset, data, True))
        else:
            all_assets.append((asset, data, False))
            if not any_missing_assets and any(lang in data[-1] for lang in langs):
                any_missing_assets = True

    if not any_missing_assets and local_version == new_version:
        # Nothing to do, no need to read a ROM. For efficiency we don't check
        # the list of old assets either.
        return

    # Late imports (to optimize startup perf)
    import subprocess
    import hashlib
    import tempfile
    import zipfile
    from collections import defaultdict

    new_assets = {a[0] for a in all_assets}

    previous_assets = read_local_asset_list(local_asset_file)
    if local_version == -1:
        # If we have no local asset file, we assume that files are version
        # controlled and thus up to date.
        local_version = new_version

    # Create work list
    todo = defaultdict(lambda: [])
    for (asset, data, exists) in all_assets:
        # Leave existing assets alone if they have a compatible version.
        if exists and not asset_needs_update(asset, local_version):
            continue

        meta = data[:-2]
        size, positions = data[-2:]
        for lang, pos in positions.items():
            mio0 = None if len(pos) == 1 else pos[0]
            pos = pos[-1]
            if lang in langs:
                todo[(lang, mio0)].append((asset, pos, size, meta))
                break

    # Load ROMs
    roms = {}
    try:
        with open(romfile, "rb") as f:
            roms['us'] = f.read()
    except Exception as e:
        print(json.dumps({ "status": False, "code": f"Failed to open {romfile}! {str(e)}"}))
        sys.exit(1)

    # Go through the assets in roughly alphabetical order (but assets in the same
    # mio0 file still go together).
    keys = sorted(list(todo.keys()), key=lambda k: todo[k][0][0])
    # Import new assets
    for key in keys:
        assets = todo[key]
        lang, mio0 = key
        if mio0 == "@sound":
            with tempfile.NamedTemporaryFile(prefix="ctl", delete=False) as ctl_file:
                with tempfile.NamedTemporaryFile(prefix="tbl", delete=False) as tbl_file:
                    rom = roms[lang]
                    size, locs = asset_map["@sound ctl " + lang]
                    offset = locs[lang][0]
                    ctl_file.write(rom[offset : offset + size])
                    ctl_file.close()
                    size, locs = asset_map["@sound tbl " + lang]
                    offset = locs[lang][0]
                    tbl_file.write(rom[offset : offset + size])
                    tbl_file.close()
                    args = [
                        "python3",
                        os.path.join(dir, "disassemble_sound.py"),
                        ctl_file.name,
                        tbl_file.name,
                        "--only-samples",
                    ]
                    for (asset, pos, size, meta) in assets:
                        output = os.path.join(outdir, asset)
                        args.append(output + ":" + str(pos))
                    try:
                        subprocess.run(args, check=True)
                    finally:
                        os.unlink(ctl_file.name)
                        os.unlink(tbl_file.name)
            continue

        if mio0 is not None:
            image = subprocess.run(
                [
                    os.path.join(dir, "mio0"),
                    "-d",
                    "-o",
                    str(mio0),
                    romfile,
                    "-",
                ],
                check=True,
                stdout=subprocess.PIPE,
            ).stdout
        else:
            image = roms[lang]

        for (asset, pos, size, meta) in assets:
            output = os.path.join(outdir, asset)
            # print("extracting", output)
            input = image[pos : pos + size]
            os.makedirs(os.path.dirname(output), exist_ok=True)

            if asset.endswith(".png"):
                png_file = tempfile.NamedTemporaryFile(prefix="asset", delete=False)
                try:
                    png_file.write(input)
                    png_file.flush()
                    png_file.close()
                    if asset.startswith("textures/skyboxes/") or asset.startswith("levels/ending/cake"):
                        if asset.startswith("textures/skyboxes/"):
                            imagetype = "sky"
                        else:
                            imagetype =  "cake" + ("-eu" if "eu" in asset else "")
                        subprocess.run(
                            [
                                os.path.join(dir, "skyconv"),
                                "--type",
                                imagetype,
                                "--combine",
                                png_file.name,
                                output,
                            ],
                            check=True,
                        )
                    else:
                        w, h = meta
                        fmt = asset.split(".")[-2]
                        subprocess.run(
                            [
                                os.path.join(dir, "n64graphics"),
                                "-e",
                                png_file.name,
                                "-g",
                                output,
                                "-f",
                                fmt,
                                "-w",
                                str(w),
                                "-h",
                                str(h),
                            ],
                            check=True,
                        )
                finally:
                    png_file.close()
                    os.remove(png_file.name)
            else:
                with open(output, "wb") as f:
                    f.write(input)
    print(json.dumps({ "status": True }))
main()