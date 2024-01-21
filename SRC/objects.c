#include <cx16.h>
#include <stdio.h>
#include "objects.h"
#include "gameobjs/player.h"

struct Gameobj;
unsigned char currentObjects = 0;
struct Gameobj OBJECTS[MAXOBJS];

// OAM handler
unsigned char InitGameObject(unsigned char getid, unsigned short x,unsigned short y, unsigned char flags) {
    unsigned char i = 0;
    unsigned char spriteSlot = 0xff;
    struct Gameobj obj = {};

    // add to process list
    while(i < MAXOBJS)
    {
        if(i >= currentObjects)
        {
            // new slot
            currentObjects += 1;
            spriteSlot = currentObjects-1;
            break;
        }
        else
        {
            // used slots
            if(!(OBJECTS[i].flags & 0b00000001))
            {
                // active flag disabled, use this slot!
                spriteSlot = i;
                break;
            }
        }
        i++;
    }

    if(i == MAXOBJS)
    {
        return 0xff;
    }

    // solve sprite address;
    OBJECTS[spriteSlot] = obj;
    OBJECTS[spriteSlot].id = getid;
    // position
    OBJECTS[spriteSlot].x = x;
    OBJECTS[spriteSlot].y = y;
    OBJECTS[spriteSlot].subx = 0;
    OBJECTS[spriteSlot].suby = 0;
    // velocity
    OBJECTS[spriteSlot].subvelx = 0;
    OBJECTS[spriteSlot].subvely = 0;
    OBJECTS[spriteSlot].velx = 0;
    OBJECTS[spriteSlot].vely = 0;
    // gfx and state
    OBJECTS[spriteSlot].flags = flags;
    OBJECTS[spriteSlot].gfxState = 0;
    OBJECTS[spriteSlot].timer = 0;
    // unique object inits!
    switch(OBJECTS[spriteSlot].id)
    {
        case 0:
            Gameobject_PlayerInit(spriteSlot);
        break;

        case 1:
            Gameobject_PlayerShotInit(spriteSlot);
        break;

        default:
            // all above entries need to call this eventually!
            HandleGameObjectGraphicsInit(spriteSlot, 0x0000, 0, 0, 0);
        break;
    }
    return spriteSlot;
};

void HandleGameObjectGraphicsInit(unsigned char index, unsigned long gfxAddress, unsigned char spriteSize, signed char alignX, signed char alignY)
{
    unsigned char i = 0;
    unsigned char color;
    unsigned char colsize = 0;

    // Object graphics config
    OBJECTS[index].gfxAddress = gfxAddress;
    OBJECTS[index].spriteAddress = 0x1FC08 + (8 * index);
    OBJECTS[index].alignX = alignX;
    OBJECTS[index].alignY = alignY;

    // Create a 64x64 256 color image
    VERA.address = OBJECTS[index].gfxAddress;
    VERA.address_hi = OBJECTS[index].gfxAddress>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;
    // Use all 256 colors in a 64x64 block
    color=255;
    if(spriteSize == 0)
    {
        colsize = 8 * 8;
    }
    else if(spriteSize == 0)
    {
        colsize = 16 * 16;
    }
    else if(spriteSize == 0)
    {
        colsize = 32 * 32;
    }
    else
    {
        colsize = 64 * 64;
    }
    for (i=0; i<colsize; i++) {
        VERA.data0 = color;
    }
    if(OBJECTS[index].flags & 0b00000010)
    {
        // Point to Sprite slot!
        VERA.address = OBJECTS[index].spriteAddress;
        VERA.address_hi = OBJECTS[index].spriteAddress>>16;
        // Set the Increment Mode, turn on bit 4
        VERA.address_hi |= 0b10000;
        // Graphic address bits 12:5
        VERA.data0 = OBJECTS[index].gfxAddress>>5;
        // 16 color mode, and graphic address bits 16:13
        VERA.data0 = 0b00000000 | OBJECTS[index].gfxAddress>>13;
        VERA.data0 = ((OBJECTS[index].x+OBJECTS[index].alignX)-VERA.layer0.hscroll);
        VERA.data0 = ((OBJECTS[index].x+OBJECTS[index].alignX)-VERA.layer0.hscroll)>>8;
        VERA.data0 = ((OBJECTS[index].y+OBJECTS[index].alignY)-VERA.layer0.vscroll);
        VERA.data0 = ((OBJECTS[index].y+OBJECTS[index].alignY)-VERA.layer0.vscroll)>>8;
        VERA.data0 = 0b00001100; // Z-Depth=3, Sprite in front of layer 1

        if(spriteSize == 0)
        {
            // 8x8
            VERA.data0 = 0b00000000;
        }
        else if(spriteSize == 1)
        {
            // 16x16
            VERA.data0 = 0b01010000;
        }
        else if(spriteSize == 2)
        {
            // 32x32
            VERA.data0 = 0b10100000;
        }
        else
        {
            // 64x64
            VERA.data0 = 0b11110000;
        }
    }
}

