/**
  ******************************************************************************
  * @file    usbd_core.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   Header file for usbd_core.c
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

#pragma once

#include "usb_dcd.h"
#include "usbd_def.h"
#include "usbd_conf.h"

typedef enum {
  USBD_OK   = 0,
  USBD_BUSY,
  USBD_FAIL,
} USBD_Status;

void USBD_DataOutStage(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
void USBD_DataInStage(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
void USBD_SetupStage(USB_OTG_CORE_HANDLE *pdev);
void USBD_SOF(USB_OTG_CORE_HANDLE *pdev);
void USBD_Reset(USB_OTG_CORE_HANDLE *pdev);
void USBD_Suspend(USB_OTG_CORE_HANDLE *pdev);
void USBD_Resume(USB_OTG_CORE_HANDLE *pdev);
void USBD_IsoINIncomplete(USB_OTG_CORE_HANDLE *pdev);
void USBD_IsoOUTIncomplete(USB_OTG_CORE_HANDLE *pdev);
void USBD_DevConnected(USB_OTG_CORE_HANDLE *pdev);
void USBD_DevDisconnected(USB_OTG_CORE_HANDLE *pdev);


//useful at this stage
//for debugging a HID device
#define USBD_Class_Init(X,Y) usb_hid_device_init()//USBD_HID_Init
#define USBD_Class_DeInit(X,Y) usb_hid_device_deinit()//USBD_HID_DeInit
#define USBD_Class_Setup USBD_HID_Setup
#define USBD_Class_EP0_TxSent USBD_HID_EP0_TxSent
#define USBD_Class_EP0_RxReady USBD_HID_EP0_RxReady
#define USBD_Class_DataIn USBD_HID_DataIn
#define USBD_Class_DataOut USBD_HID_DataOut
#define USBD_Class_OF USBD_HID_OF
#define USBD_Class_IsoINIncomplete USBD_HID_IsoINIncomplete
#define USBD_Class_IsoOUTIncomplete USBD_HID_IsoOUTIncomplete
#define USBD_Class_GetConfigDescriptor USBD_HID_GetConfigDescriptor
#define USBD_Class_GetOtherConfigDescriptor USBD_HID_GetOtherConfigDescriptor
#define USBD_Class_GetUsrStrDescriptor USBD_HID_GetUsrStrDescriptor

void USBD_Class_Init(void *pdev , uint8_t cfgidx);
void USBD_Class_DeInit(void *pdev, uint8_t cfgidx);
void USBD_Class_Setup(void *pdev, USB_SETUP_REQ *req);
void USBD_Class_EP0_TxSent(void *pdev);
void USBD_Class_EP0_RxReady(void *pdev);
void USBD_Class_DataIn(void *pdev, uint8_t epnum);
void USBD_Class_DataOut(void *pdev, uint8_t epnum);
void USBD_Class_OF(void *pdev);
void USBD_Class_IsoINIncomplete(void *pdev);
void USBD_Class_IsoOUTIncomplete(void *pdev);
void USBD_Class_GetConfigDescriptor(uint8_t speed, uint16_t *length);
void USBD_Class_GetOtherConfigDescriptor(uint8_t speed, uint16_t *length);
void USBD_Class_GetUsrStrDescriptor(uint8_t speed, uint8_t index, uint16_t *length);

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



