#!/usr/bin/env python3
import sys
import os
import shutil

# Checks arguments

if (len(sys.argv) != 2):
    print("Usage: python3 ./tools/dynos_cc.py <character_name>")
    quit()

cc_name = sys.argv[1]

# Does some checks to see if our files will line up correctly

if (os.path.exists('./README.md') == False):
    print("Please make sure this script is executed from Saturn's root directory.\n")
    quit()

if (os.path.isdir('./build') == False):
    print("Please compile the game before running this script!\n")
    quit()

# Note

print("Note that this converter is unfinished and doesn't always work with older color codes. For best results, use either cometSpectrum or M64MM.\n")

# Declare GS code

print("Paste GS code (then press enter twice):\n")

base_address = "8107EC"
good_addresses = ["20", "22", "28", "2A", "38", "3A", "40", "42", "50", "52", "58", "5A", "68", "6A", "70", "72", "80", "82", "88", "8A", "98", "9A", "A0", "A2"]

t_lines = []
current_line = "a"
while (current_line != ""):
    current_line = input()
    for addr in good_addresses:
        if base_address + addr in current_line:
            t_lines.append(current_line)

t_lines.sort()

print("\n")

#pain
c5 = "0x" + t_lines[0].partition(" ")[2][:2].lower() + ", 0x" + t_lines[0].partition(" ")[2][2:].lower() + ", 0x" + t_lines[1].partition(" ")[2][:2].lower() + ","
c6 = "0x" + t_lines[2].partition(" ")[2][:2].lower() + ", 0x" + t_lines[2].partition(" ")[2][2:].lower() + ", 0x" + t_lines[3].partition(" ")[2][:2].lower() + ","
c11 = "0x" + t_lines[4].partition(" ")[2][:2].lower() + ", 0x" + t_lines[4].partition(" ")[2][2:].lower() + ", 0x" + t_lines[5].partition(" ")[2][:2].lower() + ","
c12 = "0x" + t_lines[6].partition(" ")[2][:2].lower() + ", 0x" + t_lines[6].partition(" ")[2][2:].lower() + ", 0x" + t_lines[7].partition(" ")[2][:2].lower() + ","
c17 = "0x" + t_lines[8].partition(" ")[2][:2].lower() + ", 0x" + t_lines[8].partition(" ")[2][2:].lower() + ", 0x" + t_lines[9].partition(" ")[2][:2].lower() + ","
c18 = "0x" + t_lines[10].partition(" ")[2][:2].lower() + ", 0x" + t_lines[10].partition(" ")[2][2:].lower() + ", 0x" + t_lines[11].partition(" ")[2][:2].lower() + ","
c23 = "0x" + t_lines[12].partition(" ")[2][:2].lower() + ", 0x" + t_lines[12].partition(" ")[2][2:].lower() + ", 0x" + t_lines[13].partition(" ")[2][:2].lower() + ","
c24 = "0x" + t_lines[14].partition(" ")[2][:2].lower() + ", 0x" + t_lines[14].partition(" ")[2][2:].lower() + ", 0x" + t_lines[15].partition(" ")[2][:2].lower() + ","
c29 = "0x" + t_lines[16].partition(" ")[2][:2].lower() + ", 0x" + t_lines[16].partition(" ")[2][2:].lower() + ", 0x" + t_lines[17].partition(" ")[2][:2].lower() + ","
c30 = "0x" + t_lines[18].partition(" ")[2][:2].lower() + ", 0x" + t_lines[18].partition(" ")[2][2:].lower() + ", 0x" + t_lines[19].partition(" ")[2][:2].lower() + ","
c35 = "0x" + t_lines[20].partition(" ")[2][:2].lower() + ", 0x" + t_lines[20].partition(" ")[2][2:].lower() + ", 0x" + t_lines[21].partition(" ")[2][:2].lower() + ","
c36 = "0x" + t_lines[22].partition(" ")[2][:2].lower() + ", 0x" + t_lines[22].partition(" ")[2][2:].lower() + ", 0x" + t_lines[23].partition(" ")[2][:2].lower() + ","

# Setting paths

fp_mario = "actors/mario"
fp_packs = "build/us_pc/dynos/packs"

if (os.path.isdir(fp_mario) == False):
    print("Could not find " + fp_mario + ".")
else:
    if (os.path.isdir(fp_packs) == False):
        print("Could not find " + fp_packs + ".")
    else:
        fp_model = fp_packs + "/" + cc_name
        os.mkdir(fp_model, 0o777)

        if (os.path.isdir(fp_model + "/mario") == True):
            print("Model already exists.")
        else:
            fp_destination = shutil.copytree(fp_mario, fp_model + "/mario")
            fp_modeldata = fp_destination + "/model.inc.c"

            with open(fp_modeldata, 'r') as file:
                filelines = file.readlines()

            filelines[4] = "    " + c5
            filelines[5] = "    " + c6 + " 0x28, 0x28, 0x28"
            filelines[10] = "    " + c11
            filelines[11] = "    " + c12 + " 0x28, 0x28, 0x28"
            filelines[16] = "    " + c17
            filelines[17] = "    " + c18 + " 0x28, 0x28, 0x28"
            filelines[22] = "    " + c23
            filelines[23] = "    " + c24 + " 0x28, 0x28, 0x28"
            filelines[28] = "    " + c29
            filelines[29] = "    " + c30 + " 0x28, 0x28, 0x28"
            filelines[34] = "    " + c35
            filelines[35] = "    " + c36 + " 0x28, 0x28, 0x28"

            with open(fp_modeldata, 'w') as file:
                file.writelines(filelines)

            # Copy over new texture
            shutil.copy("textures/saturn/mario_logo.rgba16.png", fp_destination)

            print("\nFinished writing to " + cc_name + ".\nYou can find your CC under the DynOS Menu > Model Packs.")
            print("\nIf you want to edit textures, you can find them under build/us_pc/dynos/packs/" + cc_name + ".\nYou may also have to delete mario_geo.bin in that folder to reload the model.")
