# noughts_and_crosses_nds

A tiny Noughts & Crosses game for the Nintendo DS.

This is very WIP and probably will never be finished. Its destiny is probably just to serve as another code example that people can possibly use for reference in future. Makefile is copied from https://github.com/devkitPro/nds-examples

Some notes:

- The game uses a single screen (the main screen) and touch controls.
- The graphics are drawn using the sprite/OAM API https://libnds.devkitpro.org/sprite_8h.html. Each "cell" in the game is a 32x32 image.
- To play the game, tap one of the cells to place your first item. Then pass the console to another player who then taps one of the cells. When a row, column or diagonal is filled, the items of a given player spin to show they have won the game!

### Building

The game uses the devkitARM compiler collection - this is a port of GCC and a set of libraries for Nintendo DS/GBA/etc development. The easiest way to set this up is to use the official [docker images ](https://github.com/devkitPro/docker/). I wrote a simple build script that uses this docker image to build the game, then copies the result into the `dist` directory. You should be able to build the final `.nds` using (it might be necessary to chmod +x this script):

```
./build.sh
```
