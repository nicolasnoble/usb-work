/**
  ******************************************************************************
  * @file    usbd_core.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file provides all the USBD core functions.
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

#include "usbd_core.h"
#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usb_dcd_int.h"
#include "usb_bsp.h"

/*void USBD_SOF(USB_OTG_CORE_HANDLE *pdev) { return 0; }
void USBD_Reset(USB_OTG_CORE_HANDLE *pdev) { return 0; }
void USBD_Suspend(USB_OTG_CORE_HANDLE *pdev) { return 0; }
void USBD_Resume(USB_OTG_CORE_HANDLE *pdev) { return 0; }
void USBD_IsoINIncomplete(USB_OTG_CORE_HANDLE *pdev) { return 0; }*/
void USBD_IsoOUTIncomplete(USB_OTG_CORE_HANDLE *pdev) {}
/*void USBD_DevConnected(USB_OTG_CORE_HANDLE *pdev) { return 0; }
void USBD_DevDisconnected(USB_OTG_CORE_HANDLE *pdev) { return 0; }*/


void USBD_SetupStage(USB_OTG_CORE_HANDLE *pdev)
{
  USB_SETUP_REQ req;

  USBD_ParseSetupRequest(pdev, &req);

  switch (req.bmRequest & 0x1F)
  {
    case USB_REQ_RECIPIENT_DEVICE:
      USBD_StdDevReq (pdev, &req);
      break;

    case USB_REQ_RECIPIENT_INTERFACE:
      USBD_StdItfReq(pdev, &req);
      break;

    case USB_REQ_RECIPIENT_ENDPOINT:
      USBD_StdEPReq(pdev, &req);
      break;

    default:
      DCD_EP_Stall(pdev, req.bmRequest & 0x80);
      break;
  }
}


void USBD_DataOutStage(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum)
{
  USB_OTG_EP *ep;

  if (epnum == 0)
  {
    ep = &pdev->dev.out_ep[0];
    if ( pdev->dev.device_state == USB_OTG_EP0_DATA_OUT)
    {
      if (ep->rem_data_len > ep->maxpacket)
      {
        ep->rem_data_len -=  ep->maxpacket;

        if (pdev->cfg.dma_enable == 1)
          /* in slave mode this, is handled by the RxSTSQLvl ISR */
          ep->xfer_buff += ep->maxpacket;
        USBD_CtlContinueRx (pdev, ep->xfer_buff, MIN(ep->rem_data_len,ep->maxpacket));
      }
      else
      {
        if (pdev->dev.device_status == USB_OTG_CONFIGURED)
          USBD_Class_EP0_RxReady(pdev);
        USBD_CtlSendStatus(pdev);
      }
    }
  }
  else if (pdev->dev.device_status == USB_OTG_CONFIGURED)
    USBD_Class_DataOut(pdev, epnum);
}


void USBD_DataInStage(USB_OTG_CORE_HANDLE *pdev, uint8_t epnum)
{
  USB_OTG_EP *ep;

  if (epnum == 0)
  {
    ep = &pdev->dev.in_ep[0];
    if ( pdev->dev.device_state == USB_OTG_EP0_DATA_IN)
    {
      if (ep->rem_data_len > ep->maxpacket)
      {
        ep->rem_data_len -=  ep->maxpacket;
        if (pdev->cfg.dma_enable == 1)
          /* in slave mode this, is handled by the TxFifoEmpty ISR */
          ep->xfer_buff += ep->maxpacket;
        USBD_CtlContinueSendData (pdev, ep->xfer_buff, ep->rem_data_len);
      }
      else
      { /* last packet is MPS multiple, so send ZLP packet */
        if ((ep->total_data_len % ep->maxpacket == 0) &&
           (ep->total_data_len >= ep->maxpacket) &&
             (ep->total_data_len < ep->ctl_data_len ))
        {

          USBD_CtlContinueSendData(pdev, NULL, 0);
          ep->ctl_data_len = 0;
        }
        else
        {
          if (pdev->dev.device_status == USB_OTG_CONFIGURED)
            USBD_Class_EP0_TxSent(pdev);
          USBD_CtlReceiveStatus(pdev);
        }
      }
    }
  }
  else if (pdev->dev.device_status == USB_OTG_CONFIGURED)
    DCD_EP_Flush(pdev, HID_IN_EP);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

