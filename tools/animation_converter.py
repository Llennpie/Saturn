#!/usr/bin/env python3

from pathlib import Path
import sys
import os
import os.path
import json
import glob

ANIM_FLAG_NONE   = 0x00
ANIM_FLAG_NOLOOP = 0x01

header_declarations = [
    "#ifndef JSON_ANIMATIONS_H\n",
    "#define JSON_ANIMATIONS_H\n",
    "\n",
    '#include "types.h"\n\n'
]

animation_table = [
    "struct Animation anim_table[] = {\n"
]

anim_data = [
    '#include "types.h"\n',
    '#include "animations.h"\n'
]

def write_list(type, name, values):
    data = [
        f"const {type} {name}[] = {'{'}\n"
    ]
    tmp = 0
    breakline = 8
    for value in values:
        data.append("{}{}{}".format("    " if tmp % breakline == 0 else "", value, "\n" if ((tmp + 1) % breakline == 0) else ", "))
        tmp += 1
    data[len(data) - 1] = data[len(data) - 1].replace(",", "")
    if tmp % breakline != 0:
        data.append("\n")
    data.append("};\n\n")
    return data

def write_struct(name, flags, animY, startFrame, loopStart, loopEnd, boneCount, values, index, length):
    data = [
        f"const struct Animation {name}[] = {'{'}\n",
        f"    /* flags */ {flags},\n",
        f"    /* animYTransDivisor */ {animY},\n",
        f"    /* startFrame */ {startFrame},\n",
        f"    /* loopStart */ {loopStart},\n",
        f"    /* loopEnd */ {loopEnd},\n",
        f"    /* unusedBoneCount */ {boneCount},\n",
        f"    /* values */ {values},\n",
        f"    /* index */ {index},\n",
        f"    /* length */ {length},\n",
        "};\n"
    ]
    return data;

def read_json(file):
    raw = open(file, 'r')
    data = json.load(raw)

    name    = data['name'].lower().replace(' ', '_')
    looping = ANIM_FLAG_NONE if bool(data['looping']) else ANIM_FLAG_NOLOOP
    nodes   = data['nodes']
    length  = data["length"]
    start   = 0
    loop_start = 0
    loop_end   = 0
    breakline  = 4
    values  = data['values']
    indices = data['indices']
    amount  = len(animation_table) - 1

    anim_data.append("\n")
    anim_data.extend(write_list("u16", f"{name}_indices", indices))
    anim_data.extend(write_list("s16", f"{name}_values", values))
    anim_data.extend(write_struct(name, looping, 0, start, loop_start, loop_end, nodes, f"{name}_values", f"{name}_indices", length))
    header_declarations.append(f'// Animation declarations for {os.path.basename(file)}\n')
    header_declarations.append(f'extern const struct Animation {name};\n\n');
    animation_table.append("{}{}{}".format("    " if amount % breakline == 0 else "", name, "\n" if ((amount + 1) % breakline == 0) else ", "))
    raw.close()
    print(f"[Neptune] Converting '{os.path.basename(file)}' to '{name}' animation")
    return anim_data

def generate_header(path):
    breakline  = 4
    amount  = len(animation_table) - 1
    animation_table[amount] = animation_table[amount].replace(",", "")
    if amount % breakline != 0:
        animation_table.append("\n")
    animation_table.append("};\n")
    header_declarations.extend(animation_table)
    header_declarations.append("\n#endif")
    with open(os.path.join(path, "animations.h"), 'w') as f:
        f.writelines(header_declarations)
    print(f"[Neptune] Succefully converted '{amount}' animations")

def generate_cfile(file, path):
    os.makedirs(path, exist_ok=True)
    with open(os.path.join(path, "animations.c"), 'w') as f:
        f.writelines(read_json(file))

def main(args):
    if len(args) < 2:
        print("[Neptune] Usage: animation_converter.py <file/directory> <output>")
        return

    in_path  = args[1]
    out_path = args[2]

    if not os.path.exists(in_path):
        print("[Neptune] The specified file or folder was not found")
        return

    inmode = Path(in_path)
    if inmode.is_dir():
        path = f"{in_path}/*.json"
        files = list(glob.iglob(path, recursive=False))
        files.sort()
        for file in files:
            try:
                generate_cfile(file, out_path)
            except Exception as err:
                print(f"Error on {file}")
                print(err)
        generate_header(out_path)
        return
    if inmode.is_file():
        generate_cfile(in_path, out_path)
        generate_header(out_path)
        return

    print("[Neptune] The specified path is not a file or folder")

main(sys.argv)