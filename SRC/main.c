#include <cx16.h>
#include <6502.h>
#include <stdio.h>
#include "levelloader/levels.h"
#include "objects.h"
#include "controller.h"
#include "joy.h"

#define IRQ_HANDLER_STACK_SIZE 8
unsigned char vsync_set = 0x0;
unsigned char irqHandlerStack[IRQ_HANDLER_STACK_SIZE];
unsigned char irqHandler() {
    if (VERA.irq_flags & 0b00000001) {
        if(vsync_set == 0x0)
        {
            // VSYNC READY
            vsync_set = 0xff;
        }
        return IRQ_HANDLED;
    }
    return IRQ_NOT_HANDLED;
}

void main() {
    // Setup the IRQ handler for sprite collisions
    set_irq(&irqHandler, irqHandlerStack, IRQ_HANDLER_STACK_SIZE);

    // setup layers for gameplay and hud
    InitLayers();
    
    // Enable sprites, set scale
    VERA.display.video |= 0b01000000;
    VERA.display.hscale = 32;
    VERA.display.vscale = 32;
    VERA.layer0.hscroll = 0;
    VERA.layer0.vscroll = 0;

    while (1) {
        if(vsync_set == 0xff)
        {
            // update graphics
            UpdateVeraScroll();
            DrawGameObjects();
            // handle next frame's logic
            MainControllerUpdate();
            vsync_set = 0x0;
        }
    }
}