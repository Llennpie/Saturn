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

if (os.path.exists('./.assets-local.txt') == False):
    print("Please compile the game before running this script!\n")
    quit()

# Declare GS code

print("Enter GS code (multiline):\n")

t_lines = []
for i in range(24):
    t_lines.append(input())

t_lines.sort()

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

            f = open(fp_modeldata, "r")
            filedata = f.read()
            f.close()

            filedata = filedata.replace("0x00, 0x00, 0x7f,", c5)
            filedata = filedata.replace("0x00, 0x00, 0xff,", c6)
            filedata = filedata.replace("0x7f, 0x00, 0x00,", c11)
            filedata = filedata.replace("0xff, 0x00, 0x00,", c12)
            filedata = filedata.replace("0x7f, 0x7f, 0x7f,", c17)
            filedata = filedata.replace("0xff, 0xff, 0xff,", c18)
            filedata = filedata.replace("0x39, 0x0e, 0x07,", c23)
            filedata = filedata.replace("0x72, 0x1c, 0x0e,", c24)
            filedata = filedata.replace("0x7f, 0x60, 0x3c,", c29)
            filedata = filedata.replace("0xfe, 0xc1, 0x79,", c30)
            filedata = filedata.replace("0x39, 0x03, 0x00,", c35)
            filedata = filedata.replace("0x73, 0x06, 0x00,", c36)

            f = open(fp_modeldata, "w")
            f.write(filedata)
            f.close()

            print("\nFinished writing to " + cc_name + ".\nYou can find your CC under the DynOS Menu > Model Packs.")
            print("\nIf you want to edit textures, you can find them under build/us_pc/dynos/packs/" + cc_name + ".\nYou may also have to delete mario_geo.bin in that folder to reload the model.")