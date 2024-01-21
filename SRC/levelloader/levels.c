#include <cx16.h>
#include "levels.h"

unsigned int scroll0x;
unsigned int scroll0y;
unsigned int scroll1x;
unsigned int scroll1y;

void InitLayers()
{
    // layer inits
    unsigned long tileBaseAddr = TILE_BASE_ADDR;
    unsigned long l0MapBaseAddr = LAYER0_MAPBASE_ADDR;
    unsigned long l1MapBaseAddr = LAYER1_MAPBASE_ADDR;
    unsigned char c;
    unsigned short i;
    
    // layer enables
    VERA.display.video |= 0b00110000;

    // Layers can share a tilebase (use the same tiles)
    // Get bytes 16-11 of the new TileBase address
    // ALSO Set Tile W/H (bits 0/1) to 1 for 16 pixel tiles
    VERA.layer0.tilebase = tileBaseAddr>>9 | 0b11;
    VERA.layer1.tilebase = tileBaseAddr>>9 | 0b11;

    // Get bytes 16-9 of the MapBase addresses and set on both layers
    VERA.layer0.mapbase = l0MapBaseAddr>>9;
    VERA.layer1.mapbase = l1MapBaseAddr>>9;

    // With 16 pixel tiles, we don't need as many tiles
    // Only 640/16 = 40, 480/16=30 (40x30 tile resolution now)
    // Set the Map Height=0 (32), Width=1 (64)
    // Set Color Depth to 8 bpp mode
    VERA.layer1.config = 0b00010011;
    VERA.layer0.config = 0b00010011;

    // Point to the TileBase address so we can write to VRAM
    VERA.address = tileBaseAddr;
    VERA.address_hi = tileBaseAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Create 16 tiles, each with a different color (0-15)
    // Tile 0 will be all transparent color 0
    for (c=0; c<24; c++) {
        for (i=0; i<TILE_SIZE; i++) {
            VERA.data0 = c;
        }
    };

    // Point to the L0 MapBase
    VERA.address = l0MapBaseAddr;
    VERA.address_hi = l0MapBaseAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // L0 uses all the colors between 2-23
    c = 2;
    for (i=0; i<MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = c;
        VERA.data0 = 0;

        c++;
        if (c == 24) {
            c=2;
        }
    }

    // Point to the L1 MapBase
    VERA.address = l1MapBaseAddr;
    VERA.address_hi = l1MapBaseAddr>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;

    // Alternate between columns of transparent and white
    // You will see L0 through the transparent columns
    for (i=0; i<MAPBASE_TILE_COUNT; i++) {
        VERA.data0 = DRAW_LAYER_1
            ? i % 2 == 0 ? 0 : 1
            : 0;
        VERA.data0 = 0;
    }
}

void LevelScrollMove(signed short amount_h, signed short amount_v)
{
    scroll0x += amount_h;
    scroll0y += amount_v;
}

void LevelScrollSet(signed short set_h, signed short set_v)
{
    scroll0x = set_h;
    scroll0y = set_v;
}

void LevelScrollLock(signed short min_left, signed short min_up,signed short max_right, signed short max_down)
{
    if(scroll0x + 0xff < min_left + 0xff) // offset to avoid rollunder at 0
    {
        scroll0x = min_left;
    }
    if(scroll0x >= max_right) // we don't bother with overflow, we are likely not going to
    {
        scroll0x = max_right;
    }
    if(scroll0y + 0xff < min_up + 0xff) // offset to avoid rollunder at 0
    {
        scroll0y = min_up;
    }
    if(scroll0y >= max_down) // we don't bother with overflow, we are likely not going to
    {
        scroll0y = max_down;
    }
}

void HudScrollMove(signed short amount_h, signed short amount_v)
{
    scroll1x += amount_h;
    scroll1y += amount_v;
}

void HudScrollSet(signed short set_h, signed short set_v)
{
    scroll1x = set_h;
    scroll1y = set_v;
}

void UpdateVeraScroll()
{
    VERA.layer0.hscroll = scroll0x;
    VERA.layer0.vscroll = scroll0y;
    VERA.layer1.hscroll = scroll1x;
    VERA.layer1.vscroll = scroll1y;
}