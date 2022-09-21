# Enhancements

This directory contains unofficial patches to the source code that provide various features
and enhancements.

To apply a patch, run `git apply [patch]` where `[patch]` is the name of the .patch file you wish to apply. This will perform all of the patch's changes to the source code.

Likewise, to undo the changes from a patch you applied, run `git apply -R` with the name of the .patch file you wish to undo. 

To create your own enhancement patch, make changes to the source code and run `git diff > my_patch.patch`.
Your changes will be stored in the .patch file you specify.

The following enhancements are included in this directory:

## 60 FPS - `60fps_saturn.patch`

This allows the game to be rendered at 60 FPS instead of 30 FPS by interpolation (game logic still runs at 30 FPS).

This is the 60fps patch from [sm64ex](https://github.com/sm64pc/sm64ex/tree/nightly/enhancements) adapted for Saturn.
