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
#include <stdint.h>
#include <stdbool.h>
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__ALIGN_BEGIN uint8_t report[16] __ALIGN_END;
extern uint8_t device_serial_32[4];

bool led_trigger;
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
  //msg = corsairLightingNodeMessage;

  switch(*corsairLightingNodeMessage) {
    case READ_FIRMWARE_VERSION: {

      //printf("READ_FIRMWARE_VERSION\n");
      uint8_t corsairLightingNodeReply[16] = {0x00, 0x0A, 0x04};
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 3);
      break;
    }

    case READ_DEVICE_ID: {
      //printf("READ_DEVICE_ID\n");
      CUSTOM_HID_SendOk(device_serial_32, 4);
      break;
    }

    case READ_BOOTLOADER_VERSION: {
      //printf("READ_BOOTLOADER_VERSION\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    /*
      Do NOT response too fast
    */
    case WRITE_LED_TRIGGER: {
      //HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_SET);
      //printf("WRITE_LED_TRIGGER\n");
      if(led_trigger){
        //printf("    error\n");
        CUSTOM_HID_SendError();
        break;
      } else {
        led_trigger = true;
      }
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      //HAL_GPIO_WritePin(ACTIVITY_GPIO_Port, ACTIVITY_Pin, GPIO_PIN_RESET);
      break;
    }

    case WRITE_LED_MODE: {
      //printf("WRITE_LED_MODE Channel(%d) Mode(%.02x)\n", msg[1], msg[2]);
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_CLEAR: {
      //printf("WRITE_LED_CLEAR\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_GROUP_SET: {
      //printf("WRITE_LED_GROUP_SET\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_GROUP_CLEAR: {
      //printf("WRITE_LED_GROUP_CLEAR\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_BRIGHTNESS: {
      //printf("WRITE_LED_BRIGHTNESS\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_PORT_TYPE: {
      //printf("WRITE_LED_PORT_TYPE\n");
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    case WRITE_LED_COLOR_VALUES: {
      //printf("WRITE_LED_COLOR_VALUES\n");
      //for(volatile uint32_t i = 0; i<0x8ffff; i++){};
      uint8_t corsairLightingNodeReply[16];
      CUSTOM_HID_SendOk(corsairLightingNodeReply, 0);
      break;
    }

    default: {
      //printf("Other event_idx %.02x\n", msg[0]);
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

