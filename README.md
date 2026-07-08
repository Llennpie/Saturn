# Saturn

- **Saturn** is an all-in-one machinima studio for *Super Mario 64*, based on [the PC port](https://github.com/sm64pc/sm64ex).
- *In order to download or compile the editor, a prior copy of the game is required. This is to avoid including any copyrighted material.*

![image](https://github.com/Llennpie/Saturn/assets/44985633/e671b48a-e1c3-446a-9cff-0c76f49d069a)

### Features

```
- Machinima camera system
- Extensive color code editor
- Custom model library
  - Create, share, and download custom playermodels. No reloading required!
  - Works with color codes
  - Completely customizable expressions and textures
- CHROMA KEY STAGE
  - Toggle instantly from any level
- Keyframe editor
  - Animate almost anything!
- Completely rebindable controls
  - Supports most modern controllers
- Dynamic window size/resolution
- Interpolated 60 fps
  
...and more!
```
This fork basically changed lines in some files to make saturn compilable for macOS.

You will not have discord integration with this fork

Heres how to compile saturn for mac:
1. git clone https://github.com/Pracargithub/Saturn-macOSfix
2. Copy an unmodified, vanilla ROM into the repo's directory and rename it to baserom.us.z64
3. in Terminal using homebrew package manager (https://brew.sh/) install these packages by typing
**brew install coreutils make mingw-w64 gcc sdl2 pkg-config glew glfw3 libusb adamstark-audiofile**
4. type cd Saturn-macOSfix
5. Finally, compile via:
**gmake OSX_BUILD=1 CPP="clang -E -x c -P" DISCORDGAMESDK=0 DISCORDRPC=0 -j$(nproc)**
When finished, an executable will be created in /build/us_pc/
to launch the app cd into repos directory and drag binary file saturn.us.f3dex2e into terminal and saturn will launch

This was not tested on intel based mac but it probably works
---

Support developer behind saturn legacy

[![ko-fi](https://ko-fi.com/J3J05B5WR)
