#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "grlib/grlib.h"

#include "Nokia5110_graphics.h"
#include "Nokia5110.h"

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
    Nokia5110_Init();
    tContext context;
    GrContextInit(&context, &Nokia5110_driver);

    // enable the Nokia 5110 LCD's LEDs on pin A4
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);

    Nokia5110_Clear();
    GrContextForegroundSetTranslated(&context, 1);
    GrLineDrawH(&context, 10, MAX_X - 10, MAX_Y/2);
    GrLineDrawV(&context, MAX_X/2, 10, MAX_Y - 10);
    GrLineDraw(&context, 10, 10, MAX_X - 10, MAX_Y - 10);
    GrLineDraw(&context, 10, MAX_Y - 10, MAX_X - 10, 10);
    tRectangle rect = {.i16XMin = MAX_X/2 - 10, .i16XMax = MAX_X/2 + 10, .i16YMin = MAX_Y/2 - 10, .i16YMax = MAX_Y/2 + 10};
    GrRectDraw(&context, &rect);
    GrFlush(&context);

    SysCtlDelay(SysCtlClockGet()*2);
    Nokia5110_ClearBuffer();
    // draw an astonished face
    GrCircleDraw(&context, MAX_X/2, MAX_Y/2, 23);
    GrCircleDraw(&context, MAX_X/2 - 10, MAX_Y/2 - 10, 5);
    GrCircleDraw(&context, MAX_X/2 + 10, MAX_Y/2 - 10, 5);
    GrCircleFill(&context, MAX_X/2, MAX_Y/2 + 10, 3);
    GrFlush(&context);

    while(1) {}

}
