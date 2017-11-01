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

void USBD_HID_Init(void *pdev, uint8_t cfgidx);       //this one is still here
void USBD_HID_DeInit(void *pdev, uint8_t cfgidx);     //this one is still here
void USBD_HID_Setup(void *pdev, USB_SETUP_REQ *req);  //this one is still here
void USBD_HID_EP0_TxSent(void *pdev) {};
void USBD_HID_EP0_RxReady(void *pdev) {};
void USBD_HID_DataIn(void *pdev, uint8_t epnum) {};
void USBD_HID_DataOut(void *pdev, uint8_t epnum) {};
void USBD_HID_OF(void *pdev) {};
void USBD_HID_IsoINIncomplete(void *pdev) {};
void USBD_HID_IsoOUTIncomplete(void *pdev) {};
void USBD_HID_GetConfigDescriptor(uint8_t speed, uint16_t *length) {};
void USBD_HID_GetOtherConfigDescriptor(uint8_t speed, uint16_t *length) {};
void USBD_HID_GetUsrStrDescriptor(uint8_t speed, uint8_t index, uint16_t *length) {};

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
