/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "ledplayer.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN uint8_t report[16] __ALIGN_END;
extern uint8_t device_serial_32[4];

extern Settings_TypeDef settings;
extern bool led_trigger;
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
static int8_t CUSTOM_HID_SendOk(uint8_t *buffer, size_t size);
static int8_t CUSTOM_HID_SendError(void);
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */
typedef union {
  struct {
    uint8_t b;
    uint8_t r;
    uint8_t g;
  } color;
  uint32_t data;
} PixelRGB_t;

extern PixelRGB_t pixel0[];
extern PixelRGB_t pixel1[];

extern uint32_t dmaBuffer0[];
extern uint32_t dmaBuffer1[];

extern uint32_t *pBuff0;
extern uint32_t *pBuff1;

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
  0x06, 0xC0, 0xFF,  // Usage Page (Vendor Defined 0xFFC0)
  0x09, 0x01,        // Usage (0x01)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x10,        //   Report Count (16)
  0x09, 0x01,        //   Usage (0x01)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x40,        //   Report Count (64)
  0x09, 0x01,        //   Usage (0x01)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x00,        //   Report Count (0)
  0x09, 0x01,        //   Usage (0x01)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  //HAL_GPIO_TogglePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin);

  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;

  uint8_t *corsairLightingNodeMessage, *msg;
  corsairLightingNodeMessage = hhid->Report_buf;
  msg = corsairLightingNodeMessage;

  switch(*corsairLightingNodeMessage) {
    case READ_FIRMWARE_VERSION: {
      printf("READ_FIRMWARE_VERSION\n");
      uint8_t corsairLightingNodeReply[16] = {0x00, 0x0A, 0x04};
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 3);
      break;
    }

    case READ_DEVICE_ID: {
      printf("READ_DEVICE_ID\n");
      CUSTOM_HID_SendOk(device_serial_32, 4);
      break;
    }

    case READ_BOOTLOADER_VERSION: {
      printf("READ_BOOTLOADER_VERSION\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_TRIGGER: {
      printf("WRITE_LED_TRIGGER\n");
      if(led_trigger){
        //printf("    error\n");
        CUSTOM_HID_SendError();
        break;
      } else {
        led_trigger = true;
      }
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

/*
iCUE shuts down
WRITE_LED_GROUP_CLEAR
WRITE_LED_CLEAR 34 0 0 0 0 0
WRITE_LED_MODE Channel(0) Mode(on)
WRITE_LED_GROUP_SET
WRITE_LED_GROUP_CLEAR
WRITE_LED_CLEAR 34 1 0 0 0 0
WRITE_LED_MODE Channel(1) Mode(on)
WRITE_LED_GROUP_SET
*/

    case WRITE_LED_MODE: {
      printf("WRITE_LED_MODE Channel(%d) Mode(%d)\n", msg[1], msg[2]);

      if(msg[1]<=settings.channel_count) {
        settings.channel[msg[1]].mode = msg[2];
      }

      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }
/*
WRITE_LED_CLEAR 34 0 0 0 0 0
WRITE_LED_CLEAR 34 1 0 0 0 0
*/
    case WRITE_LED_CLEAR: {
      printf("WRITE_LED_CLEAR %.02x %d\n",msg[0], msg[1]);
      if(msg[1]<=settings.channel_count) {
        if(settings.channel[msg[1]].mode == CHANNEL_MODE_SOFTWARE_PLAYBACK){
          for (uint32_t i = 0; i < settings.channel[msg[1]].led_count; ++i) {
            if(msg[1] == 0){
              pixel0[i].data = 0;
            }else{
              pixel1[i].data = 0;
            }
          }
        }
      }
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_GROUP_SET: {
      printf("WRITE_LED_GROUP_SET\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_GROUP_CLEAR: {
      printf("WRITE_LED_GROUP_CLEAR\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_BRIGHTNESS: {
      printf("WRITE_LED_BRIGHTNESS\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_PORT_TYPE: {
      printf("WRITE_LED_PORT_TYPE %.02x %d %d %d %d %d\n",msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_COUNT: {
      printf("WRITE_LED_COUNT %.02x %d %d %d %d %d\n",msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }  
/*
32 0 0 l(12) [R] i(00) 28 i(01) 69 i(02) ab i(03) ec i(04) ff i(05) ff i(06) ff i(07) ff i(08) ff i(09) ff i(0a) ff i(0b) ff 
32 0 0 l(12) [G] i(00) d1 i(01) df i(02) ed i(03) fb i(04) ff i(05) ff i(06) ff i(07) ff i(08) ca i(09) 89 i(0a) 47 i(0b) 06 
32 0 0 l(12) [B] i(00) d7 i(01) 96 i(02) 54 i(03) 13 i(04) 2e i(05) 70 i(06) b1 i(07) f3 i(08) f4 i(09) e5 i(0a) d7 i(0b) c9

32 1 0 l(12) [R] i(00) ff i(01) ff i(02) ff i(03) ff i(04) e7 i(05) a6 i(06) 64 i(07) 23 i(08) 1e i(09) 60 i(0a) a1 i(0b) e2 
32 1 0 l(12) [G] i(00) 11 i(01) 53 i(02) 94 i(03) d5 i(04) fa i(05) ec i(06) de i(07) d0 i(08) cf i(09) dd i(0a) eb i(0b) f9 
32 1 0 l(12) [B] i(00) cc i(01) da i(02) e8 i(03) f6 i(04) ff i(05) ff i(06) ff i(07) ff i(08) e1 i(09) 9f i(0a) 5e i(0b) 1d

settings.channel[msg[1]].led_count

32 1  0 l(50)
32 1 50 l(10)

*/
    case WRITE_LED_COLOR_VALUES: {
      //printf("WRITE_LED_COLOR_VALUES\n");

      printf("%.02x %d %d l(%d) %d\n",msg[0], msg[1], msg[2], msg[3], msg[4]);

      if(msg[4] == 0) {
        int32_t len = (uint32_t)msg[3];
        int32_t off = (uint32_t)msg[2];
        int32_t max = (uint32_t)settings.channel[msg[1]].led_count;

        if((len+off)>settings.channel[msg[1]].led_count) {
          len -= (off+len)-settings.channel[msg[1]].led_count;
          printf("  NEW len %d (%d-%d) (max%d)\n", len, off, off+len, max);
        }

        for(int i = 0; i<len; i++) {
          if(msg[1] == 0) {
            pixel0[i+off].color.r = msg[i+5];
          } else {
            pixel1[i+off].color.r = msg[i+5];
          }
        }
      }

      if(msg[4] == 1) {
        int32_t len = (uint32_t)msg[3];
        int32_t off = (uint32_t)msg[2];
        int32_t max = (uint32_t)settings.channel[msg[1]].led_count;

        if((len+off)>settings.channel[msg[1]].led_count) {
          len -= (off+len)-settings.channel[msg[1]].led_count;
          printf("  NEW len %d (%d-%d) (max%d)\n", len, off, off+len, max);
        }

        for(int i = 0; i<len; i++) {
          if(msg[1] == 0) {
            pixel0[i].color.g = msg[i+5-msg[2]];
          } else {
            pixel1[i].color.g = msg[i+5-msg[2]];
          }
        }
      }

      if(msg[4] == 2) {
        int32_t len = (uint32_t)msg[3];
        int32_t off = (uint32_t)msg[2];
        int32_t max = (uint32_t)settings.channel[msg[1]].led_count;

        if((len+off)>settings.channel[msg[1]].led_count) {
          len -= (off+len)-settings.channel[msg[1]].led_count;
          printf("  NEW len %d (%d-%d) (max%d)\n", len, off, off+len, max);
        }

        for(int i = 0; i<len; i++) {
          if(msg[1] == 0) {
            pixel0[i].color.b = msg[i+5-msg[2]];
          } else {
            pixel1[i].color.b = msg[i+5-msg[2]];
          }
        }
      }

      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    default: {
      printf("Other event_idx %.02x\n", msg[0]);
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }
  }
  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
static int8_t CUSTOM_HID_SendOk(uint8_t *buffer, size_t size){
  uint64_t *t;
  t = (uint64_t*)&report;
  t[0] = 0;
  t[1] = 0;

  report[0] = PROTOCOL_RESPONSE_OK;
  for(uint8_t i = 0; i<size; i++) {
    report[i+1] = buffer[i];
    //printf("...%.2x\n", buffer[i]);
  }

  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, 16);

}

static int8_t CUSTOM_HID_SendError(void){
  uint64_t *t;
  t = (uint64_t*)&report;
  t[0] = 0;
  t[1] = 0;

  report[0] = PROTOCOL_RESPONSE_ERROR;

  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, 16);

}
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

