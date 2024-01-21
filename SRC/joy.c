#include "joy.h"

unsigned char joy_status1 = 0;
unsigned char joy_status2 = 0;
unsigned char joy_status3 = 0;
unsigned char joy_prev1 = 0;
unsigned char joy_prev2 = 0;
unsigned char joy_prev3 = 0;
unsigned char buttonToJoy[13] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001, 0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b11111111};

unsigned char gameButton_B     = 0b00000000;
unsigned char gameButton_Y     = 0b00000001;
unsigned char gameButton_SEL   = 0b00000010;
unsigned char gameButton_START = 0b00000011;
unsigned char gameButton_UP    = 0b00000100;
unsigned char gameButton_DOWN  = 0b00000101;
unsigned char gameButton_LEFT  = 0b00000110;
unsigned char gameButton_RIGHT = 0b00000111;
unsigned char gameButton_A     = 0b00001000;
unsigned char gameButton_X     = 0b00001001;
unsigned char gameButton_LT    = 0b00001010;
unsigned char gameButton_RT    = 0b00001011;
unsigned char gameButton_PLUG  = 0b00001100;

void DecodeJoystickBytes(unsigned char padIndex) {
    // Here are the bits for each button/pad on the joystick
    /*
    .A, byte 0:  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
            SNES | B | Y |SEL|STA|UP |DN |LT |RT |

    .X, byte 1:  | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
            SNES | A | X | L | R | 1 | 1 | 1 | 1 |

    We aren't using this one but we could if we wanted to check
    if the joystick was actually present or not
    .Y, byte 2:
        $00 = joystick present
        $FF = joystick not present
    */

    // IMPORTANT: Bits will be 0 when that button/pad is pressed, NOT 1.
    // This means all bits are 1 when nothing is pressed.
    // So to check a bit, we do an AND (&) but NEGATE (!) to flip it
    joy_prev1 = joy_status1;
    joy_prev2 = joy_status2; 
    joy_prev3 = joy_status3;

    // Check joystick 0 (the default)
    if(padIndex == 0x01)
    {
        asm("lda #1");
    }
    else
    {
        asm("lda #0");
    }

    // Call the "joystick_get" Kernal function at 0xFF56
    asm("jsr $FF56");

    // Get the status bytes from the A and X registers
    asm("STA %v", joy_status1);
    asm("STX %v", joy_status2);
    asm("STY %v", joy_status3);
}

char getPadInputCurrent(unsigned char gameInput)
{
    if(gameInput == gameButton_PLUG)
        return !(joy_status3 & 0b11111111);
    if(gameInput >= gameButton_A)
        return !(joy_status2 & buttonToJoy[gameInput]);
    return !(joy_status1 & buttonToJoy[gameInput]);
}

char getPadInputPrevious(unsigned char gameInput)
{
    if(gameInput == gameButton_PLUG)
        return !(joy_prev3 & 0b11111111);
    if(gameInput >= gameButton_A)
        return !(joy_prev2 & buttonToJoy[gameInput]);
    return !(joy_prev1 & buttonToJoy[gameInput]);
}

char getPadPressed(unsigned char gameInput)
{
    return getPadInputPrevious(gameInput) == 0 && getPadInputCurrent(gameInput);
}

char getPadReleased(unsigned char gameInput)
{
    return getPadInputPrevious(gameInput) && getPadInputCurrent(gameInput) == 0;
}