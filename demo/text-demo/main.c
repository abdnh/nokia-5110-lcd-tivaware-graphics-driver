#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "grlib/grlib.h"

#include "Nokia5110_graphics.h"
#include "Nokia5110.h"

extern tFont *g_psFontWNotosans10pt;

tCodePointMap codepointMaps[] = {
    { CODEPAGE_UTF_8, CODEPAGE_UNICODE, GrMapUTF8_Unicode},
};

#define CODEPOINT_MAPS_NUM (sizeof(codepointMaps) / sizeof(tCodePointMap))

tGrLibDefaults g_sGrLibDefaultlanguage = {
    GrDefaultStringRenderer,
    codepointMaps,
    CODEPAGE_UTF_8,
    CODEPOINT_MAPS_NUM
};

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
    Nokia5110_Init();

    GrLibInit(&g_sGrLibDefaultlanguage);
    tContext context;
    GrContextInit(&context, &Nokia5110_driver);

    GrContextFontSet(&context, g_psFontWNotosans10pt);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);

    Nokia5110_Clear();
    GrContextForegroundSetTranslated(&context, 1);
    GrStringDraw(&context, "Hello world!", -1, 15, 10, 1);
    GrFlush(&context);
    SysCtlDelay(SysCtlClockGet()*2);
    Nokia5110_Clear();
    GrStringDraw(&context, "Merhaba dünya!", -1, 5, 10, 1);
    GrFlush(&context);

    while(1) {}

}
