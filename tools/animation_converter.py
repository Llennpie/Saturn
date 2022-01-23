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
    "struct Animation* json_anim_table[] = {\n"
]

anim_data = [
    '#include "types.h"\n',
    '#include "animations.h"\n'
]

def reverse(objArray, index, length):
    i = index;
    j = index + length - 1;
    while (i < j):
        temp = objArray[i];
        objArray[i] = objArray[j];
        objArray[j] = temp;
        i += 1;
        j -= 1;
    return objArray

def swapEndian(array, wordSize):
    baseList = list(map(lambda x: int(x, 16), array));
    byteArray = []
    for x in range(0, len(baseList), 2):
        byteArray.append((baseList[x] << 8) | (baseList[x + 1]))
    return byteArray


def write_list(type, name, values):
    data = [
        f"const {type} {name}[] = {'{'}\n"
    ]
    tmp = 0
    breakline = 8
    for value in values:
        data.append("{}{}{}".format("    " if tmp % breakline == 0 else "", "{0:#0{1}x}".format(value, 6), ",\n" if ((tmp + 1) % breakline == 0) else ", "))
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

def append_animation(file):
    raw = open(file, 'r')
    data = json.load(raw)

    name    = data['name'].lower().replace(' ', '_').replace('-', 'x')
    looping = ANIM_FLAG_NOLOOP if bool(data['looping']) else ANIM_FLAG_NONE
    nodes   = data['nodes']
    length  = data['length']
    start   = 0
    loop_start = 0
    loop_end   = length
    breakline  = 4
    values  = data['values']
    indices = data['indices']
    amount  = len(animation_table) - 1

    values_len = len(values)
    if(values_len % 8 != 0):
        copy = []
        for x in range(values_len + (8 - values_len % 8)):
            copy.append(values[x] if x < values_len else "0x00")
        values = copy

    values  = swapEndian(values, 4)
    indices = swapEndian(indices, 4)

    anim_data.append("\n")
    anim_data.extend(write_list("u16", f"{name}_indices", indices))
    anim_data.extend(write_list("s16", f"{name}_values", values))
    anim_data.extend(write_struct(name, looping, 189, start, loop_start, loop_end, nodes, f"{name}_values", f"{name}_indices", 0))
    header_declarations.append(f'// Animation declarations for {os.path.basename(file)}\n')
    header_declarations.append(f'extern const struct Animation {name}[];\n\n');
    animation_table.append("{}{}{}".format("    " if amount % breakline == 0 else "", name, "\n" if ((amount + 1) % breakline == 0) else ", "))
    raw.close()
    print(f"[Neptune] Converting '{os.path.basename(file)}' to '{name}' animation")
    return anim_data

def generate_header(path):
    header_declarations.append("// Animation table for extern use\n")
    header_declarations.append("extern struct Animation* json_anim_table[];\n")
    header_declarations.append("\n#endif")
    with open(os.path.join(path, "animations.h"), 'w') as f:
        f.writelines(header_declarations)
    print(f"[Neptune] Succefully converted '{len(animation_table) - 3}' animations")

def write_anim_table():
    breakline  = 4
    amount  = len(animation_table) - 1
    animation_table[amount] = animation_table[amount].replace(",", "")
    if amount % breakline != 0:
        animation_table.append("\n")
    animation_table.append("};\n")
    anim_data.append("\n")
    anim_data.extend(animation_table)

def generate_cfile(path):
    write_anim_table()
    os.makedirs(path, exist_ok=True)
    with open(os.path.join(path, "animations.c"), 'w') as f:
        f.writelines(anim_data)

def main(args):
    if len(args) < 2:
        print("[Neptune] Usage: animation_converter.py <file/directory> <output>")
        return

    in_path  = args[1]
    out_path = args[2]
    done     = False

    if not os.path.exists(in_path):
        print("[Neptune] The specified file or folder was not found")
        return

    inmode = Path(in_path)
    if inmode.is_dir():
        path = f"{in_path}/*.json"
        files = list(glob.iglob(path, recursive=False))
        files.sort()
        for file in files:
            append_animation(file)
        done = True
    if inmode.is_file():
        append_animation(in_path)
        done = True
    if done:
        generate_cfile(out_path)
        generate_header(out_path)
        return
    print("[Neptune] The specified path is not a file or folder")

main(sys.argv)