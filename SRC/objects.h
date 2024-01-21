#ifndef OBJECTS_H
#define OBJECTS_H

#define MAXOBJS 120
#define GRAVITY 100
#define SCREENFLOOR 120

// OBJECT LIST
#define OBJ_PLAYER      0x0
#define OBJ_PLAYERSHOT  0x1


extern struct Gameobj {
    unsigned char id;
    // XY pos and subpixels
    unsigned short x;
    unsigned short y;
    unsigned short oldx;
    unsigned short oldy;
    unsigned char subx;
    unsigned char suby;
    // XY velocities, and subpixels
    signed char subvelx;
    signed char subvely;
    signed char velx;
    signed char vely;
    // flags and graphics
    unsigned char flags; // 0b[FLIP]00000[VISIBLE][ENABLED]
    unsigned char timer;
    unsigned long gfxAddress;
    unsigned char gfxState;
    unsigned long spriteAddress;
    // alignXY
    signed char alignX;
    signed char alignY;
};

extern unsigned char currentObjects;

extern unsigned char InitGameObject(unsigned char getid, unsigned short x,unsigned short y, unsigned char flags);

extern void HandleGameObjectGraphicsInit(unsigned char index, unsigned long gfxAddress, unsigned char spriteSize, signed char alignX, signed char alignY);

extern void HandleGameObjects();

extern void DrawGameObjects();

extern void HandleGameObject_LockXY(unsigned char index, signed short min_left, signed short min_up,signed short max_right, signed short max_down);

extern void HandleGameObject_Velocity(unsigned char index);

extern void HandleGameObject_SnapXY(unsigned char index,unsigned short x,unsigned short y);

extern void HandleGameObject_SnapX(unsigned char index,unsigned short x);

extern void HandleGameObject_SnapY(unsigned char index,unsigned short y);

extern void HandleGameObject_AddVelocity(unsigned char index, signed char velx, signed char vely, signed char velsubx, signed char velsuby);

extern void HandleGameObject_SetFacing(unsigned char index, unsigned char RL);

extern unsigned char HandleGameObject_GetFacing(unsigned char index);

extern struct Gameobj OBJECTS[MAXOBJS];

#endif