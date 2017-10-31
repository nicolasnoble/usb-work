/**
  ******************************************************************************
  * @file    usbd_hid_core.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   header file for the usbd_hid_core.c file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#pragma once

#include  "usbd_ioreq.h"

#define USB_HID_CONFIG_DESC_SIZ       34
#define USB_HID_DESC_SIZ              9
#define HID_MOUSE_REPORT_DESC_SIZE    74

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22


#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01

//extern USBD_Class_cb_TypeDef  USBD_HID_cb;

uint8_t USBD_HID_Init(void *pdev, uint8_t cfgidx) { return 0; };
uint8_t USBD_HID_DeInit(void *pdev, uint8_t cfgidx) { return 0; };
uint8_t USBD_HID_Setup(void  *pdev, USB_SETUP_REQ *req); //this one is still here
uint8_t USBD_HID_EP0_TxSent(void *pdev) { return 0; };
uint8_t USBD_HID_EP0_RxReady(void *pdev) { return 0; };
uint8_t USBD_HID_DataIn(void *pdev, uint8_t epnum) { return 0; };
uint8_t USBD_HID_DataOut(void *pdev, uint8_t epnum) { return 0; };
uint8_t USBD_HID_OF(void *pdev) { return 0; };
uint8_t USBD_HID_IsoINIncomplete(void *pdev) { return 0; };
uint8_t USBD_HID_IsoOUTIncomplete(void *pdev) { return 0; };
uint8_t USBD_HID_GetConfigDescriptor(uint8_t speed, uint16_t *length) { return 0; };
uint8_t USBD_HID_GetOtherConfigDescriptor(uint8_t speed, uint16_t *length) { return 0; };
uint8_t USBD_HID_GetUsrStrDescriptor(uint8_t speed, uint8_t index, uint16_t *length) { return 0; };



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
