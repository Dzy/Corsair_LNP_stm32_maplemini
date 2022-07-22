#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "tim.h"

typedef struct {
	bool standalone;
	uint8_t led_count_channel0;
	uint8_t led_count_channel1;
} __attribute__((aligned(4))) Settings_TypeDef;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGBColor_TypeDef;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HSVColor_TypeDef;

typedef union {
	RGBColor_TypeDef color;
	uint32_t data;
} PixelRGB_TypeDef;

typedef union {
	HSVColor_TypeDef color;
	uint32_t data;
} PixelHSV_TypeDef;

#define NEOPIXEL_ZERO 27
#define NEOPIXEL_ONE  57
#define NEOPIXEL_RESET 0

#define MAX_NUM_PIXELS0 12
#define MAX_NUM_PIXELS1 12

#define DMA_BUFF_SIZE0 ((24+2)*24)
#define DMA_BUFF_SIZE1 ((24+2)*24)

void ledplayer(void);