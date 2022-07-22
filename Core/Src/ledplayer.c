
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

uint32_t last_tick = 0, new_tick = 0, ticks_since = 0;

RGBColor_TypeDef rgb;
HSVColor_TypeDef hsv0;
HSVColor_TypeDef hsv1;

void ledplayer(void) {
	HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_SET);

	new_tick = HAL_GetTick();
	ticks_since = new_tick - last_tick;

	if(ticks_since>=50) {
		last_tick = new_tick;

		if(settings.channel[0].mode == CHANNEL_MODE_ON) {
			if(ledsupdated) {

				hsv0.s = 255;
				hsv0.v = 255;
				for (uint32_t i = 0; i < settings.channel[0].led_count; ++i) {
					hsv0.h += i*1;
					rgb = HSV2RGB(hsv0);
					pixel0[i].color.r = rgb.r;
					pixel0[i].color.g = rgb.g;
					pixel0[i].color.b = rgb.b;
				}
    			hsv0.h++;
    			led_trigger = true;
    		}
		}

		if(settings.channel[1].mode == CHANNEL_MODE_ON) {
			if(ledsupdated) {

  				hsv1.s = 255;
				hsv1.v = 255;
				for (uint32_t i = 0; i < settings.channel[1].led_count; ++i) {
					hsv1.h += i*1;
					rgb = HSV2RGB(hsv1);
					pixel1[i].color.r = rgb.r;
					pixel1[i].color.g = rgb.g;
					pixel1[i].color.b = rgb.b;
				}
   				hsv1.h++;
   				led_trigger = true;
			}
		}
	}

	if(led_trigger&&ledsupdated){
    	pBuff0 = dmaBuffer0;
    	for (uint32_t i = 0; i < settings.channel[0].led_count; i++) {
    		for (int j = 23; j >= 0; j--) {
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
    	for (uint32_t i = 0; i < settings.channel[1].led_count; i++) {
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