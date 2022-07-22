
#include "ledplayer.h"
#include "main.h"

Settings_TypeDef settings;

PixelRGB_TypeDef pixel0[(MAX_NUM_PIXELS0+2)];
uint32_t dmaBuffer0[DMA_BUFF_SIZE0];
uint32_t *pBuff0;

PixelRGB_TypeDef pixel1[(MAX_NUM_PIXELS0+2)];
uint32_t dmaBuffer1[DMA_BUFF_SIZE1];
uint32_t *pBuff1;

bool led_trigger = false;
bool ledsupdated = true;
bool ready2run   = false;

void ledplayer(void) {
	HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_SET);
	if(settings.standalone) {
		//printf("Standalone %lu\n", (uint32_t)HAL_GetTick());

		RGBColor_TypeDef rgb;
		HSVColor_TypeDef hsv;

		uint8_t hue = 0;

		/*
		for (int i = 0; i < NUM_LEDS; ++i) {
			leds[i] = CHSV(hue + (i * 10), 255, 255);
		}

    	hue++;
		*/
	} else {
	}

	if(led_trigger&&ledsupdated){
    	pBuff0 = dmaBuffer0;
    	for (uint32_t i = 0; i < settings.led_count_channel0; i++) {
    		for (int32_t j = 23; j >= 0; j--) {
        		if ((pixel0[i].data >> j) & 0x01) {
        			*pBuff0 = NEOPIXEL_ONE;
        		} else {
        			*pBuff0 = NEOPIXEL_ZERO;
        		}
        			pBuff0++;
    			}
    	}
    	*pBuff0++ = NEOPIXEL_RESET;
    	*pBuff0++ = NEOPIXEL_RESET;

    	pBuff1 = dmaBuffer1;
    	for (uint32_t i = 0; i < settings.led_count_channel1; i++) {
    		for (int32_t j = 23; j >= 0; j--) {
        	if ((pixel1[i].data >> j) & 0x01) {
        		*pBuff1 = NEOPIXEL_ONE;
        	} else {
        		*pBuff1 = NEOPIXEL_ZERO;
        	}
        		pBuff1++;
    		}
    	}
    	*pBuff1++ = NEOPIXEL_RESET;
    	*pBuff0++ = NEOPIXEL_RESET;

    	ledsupdated = false;
    	led_trigger = false;

    	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, dmaBuffer0, DMA_BUFF_SIZE0);
    	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, dmaBuffer1, DMA_BUFF_SIZE1);
	}

    HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_RESET);
}

RGBColor_TypeDef HSV2RGB(HSVColor_TypeDef hsv) {
    RGBColor_TypeDef rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0) {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
        default:
            rgb.r = hsv.v; rgb.g = p; rgb.b = q;
            break;
    }

    return rgb;
}

HSVColor_TypeDef RGB2HSV(RGBColor_TypeDef rgb) {
    HSVColor_TypeDef hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0) {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * (int32_t)(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0) {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
    else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

    return hsv;
}