void HandleGameObjects() {
    // the mega function to rule them all
    unsigned char i = 0;
    while(i < currentObjects)
    {
        if((OBJECTS[i].flags & 0b00000001))
        {
            // always update oldpos!
            OBJECTS[i].oldx = OBJECTS[i].x;
            OBJECTS[i].oldy = OBJECTS[i].y;
            // game behaviors ahead!
            switch(OBJECTS[i].id)
            {
                case 0:
                    Gameobject_PlayerUpdate(i);
                break;

                case 1:
                    Gameobject_PlayerShotUpdate(i);
                break;
            }
        }
        i++;
    }
}

void DrawGameObjects() {
    unsigned char i = 0;
    while(i < currentObjects)
    {
        // check if object even has a sprite
        if((OBJECTS[i].flags & 0b00000011)) // enabled and visible
        {
            // Update Sprite X/Y Position
            VERA.address = OBJECTS[i].spriteAddress+2; // Point to Sprite byte 2
            VERA.address_hi = OBJECTS[i].spriteAddress>>16;
            // Set the Increment Mode, turn on bit 4
            VERA.address_hi |= 0b10000;
            // Set the X and Y values
            VERA.data0 = ((OBJECTS[i].x+OBJECTS[i].alignX)-VERA.layer0.hscroll);
            VERA.data0 = ((OBJECTS[i].x+OBJECTS[i].alignX)-VERA.layer0.hscroll)>>8;
            VERA.data0 = ((OBJECTS[i].y+OBJECTS[i].alignY)-VERA.layer0.vscroll);
            VERA.data0 = ((OBJECTS[i].y+OBJECTS[i].alignY)-VERA.layer0.vscroll)>>8;
        }
        else
        {
            // disable
            VERA.address = OBJECTS[i].spriteAddress+2; // Point to Sprite byte 2
            VERA.address_hi = OBJECTS[i].spriteAddress>>16;
            // Set the Increment Mode, turn on bit 4
            VERA.address_hi |= 0b10000;
            // Set the X and Y values
            VERA.data0 = ((OBJECTS[i].x+OBJECTS[i].alignX)-VERA.layer0.hscroll);
            VERA.data0 = ((OBJECTS[i].x+OBJECTS[i].alignX)-VERA.layer0.hscroll)>>8;
            VERA.data0 = ((OBJECTS[i].y+OBJECTS[i].alignY)-VERA.layer0.vscroll);
            VERA.data0 = ((OBJECTS[i].y+OBJECTS[i].alignY)-VERA.layer0.vscroll)>>8;
            VERA.data0 = 0b00000000; // Disable
        }
        i++;
    }
    // clear last slot
    if(currentObjects > 0)
    {
        if(!(OBJECTS[currentObjects-1].flags & 0b00000001))
        {
            // shorten the list!
            currentObjects -= 1;
        }
    }
}

void HandleGameObject_LockXY(unsigned char index, signed short min_left, signed short min_up,signed short max_right, signed short max_down)
{
    if(OBJECTS[index].x + 0xff < min_left + 0xff) // offset to avoid rollunder at 0
    {
        OBJECTS[index].x = min_left;
    }
    if(OBJECTS[index].x >= max_right) // we don't bother with overflow, we are likely not going to
    {
        OBJECTS[index].x = max_right;
    }
    if(OBJECTS[index].y + 0xff < min_up + 0xff) // offset to avoid rollunder at 0
    {
        OBJECTS[index].y = min_up;
    }
    if(OBJECTS[index].y >= max_down) // we don't bother with overflow, we are likely not going to
    {
        OBJECTS[index].y = max_down;
    }
}

