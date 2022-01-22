#!/usr/bin/env python3

import sys
import os
import os.path
import glob
import re
from pathlib import Path

header_declarations = [
    "#ifndef MARIO_ANIMATIONS_H\n",
    "#define MARIO_ANIMATIONS_H\n",
    "\n",
    '#include "types.h"\n\n'
]

animation_table = [
    "\nstruct Animation anim_table[] = {\n"
]

count = 1

def parse_anim(file, lines):
    global count
    header_declarations.append(f'// Animation declarations for {os.path.basename(file)}\n')
    for idx, line in enumerate(lines):
        line = line.rstrip('\n')
        if "Animation" in line:
            header_declarations.append(f'extern {line.split("=")[0].strip()};\n');
            tmp = ("\n" if count % 6 == 0 else "")
            animation_table.append(f'    {line.split("[]")[0].strip().split("Animation ")[1]},{tmp}')
            count += 1
    header_declarations.append("\n")
path = f"{str(Path(__file__).resolve().parents[1])}/assets/animations/*.c"

files = list(glob.iglob(path, recursive=True))
files.sort()
for file in files:
    try:
        if not "\\build" in file:
            parse_anim(file, open(file, 'r').readlines())
    except Exception as err:
        print(f"Error on {file}")
        print(err)
animation_table.append("};\n")
header_declarations.extend(animation_table)
with open("mario_animations.h", 'w') as f:
    f.writelines(header_declarations)