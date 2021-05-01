#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/checkbox.h"

#include "Nokia5110_graphics.h"
#include "Nokia5110.h"

tContext context;

#define FONT &g_sFontCmss12

void onRadioChange(tWidget *psWidget, uint8_t p, uint32_t bSelected) {
    uint8_t v = 0;
    if(bSelected) v = p;
    GPIOPinWrite(GPIO_PORTF_BASE, p, v);
}

void onRedChange(tWidget *psWidget, uint32_t bSelected) {
    onRadioChange(psWidget, GPIO_PIN_1, bSelected);
}

void onBlueChange(tWidget *psWidget, uint32_t bSelected) {
    onRadioChange(psWidget, GPIO_PIN_2, bSelected);
}

void onGreenChange(tWidget *psWidget, uint32_t bSelected) {
    onRadioChange(psWidget, GPIO_PIN_3, bSelected);
}

#define ColorCheckbox(\
    x, y, width, height, \
    style, fillColor, \
    outlineColor, textColor, text, onChange) \
    CheckBoxStruct(\
        WIDGET_ROOT, \
        NULL, \
        NULL, \
        &Nokia5110_driver, \
        x, y, width, height, \
        style, 8, \
        fillColor, outlineColor, textColor, \
        FONT, text, NULL, onChange)


#define BUTTONS_NUM 3
tCheckBoxWidget buttons[BUTTONS_NUM] = {
    ColorCheckbox(
        10, 0, 50, 12,
        CB_STYLE_TEXT, ~0, 0, 0,
        "Red", onRedChange)
    ,
    ColorCheckbox(
        10, 15, 50, 12,
        CB_STYLE_TEXT, ~0, 0, 0,
        "Blue", onBlueChange)
    ,
    ColorCheckbox(
        10, 30, 50, 12,
        CB_STYLE_TEXT, ~0, 0, 0,
        "Green", onGreenChange)
};
int16_t selected = -1;

void enable_peripheral_and_wait(uint32_t p) {
    SysCtlPeripheralEnable(p);
    while(!SysCtlPeripheralReady(p))
    {
    }
}

#define SWITCH_PINS GPIO_PIN_0|GPIO_PIN_4

void on_switch(void) {
    GPIOIntClear(GPIO_PORTF_BASE, SWITCH_PINS);
    uint32_t pin4 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
    uint32_t pin0 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
    if(!pin4) {
        if(selected >= 0) {
            tCheckBoxWidget *prev = &buttons[selected];
            GrContextForegroundSet(&context, ~0);
            GrRectDraw(&context, &prev->sBase.sPosition);
            WidgetPaint(&prev->sBase);
        }
        selected = (selected + 1) % BUTTONS_NUM;
        tCheckBoxWidget *btn = &buttons[selected];
        GrContextForegroundSet(&context, 0);
        GrRectDraw(&context, &btn->sBase.sPosition);
        WidgetPaint(&btn->sBase);
    }
    else if(!pin0 && selected >= 0) {
        tCheckBoxWidget *btn = &buttons[selected];
        WidgetMessageQueueAdd(&btn->sBase, WIDGET_MSG_PTR_UP,
        btn->sBase.sPosition.i16XMin, btn->sBase.sPosition.i16YMin, false, true);
    }
    WidgetMessageQueueProcess();
    GrFlush(&context);
}

void switches_setup(void) {

    GPIOUnlockPin(GPIO_PORTF_BASE, SWITCH_PINS);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SWITCH_PINS);
    GPIOIntRegister(GPIO_PORTF_BASE, on_switch);
    GPIOPadConfigSet(GPIO_PORTF_BASE, SWITCH_PINS, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, SWITCH_PINS, GPIO_FALLING_EDGE);
    GPIOIntClear(GPIO_PORTF_BASE, SWITCH_PINS);
    GPIOIntEnable(GPIO_PORTF_BASE, SWITCH_PINS);
}

int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_INT | SYSCTL_XTAL_16MHZ);
    Nokia5110_Init();
    GrContextInit(&context, &Nokia5110_driver);

    enable_peripheral_and_wait(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, GPIO_PIN_4);

    enable_peripheral_and_wait(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    switches_setup();

    Nokia5110_Clear();
    GrContextForegroundSetTranslated(&context, 1);
    GrContextFontSet(&context, FONT);

    unsigned i;
    for(i = 0; i < BUTTONS_NUM; i++) {
        WidgetPaint(&buttons[i].sBase);
        WidgetAdd(WIDGET_ROOT, &buttons[i].sBase);
    }
    WidgetMessageQueueProcess();
    GrFlush(&context);

    while(1) {};
}