void HandleGameObject_Velocity(unsigned char index) {
    unsigned char oldsubx = OBJECTS[index].subx;
    unsigned char oldsuby = OBJECTS[index].suby;

    // Update sprite XY based on velocities
    OBJECTS[index].x += OBJECTS[index].velx;
    OBJECTS[index].y += OBJECTS[index].vely;

    // subvelocities
    if(OBJECTS[index].subvelx != 0)
    {
        OBJECTS[index].subx += OBJECTS[index].subvelx;
        if(OBJECTS[index].subvelx > 0)
        {
            // if going positive, and suddenly smaller, we wrapped!
            if(OBJECTS[index].subx < oldsubx)
            {
                OBJECTS[index].x += 1;
            }
        }
        else
        {
            // if going neg, and suddenly bigger, we wrapped!
            if(OBJECTS[index].subx > oldsubx)
            {
                OBJECTS[index].x -= 1;
            }
        }
    }
    if(OBJECTS[index].subvely != 0)
    {
        OBJECTS[index].suby += OBJECTS[index].subvely;
        if(OBJECTS[index].subvely > 0)
        {
            // if going positive, and suddenly smaller, we wrapped!
            if(OBJECTS[index].suby < oldsuby)
            {
                OBJECTS[index].y += 1;
            }
        }
        else
        {
            // if going neg, and suddenly bigger, we wrapped!
            if(OBJECTS[index].suby > oldsuby)
            {
                OBJECTS[index].y -= 1;
            }
        }
    }
}


void HandleGameObject_SnapXY(unsigned char index,unsigned short x,unsigned short y) {
    OBJECTS[index].x = x;
    OBJECTS[index].subx = 0;
    OBJECTS[index].velx = 0;
    OBJECTS[index].subvelx = 0;
    OBJECTS[index].y = y;
    OBJECTS[index].suby = 0;
    OBJECTS[index].vely = 0;
    OBJECTS[index].subvely = 0;
}

void HandleGameObject_SnapX(unsigned char index,unsigned short x) {
    OBJECTS[index].x = x;
    OBJECTS[index].subx = 0;
    OBJECTS[index].velx = 0;
    OBJECTS[index].subvelx = 0;
}

void HandleGameObject_SnapY(unsigned char index,unsigned short y) {
    OBJECTS[index].y = y;
    OBJECTS[index].suby = 0;
    OBJECTS[index].vely = 0;
    OBJECTS[index].subvely = 0;
}

void HandleGameObject_AddVelocity(unsigned char index, signed char velx, signed char vely, signed char velsubx, signed char velsuby) {
    signed char oldsubx = OBJECTS[index].subvelx;
    signed char oldsuby = OBJECTS[index].subvely;

    // Update main velocity, as it's easy to do...
    OBJECTS[index].velx += velx;
    OBJECTS[index].vely += vely;

    // subvelocities
    if(velsubx != 0)
    {
        OBJECTS[index].subvelx += velsubx;
        if(velsubx > 0)
        {
            // if going positive, and suddenly smaller, we wrapped!
            if(OBJECTS[index].subvelx < oldsubx)
            {
                OBJECTS[index].velx += 1;
                OBJECTS[index].subvelx = 0;
            }
        }
        else
        {
            // if going neg, and suddenly bigger, we wrapped!
            if(OBJECTS[index].subvelx > oldsubx)
            {
                OBJECTS[index].velx -= 1;
                OBJECTS[index].subvelx = 0;
            }
        }
    }
    if(velsuby != 0)
    {
        OBJECTS[index].subvely += velsuby;
        if(velsuby > 0)
        {
            // if going positive, and suddenly smaller, we wrapped!
            if(OBJECTS[index].subvely < oldsuby)
            {
                OBJECTS[index].vely += 1;
                OBJECTS[index].subvely = 0;
            }
        }
        else
        {
            // if going neg, and suddenly bigger, we wrapped!
            if(OBJECTS[index].subvely > oldsuby)
            {
                OBJECTS[index].vely -= 1;
                OBJECTS[index].subvely = 0;
            }
        }
    }
}

void HandleGameObject_SetFacing(unsigned char index, unsigned char RL)
{
    unsigned char setSprite = 0b00001100;
    if(RL == 1)
    {
        OBJECTS[index].flags |= 0b10000000;
        setSprite = 0b00001101;
    }
    else
    {
        OBJECTS[index].flags &= 0b01111111;
        setSprite = 0b00001100;
    }

    // Point to Sprite slot!
    VERA.address = (OBJECTS[index].spriteAddress + 6);
    VERA.address_hi = (OBJECTS[index].spriteAddress + 6)>>16;
    // Set the Increment Mode, turn on bit 4
    VERA.address_hi |= 0b10000;
    VERA.data0 = setSprite;
}

unsigned char HandleGameObject_GetFacing(unsigned char index)
{
    return OBJECTS[index].flags & 0b10000000;
}