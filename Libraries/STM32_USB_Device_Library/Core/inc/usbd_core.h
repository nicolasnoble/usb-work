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

#ifndef __USBD_CORE_H
#define __USBD_CORE_H

#include "usb_dcd.h"
#include "usbd_def.h"
#include "usbd_conf.h"

typedef enum {
  USBD_OK   = 0,
  USBD_BUSY,
  USBD_FAIL,
} USBD_Status;

//useful at this stage
//for debugging a HID device
#define USBD_Class_Init USBD_HID_Init
#define USBD_Class_DeInit USBD_HID_DeInit
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

uint8_t USBD_Class_Init(void *pdev , uint8_t cfgidx);
uint8_t USBD_Class_DeInit(void *pdev , uint8_t cfgidx);
uint8_t USBD_Class_Setup(void  *pdev, USB_SETUP_REQ *req);
uint8_t USBD_Class_EP0_TxSent(void *pdev );
uint8_t USBD_Class_EP0_RxReady(void *pdev );
uint8_t USBD_Class_DataIn(void *pdev , uint8_t epnum);
uint8_t USBD_Class_DataOut(void *pdev , uint8_t epnum);
uint8_t USBD_Class_OF(void *pdev);
uint8_t USBD_Class_IsoINIncomplete(void *pdev);
uint8_t USBD_Class_IsoOUTIncomplete(void *pdev);
uint8_t USBD_Class_GetConfigDescriptor( uint8_t speed , uint16_t *length);
uint8_t USBD_Class_GetOtherConfigDescriptor( uint8_t speed , uint16_t *length);
uint8_t USBD_Class_GetUsrStrDescriptor( uint8_t speed ,uint8_t index,  uint16_t *length);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



