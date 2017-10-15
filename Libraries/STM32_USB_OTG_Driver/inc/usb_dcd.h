/**
  ******************************************************************************
  * @file    usb_dcd.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   Peripheral Driver Header file
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

#ifndef __DCD_H__
#define __DCD_H__

#include "usb_core.h"

#define USB_OTG_EP_CONTROL                       0
#define USB_OTG_EP_ISOC                          1
#define USB_OTG_EP_BULK                          2
#define USB_OTG_EP_INT                           3
#define USB_OTG_EP_MASK                          3

#define USB_OTG_DEFAULT                          1
#define USB_OTG_ADDRESSED                        2
#define USB_OTG_CONFIGURED                       3
#define USB_OTG_SUSPENDED                        4

typedef struct
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
}
EP_DESCRIPTOR , *PEP_DESCRIPTOR;

void DCD_Init(USB_OTG_CORE_HANDLE *pdev, USB_OTG_CORE_ID_TypeDef coreID);

void DCD_DevConnect (USB_OTG_CORE_HANDLE *pdev);
void DCD_DevDisconnect (USB_OTG_CORE_HANDLE *pdev);
void DCD_EP_SetAddress (USB_OTG_CORE_HANDLE *pdev, uint8_t address);
uint32_t DCD_EP_Open(USB_OTG_CORE_HANDLE *pdev, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);

uint32_t DCD_EP_Close  (USB_OTG_CORE_HANDLE *pdev, uint8_t ep_addr);


uint32_t DCD_EP_PrepareRx ( USB_OTG_CORE_HANDLE *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t buf_len);

uint32_t DCD_EP_Tx (USB_OTG_CORE_HANDLE *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t buf_len);
uint32_t DCD_EP_Stall (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t DCD_EP_ClrStall (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t DCD_EP_Flush (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);
uint32_t DCD_Handle_ISR(USB_OTG_CORE_HANDLE *pdev);

uint32_t DCD_GetEPStatus(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum);

void DCD_SetEPStatus (USB_OTG_CORE_HANDLE *pdev, uint8_t epnum, uint32_t Status);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

