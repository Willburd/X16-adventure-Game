#include <cx16.h>
#include "player.h"
#include "../joy.h"
#include "../objects.h"

/*******************************************************************************
 * // PLAYER HANDLER
 *******************************************************************************/
unsigned char playerShots = 0;
unsigned char playerJumped = 0;
void Gameobject_PlayerInit(unsigned char index)
{   
    HandleGameObjectGraphicsInit(index, 0x1000, 1, -8, -16);
}

void Gameobject_PlayerUpdate(unsigned char index)
{
    unsigned char newobj = 0;
    unsigned char noLRinput;
    unsigned char jumping = 0;
    unsigned char cancelledJump = 0;

    // handle last frame's movement
    HandleGameObject_Velocity(index);

    // test player
    noLRinput = 1;
    if(getPadInputCurrent(gameButton_LEFT))
    {
        if(OBJECTS[index].velx > -3)
        {
            HandleGameObject_AddVelocity(index, 0, 0, -110, 0); 
        }
        noLRinput = 0;
        if( HandleGameObject_GetFacing(index) != 1)
        {
            HandleGameObject_SetFacing(index, 1);
        }
    }
    if(getPadInputCurrent(gameButton_RIGHT))
    {
        if(OBJECTS[index].velx < 3)
        {
            HandleGameObject_AddVelocity(index, 0, 0, +110, 0); 
        }
        noLRinput = 0;
        if( HandleGameObject_GetFacing(index) != 0)
        {
            HandleGameObject_SetFacing(index, 0);
        }
    }

    // stopping
    if(noLRinput > 0)
    {
        if(OBJECTS[index].velx > 0)
        {
            HandleGameObject_AddVelocity(index, 0, 0, -120, 0); 
            if(OBJECTS[index].velx <= 0)
            {
                OBJECTS[index].velx = 0;
            }
        }
        if(OBJECTS[index].velx < 0)
        {
            HandleGameObject_AddVelocity(index, 0, 0, +120, 0); 
            if(OBJECTS[index].velx >= 0)
            {
                OBJECTS[index].velx = 0;
            }
        }
    }

    // jumping
    if(getPadInputCurrent(gameButton_B))
    {
        if(playerJumped == 0)
        {
            jumping = 1;
        }
        playerJumped = 1;
    }
    else
    {
        if(playerJumped)
        {
            cancelledJump = 1;
        }
        playerJumped = 0;
    }

    // gravity and falling!
    if(OBJECTS[index].y >= SCREENFLOOR)
    {
        // lock on floor
        HandleGameObject_SnapY(index,SCREENFLOOR);
        if(jumping)
        {
            HandleGameObject_AddVelocity(index, 0, -PLAYERJUMP, 0, 0);
        }
    }
    else
    {
        // jump cancelling and gravity
        HandleGameObject_AddVelocity(index, 0, 0, 0, GRAVITY);
        if(OBJECTS[index].vely < -1 && cancelledJump)
        {
            HandleGameObject_AddVelocity(index, 0, 4, 0, 0);
        }
    }

    // handle player
    if(getPadPressed(gameButton_A))
    {
        if( playerShots < 3 )
        {
            playerShots += 1;
            newobj = InitGameObject( OBJ_PLAYERSHOT, OBJECTS[index].x, OBJECTS[index].y - 8, 0b00000011);
            if(newobj != 0xff)
            {
                if(HandleGameObject_GetFacing(index) == 0)
                {
                    OBJECTS[newobj].velx = 7;
                }
                else
                {
                    OBJECTS[newobj].velx = -7;
                }
            }
        }
    }
    
    HandleGameObject_LockXY(index,8,0,1024,256);
}


/*******************************************************************************
 * // PLAYER SHOTS HANDLER
 *******************************************************************************/
void Gameobject_PlayerShotInit(unsigned char index)
{
    OBJECTS[index].timer = 0;
    HandleGameObjectGraphicsInit(index, 0x1000, 0, -4, -4);
}

void Gameobject_PlayerShotUpdate(unsigned char index)
{
    // handle last frame's movement
    HandleGameObject_Velocity(index);
    // despawn shots
    if(((OBJECTS[index].x+OBJECTS[index].alignX)-VERA.layer0.hscroll) + 0xff < (0xff-10)) // 255 offset to allow for checking under 0
    {
        OBJECTS[index].flags = 0;
        playerShots -= 1;
    }
    if(((OBJECTS[index].x+OBJECTS[index].alignX)-VERA.layer0.hscroll) > 240)
    {
        OBJECTS[index].flags = 0;
        playerShots -= 1;
    }
}