#ifndef OBJPLAYER_H
#define OBJPLAYER_H

#define PLAYERJUMP 8

/*******************************************************************************
 * // PLAYER HANDLER
 *******************************************************************************/
extern unsigned char playerShots;
extern unsigned char playerJumped;
extern void Gameobject_PlayerInit(unsigned char index);
extern void Gameobject_PlayerUpdate(unsigned char index);


/*******************************************************************************
 * // PLAYER SHOTS HANDLER
 *******************************************************************************/
extern void Gameobject_PlayerShotInit(unsigned char index);
extern void Gameobject_PlayerShotUpdate(unsigned char index);

#endif