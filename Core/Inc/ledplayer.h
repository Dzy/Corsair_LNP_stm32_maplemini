#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "tim.h"

typedef struct {
    uint8_t b;
    uint8_t r;
    uint8_t g;
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

#define MAX_NUM_PIXELS0 60
#define MAX_NUM_PIXELS1 60

#define DMA_BUFF_SIZE0 ((MAX_NUM_PIXELS0+2)*3*8)
#define DMA_BUFF_SIZE1 ((MAX_NUM_PIXELS1+2)*3*8)

typedef struct {
	uint8_t mode;
	uint8_t led_count;
} Channel_TypeDef;

typedef struct {
	uint8_t channel_count;
	Channel_TypeDef channel[2];
} __attribute__((aligned(4))) Settings_TypeDef;

RGBColor_TypeDef HSV2RGB(HSVColor_TypeDef hsv);
HSVColor_TypeDef RGB2HSV(RGBColor_TypeDef rgb);

void ledplayer(void);

#define READ_STATUS                    0x01
#define READ_FIRMWARE_VERSION          0x02
#define READ_DEVICE_ID                 0x03
#define WRITE_DEVICE_ID                0x04
#define START_FIRMWARE_UPDATE          0x05
#define READ_BOOTLOADER_VERSION        0x06
#define WRITE_TEST_FLAG                0x07

#define READ_TEMPERATURE_MASK          0x10
#define READ_TEMPERATURE_VALUE         0x11
#define READ_VOLTAGE_VALUE             0x12

#define READ_FAN_MASK                  0x20
#define READ_FAN_SPEED                 0x21
#define READ_FAN_POWER                 0x22
#define WRITE_FAN_POWER                0x23
#define WRITE_FAN_SPEED                0x24
#define WRITE_FAN_CURVE                0x25
#define WRITE_FAN_EXTERNAL_TEMP        0x26
#define WRITE_FAN_FORCE_THREE_PIN_MODE 0x27
#define WRITE_FAN_DETECTION_TYPE       0x28
#define READ_FAN_DETECTION_TYPE        0x29

#define READ_LED_STRIP_MASK            0x30
#define WRITE_LED_RGB_VALUE            0x31
#define WRITE_LED_COLOR_VALUES         0x32
#define WRITE_LED_TRIGGER              0x33
#define WRITE_LED_CLEAR                0x34
#define WRITE_LED_GROUP_SET            0x35
#define WRITE_LED_EXTERNAL_TEMP        0x36
#define WRITE_LED_GROUP_CLEAR          0x37
#define WRITE_LED_MODE                 0x38
#define WRITE_LED_BRIGHTNESS           0x39
#define WRITE_LED_COUNT                0x3A
#define WRITE_LED_PORT_TYPE            0x3B
#define WRITE_LED_START_AUTODETECTION  0x3C
#define READ_LED_AUTODETECTION_RESULTS 0x3D

#define PROTOCOL_RESPONSE_OK           0x00
#define PROTOCOL_RESPONSE_ERROR        0x01
#define PROTOCOL_STATUS_OK             0x00
#define PROTOCOL_STATUS_ERROR          0xFF

#define CHANNEL_MODE_DISABLED 0x00
#define CHANNEL_MODE_ON 0x01
#define CHANNEL_MODE_SOFTWARE_PLAYBACK 0x02