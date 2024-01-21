#include <cx16.h>
#include "levelloader/levels.h"
#include "controller.h"
#include "objects.h"
#include "joy.h"

unsigned char gameState = 0b00000001; // 0b[TITLE,MENU,GAME,ENDING XX][FLAGS XXXX][DEBUG][PLAYING]

void MainControllerUpdate()
{
    // get gamepad inputs
    DecodeJoystickBytes(0);
    if((gameState & 0b11000000) == 0b00000000)
    {
        // title mode
        if(getPadPressed(gameButton_START))
        {
            // START!
            InitGameObject( OBJ_PLAYER, 50, 50, 0b00000011);
            gameState |= 0b10000001; // enter game state, unpaused
            // gameState |= 0b01000001; // enter menu state, unpaused
        }
    }
    else if((gameState & 0b11000000) == 0b01000000)
    {
        // menu mode

    }
    else if((gameState & 0b11000000) == 0b10000000)
    {
        // game mode
        if(getPadPressed(gameButton_START))
        {
            // pause/unpause
            gameState ^= 0b00000001;
        }

        // update objects
        if(gameState & 0b00000001)
        {
            
            // Update all loaded objects
            HandleGameObjects();
            
            // ASSUMES PLAYER IN SLOT 0
            // handle scrolling
            if(((OBJECTS[0].x+OBJECTS[0].alignX)-VERA.layer0.hscroll) > 72)
            {
                if(OBJECTS[0].oldx < OBJECTS[0].x)
                {
                    LevelScrollMove( OBJECTS[0].x - OBJECTS[0].oldx, 0);
                }
            }
            if(((OBJECTS[0].x+OBJECTS[0].alignX)-VERA.layer0.hscroll) < 72)
            {
                if(OBJECTS[0].oldx > OBJECTS[0].x)
                {
                    LevelScrollMove( OBJECTS[0].x - OBJECTS[0].oldx, 0);
                }
            }

            // locking
            LevelScrollLock(0,0,64,0);
        }
    }
    else if((gameState & 0b11000000) == 0b11000000)
    {
        // ending mode

    }
}