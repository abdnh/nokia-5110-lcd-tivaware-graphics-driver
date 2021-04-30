#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "Nokia5110_graphics.h"
#include "Nokia5110.h"

typedef void (*pxlFunc)(uint32_t, uint32_t);
pxlFunc pixelRoutines[2] = {Nokia5110_ClrPxl, Nokia5110_SetPxl};

static void pixelDraw(void *pvDisplayData, int32_t i32X, int32_t i32Y,
                      uint32_t ui32Value) {

    pixelRoutines[ui32Value](i32Y, i32X);
}

static void pixelDrawMultiple(void *pvDisplayData, int32_t i32X,
                            int32_t i32Y, int32_t i32X0,
                            int32_t i32Count, int32_t i32BPP,
                            const uint8_t *pui8Data,
                            const uint8_t *pui8Palette) {

    Nokia5110_PrintBMP(i32X, i32Y, pui8Data, 0);
}

static void lineDrawH(void *pvDisplayData, int32_t i32X1, int32_t i32X2,
                      int32_t i32Y, uint32_t ui32Value) {
    int32_t j;
    for(j = i32X1; j < i32X2; j++) {
        pixelRoutines[ui32Value](i32Y, j);
    }
}

static void lineDrawV(void *pvDisplayData, int32_t i32X, int32_t i32Y1,
                      int32_t i32Y2, uint32_t ui32Value) {
    int32_t i;
    for(i = i32Y1; i < i32Y2; i++) {
        pixelRoutines[ui32Value](i, i32X);
    }
}

static void rectFill(void *pvDisplayData, const tRectangle *psRect, uint32_t ui32Value) {
    int16_t i, j;
    for(i = psRect->i16YMin; i <= psRect->i16YMax; i++) {
        for(j = psRect->i16XMin; j <= psRect->i16XMax; j++) {
            pixelRoutines[ui32Value](i, j);
        }
    }
}

#define THRESHOLD 0x7FFFFF

static uint32_t colorTranslate(void *pvDisplayData, uint32_t ui32Value) {
    return (uint32_t)(ui32Value < THRESHOLD);
}

static void flush(void *pvDisplayData) {
    Nokia5110_DisplayBuffer();
}

extern tDisplay Nokia5110_driver = {
    .i32Size = sizeof(tDisplay),
    .pvDisplayData = NULL,
    .ui16Width = MAX_X,
    .ui16Height = MAX_Y,
    .pfnPixelDraw = pixelDraw,
    .pfnPixelDrawMultiple = pixelDrawMultiple,
    .pfnLineDrawH = lineDrawH,
    .pfnLineDrawV = lineDrawV,
    .pfnRectFill = rectFill,
    .pfnColorTranslate = colorTranslate,
    .pfnFlush = flush,
};
