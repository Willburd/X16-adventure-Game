#ifndef JOY_H
#define JOY_H

extern unsigned char joy_status1, joy_status2, joy_status3;
extern unsigned char joy_prev1, joy_prev2, joy_prev3;
extern unsigned char buttonToJoy[13];

extern unsigned char gameButton_B;
extern unsigned char gameButton_Y;
extern unsigned char gameButton_SEL;
extern unsigned char gameButton_START;
extern unsigned char gameButton_UP;
extern unsigned char gameButton_DOWN;
extern unsigned char gameButton_LEFT;
extern unsigned char gameButton_RIGHT;
extern unsigned char gameButton_A;
extern unsigned char gameButton_X;
extern unsigned char gameButton_LT;
extern unsigned char gameButton_RT;
extern unsigned char gameButton_PLUG;

extern void DecodeJoystickBytes(unsigned char padIndex);

extern char getPadInputCurrent(unsigned char gameInput);
extern char getgetPadPressedPadInputPrevious(unsigned char gameInput);
extern char getPadPressed(unsigned char gameInput);
extern char getPadReleased(unsigned char gameInput);

#endif