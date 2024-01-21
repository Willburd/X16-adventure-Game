#ifndef LEVELS_H
#define LEVELS_H

// We will use 16x16 Tiles so the screen will be 40x30 (this is 64x32 tiles)
#define MAPBASE_TILE_COUNT 64*32

#define TILE_SIZE 256 // 16x16 Tiles in 8 bpp mode = 256 bytes
#define NUM_TILES 24
#define TILE_BASE_SIZE TILE_SIZE * NUM_TILES // 6,144

// MapBase will use 2 bytes x (64 x 32 Tiles) = 4kb (4096)
#define MAPBASE_BYTE_SIZE MAPBASE_TILE_COUNT * 2

// We can put our TileBase as Addr 0
#define TILE_BASE_ADDR 0

// Start the MapBase at 6,144 to give the TileBase enough room for 24 tiles
// WARNING: Remember the chapter on MapBase, the address must be a multiple of 512 
#define LAYER0_MAPBASE_ADDR TILE_BASE_SIZE

// Start the L1 MapBase after the L0 MapBase
// Just add MAPBASE_BYTE_SIZE since that is how much mem the L0 MapBase uses
#define LAYER1_MAPBASE_ADDR LAYER0_MAPBASE_ADDR + MAPBASE_BYTE_SIZE

// Switch this to 0 to make L1 ALL transparent so you can see all of L0
#define DRAW_LAYER_1 1

extern unsigned int scroll0x;
extern unsigned int scroll0y;
extern unsigned int scroll1x;
extern unsigned int scroll1y;

extern void InitLayers();

extern void LevelScrollMove(signed short amount_h, signed short amount_v);
extern void LevelScrollSet(signed short set_h, signed short set_v);
extern void LevelScrollLock(signed short min_left, signed short min_up,signed short max_right, signed short max_down);

extern void HudScrollMove(signed short amount_h, signed short amount_v);
extern void HudScrollSet(signed short set_h, signed short set_v);

// tell the vera to update graphics
extern void UpdateVeraScroll();

#endif