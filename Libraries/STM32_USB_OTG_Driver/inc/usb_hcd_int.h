/**
  ******************************************************************************
  * @file    usb_hcd_int.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   Peripheral Device Interface Layer
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

#ifndef __HCD_INT_H__
#define __HCD_INT_H__

#include "usb_hcd.h"

#define CLEAR_HC_INT(HC_REGS, intr) \
  {\
  USB_OTG_HCINTn_TypeDef  hcint_clear; \
  hcint_clear.d32 = 0; \
  hcint_clear.b.intr = 1; \
  USB_OTG_WRITE_REG32(&((HC_REGS)->HCINT), hcint_clear.d32);\
  }\

#define MASK_HOST_INT_CHH(hc_num) { USB_OTG_HCGINTMSK_TypeDef  GINTMSK; \
    GINTMSK.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK); \
    GINTMSK.b.chhltd = 0; \
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK, GINTMSK.d32);}

#define UNMASK_HOST_INT_CHH(hc_num) { USB_OTG_HCGINTMSK_TypeDef  GINTMSK; \
    GINTMSK.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK); \
    GINTMSK.b.chhltd = 1; \
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK, GINTMSK.d32);}

#define MASK_HOST_INT_ACK(hc_num) { USB_OTG_HCGINTMSK_TypeDef  GINTMSK; \
    GINTMSK.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK); \
    GINTMSK.b.ack = 0; \
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK, GINTMSK.d32);}

#define UNMASK_HOST_INT_ACK(hc_num) { USB_OTG_HCGINTMSK_TypeDef  GINTMSK; \
    GINTMSK.d32 = USB_OTG_READ_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK); \
    GINTMSK.b.ack = 1; \
    USB_OTG_WRITE_REG32(&pdev->regs.HC_REGS[hc_num]->HCGINTMSK, GINTMSK.d32);}

void ConnectCallback_Handler(USB_OTG_CORE_HANDLE *pdev);
void Disconnect_Callback_Handler(USB_OTG_CORE_HANDLE *pdev);
void Overcurrent_Callback_Handler(USB_OTG_CORE_HANDLE *pdev);
uint32_t USBH_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

