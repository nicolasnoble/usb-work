/**
  ******************************************************************************
  * @file    usbd_req.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2011
  * @brief   This file provides the standard USB requests following chapter 9.
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

#include "usbd_req.h"
#include "usbd_ioreq.h"
#include "usbd_desc.h"

#include <string.h>


__ALIGN_BEGIN uint32_t USBD_ep_status __ALIGN_END  = 0;
__ALIGN_BEGIN uint32_t  USBD_default_cfg __ALIGN_END  = 0;
__ALIGN_BEGIN uint32_t  USBD_cfg_status __ALIGN_END  = 0;
__ALIGN_BEGIN uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ] __ALIGN_END ;


static void USBD_GetDescriptor(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_SetAddress(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_SetConfig(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_GetConfig(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_GetStatus(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_SetFeature(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);
static void USBD_ClrFeature(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req);

/*
// temporary for debugging
void usb_hid_device_init();
void usb_hid_device_deinit();
void usb_hid_device_setup();
*/

/**
* @brief  USBD_StdDevReq
*         Handle standard usb device requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
USBD_Status USBD_StdDevReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{
  USBD_Status ret = USBD_OK;

  switch (req->bRequest)
  {
    case USB_REQ_GET_DESCRIPTOR: //0x06

      USBD_GetDescriptor (pdev, req) ;
      break;

    case USB_REQ_SET_ADDRESS: // 0x05
      USBD_SetAddress(pdev, req);
      break;

    case USB_REQ_SET_CONFIGURATION: //0x09
      USBD_SetConfig (pdev , req);
      break;

    case USB_REQ_GET_CONFIGURATION: //0x08
      USBD_GetConfig (pdev , req);
      break;

    case USB_REQ_GET_STATUS: //0x00
      USBD_GetStatus (pdev , req);
      break;


    case USB_REQ_SET_FEATURE: //0x03
      USBD_SetFeature (pdev , req);
      break;

    case USB_REQ_CLEAR_FEATURE: //0x01
      USBD_ClrFeature (pdev , req);
      break;

    default:
      USBD_CtlError(pdev , req);
      break;
  }

  return ret;
}

/**
* @brief  USBD_StdItfReq
*         Handle standard usb interface requests
* @param  pdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USBD_Status  USBD_StdItfReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{
  USBD_Status ret = USBD_OK;

  switch (pdev->dev.device_status)
  {
    case USB_OTG_CONFIGURED:

      if (LOBYTE(req->wIndex) <= USBD_ITF_MAX_NUM)
      {
        //CLASS Setup
        USBD_Class_Setup(pdev, req);

        if((req->wLength == 0)&& (ret == USBD_OK))
        {
           USBD_CtlSendStatus(pdev);
        }
      }
      else
      {
         USBD_CtlError(pdev , req);
      }
      break;

    default:
       USBD_CtlError(pdev , req);
      break;
  }
  return ret;
}

/**
* @brief  USBD_StdEPReq
*         Handle standard usb endpoint requests
* @param  pdev: USB OTG device instance
* @param  req: usb request
* @retval status
*/
USBD_Status  USBD_StdEPReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req)
{

  uint8_t   ep_addr;
  USBD_Status ret = USBD_OK;

  ep_addr  = LOBYTE(req->wIndex);

  switch (req->bRequest)
  {
    case USB_REQ_SET_FEATURE :
      switch (pdev->dev.device_status)
      {
        case USB_OTG_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
          {
            DCD_EP_Stall(pdev , ep_addr);
          }
          break;

        case USB_OTG_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT)
          {
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
            {
              DCD_EP_Stall(pdev , ep_addr);

            }
          }
          //CLASS setup
          USBD_Class_Setup(pdev, req);
          USBD_CtlSendStatus(pdev);
          break;

        default:
          USBD_CtlError(pdev , req);
          break;
      }
      break;

    case USB_REQ_CLEAR_FEATURE :
      switch (pdev->dev.device_status)
      {
        case USB_OTG_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
          {
            DCD_EP_Stall(pdev , ep_addr);
          }
          break;

        case USB_OTG_CONFIGURED:
          if (req->wValue == USB_FEATURE_EP_HALT)
          {
            if ((ep_addr != 0x00) && (ep_addr != 0x80))
            {
              DCD_EP_ClrStall(pdev , ep_addr);
              //CLASS setup
              USBD_Class_Setup(pdev, req);
            }
            USBD_CtlSendStatus(pdev);
          }
          break;

        default:
           USBD_CtlError(pdev , req);
          break;
      }
      break;

    case USB_REQ_GET_STATUS:
      switch (pdev->dev.device_status)
      {
        case USB_OTG_ADDRESSED:
          if ((ep_addr != 0x00) && (ep_addr != 0x80))
          {
            DCD_EP_Stall(pdev , ep_addr);
          }
          break;

        case USB_OTG_CONFIGURED:
          if ((ep_addr & 0x80)== 0x80)
          {
            if(pdev->dev.in_ep[ep_addr & 0x7F].is_stall)
            {
              USBD_ep_status = 0x0001;
            }
            else
            {
              USBD_ep_status = 0x0000;
            }
          }
          else if ((ep_addr & 0x80)== 0x00)
          {
            if(pdev->dev.out_ep[ep_addr].is_stall)
            {
              USBD_ep_status = 0x0001;
            }
            else
            {
              USBD_ep_status = 0x0000;
            }
          }
          USBD_CtlSendData (pdev, (uint8_t *)&USBD_ep_status, 2);
          break;

        default:
           USBD_CtlError(pdev , req);
          break;
      }
      break;

    default:
      break;
  }
  return ret;
}

const uint8_t * get_USB_interface_descriptor(int configuration, int interface);
const uint8_t * get_USB_configuration_descriptor(int index);
const uint8_t * get_USB_device_descriptor();
const uint8_t * get_USB_string_descriptor(int index);

/**
* @brief  USBD_GetDescriptor
*         Handle Get Descriptor requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetDescriptor(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  uint16_t len = 0;
   uint8_t *pbuf = NULL;

  switch (req->wValue >> 8)
  {
    case USB_DESC_TYPE_DEVICE: //0x01
      pbuf = (uint8_t *)get_USB_device_descriptor();
      len = pbuf[0];
      break;

    case USB_DESC_TYPE_CONFIGURATION: //0x02
      pbuf = (uint8_t *)get_USB_configuration_descriptor(1);
      len = (((uint16_t) pbuf[3]) << 8) + pbuf[2]; //totallength
      pdev->dev.pConfig_descriptor = pbuf;
      break;

    case USB_DESC_TYPE_STRING: //0x03
        pbuf = (uint8_t *)get_USB_string_descriptor((uint8_t)(req->wValue));
        len = pbuf[0];
      break;
    case USB_DESC_TYPE_DEVICE_QUALIFIER: //0x06 //TODO
      //we use FS currently
        USBD_CtlError(pdev , req);
        return;

    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION: //TODO
      //we use FS currently
        USBD_CtlError(pdev , req);
        return;

    default:
       USBD_CtlError(pdev , req);
      return;
  }

  if((len != 0)&& (req->wLength != 0))
  {
    len = MIN(len , req->wLength);
    USBD_CtlSendData (pdev, pbuf, len);
  }

}

/**
* @brief  USBD_SetAddress
*         Set device address
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetAddress(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  uint8_t  dev_addr;

  if ((req->wIndex != 0) || (req->wLength != 0))
  {
     USBD_CtlError(pdev , req);
     return;
  }

  dev_addr = (uint8_t)(req->wValue) & 0x7F;

  if (pdev->dev.device_status == USB_OTG_CONFIGURED)
  {
    USBD_CtlError(pdev , req);
    return;
  }
  pdev->dev.device_address = dev_addr;
  DCD_EP_SetAddress(pdev, dev_addr);
  USBD_CtlSendStatus(pdev);

  if (dev_addr != 0)
  {
    pdev->dev.device_status  = USB_OTG_ADDRESSED;
  }
  else
  {
    pdev->dev.device_status  = USB_OTG_DEFAULT;
  }
}

/**
* @brief  USBD_SetConfig
*         Handle Set device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetConfig(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  static uint8_t  cfgidx;

  cfgidx = (uint8_t)(req->wValue);

  if (cfgidx > USBD_CFG_MAX_NUM )
  {
     USBD_CtlError(pdev , req);
     return;
  }

  switch (pdev->dev.device_status)
  {
    case USB_OTG_ADDRESSED:
      if (cfgidx)
      {
        pdev->dev.device_config = cfgidx;
        pdev->dev.device_status = USB_OTG_CONFIGURED;
        // begin CLASS Init
        USBD_Class_Init(pdev, cfgidx);
        // end CLASS Init
        USBD_CtlSendStatus(pdev);
      }
      break;

    case USB_OTG_CONFIGURED:
      if (cfgidx == 0)
      {
        pdev->dev.device_status = USB_OTG_ADDRESSED;
        pdev->dev.device_config = cfgidx;
        // begin CLASS Deinit
        USBD_Class_DeInit(pdev, cfgidx);
        // end CLASS Deinit

      }
      else if (cfgidx != pdev->dev.device_config)
      {
        /* Clear old configuration */
        // begin CLASS Deinit
        USBD_Class_DeInit(pdev, cfgidx);
        // end CLASS Deinit

        /* set new configuration */
        pdev->dev.device_config = cfgidx;
        // begin CLASS Init
        USBD_Class_Init(pdev, cfgidx);
        // end CLASS Init
      }
      break;

    default:
       USBD_CtlError(pdev , req);
       return;
      break;
  }
  USBD_CtlSendStatus(pdev);
}

/**
* @brief  USBD_GetConfig
*         Handle Get device configuration request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetConfig(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  if (req->wLength != 1)
  {
     USBD_CtlError(pdev , req);
  }
  else
  {
    switch (pdev->dev.device_status )
    {
      case USB_OTG_ADDRESSED:
        USBD_CtlSendData (pdev, (uint8_t *)&USBD_default_cfg, 1);
        break;

      case USB_OTG_CONFIGURED:
        USBD_CtlSendData (pdev, &pdev->dev.device_config, 1);
        break;

      default:
         USBD_CtlError(pdev , req);
        break;
    }
  }
}

/**
* @brief  USBD_GetStatus
*         Handle Get Status request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_GetStatus(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{

  switch (pdev->dev.device_status)
  {
    case USB_OTG_ADDRESSED:
    case USB_OTG_CONFIGURED:
      if (pdev->dev.DevRemoteWakeup)
      {
        USBD_cfg_status = USB_CONFIG_SELF_POWERED | USB_CONFIG_REMOTE_WAKEUP;
      }
      else
      {
        USBD_cfg_status = USB_CONFIG_SELF_POWERED;
      }

      USBD_CtlSendData (pdev, (uint8_t *)&USBD_cfg_status, 1);
      break;

    default :
      USBD_CtlError(pdev , req);
      break;
  }
}


/**
* @brief  USBD_SetFeature
*         Handle Set device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_SetFeature(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  USB_OTG_DCTL_TypeDef     dctl;
  uint8_t test_mode = 0;

  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    pdev->dev.DevRemoteWakeup = 1;
    //CLASS setup
    USBD_Class_Setup(pdev, req);
    USBD_CtlSendStatus(pdev);
  }

  else if ((req->wValue == USB_FEATURE_TEST_MODE) &&
           ((req->wIndex & 0xFF) == 0))
  {
    dctl.d32 = USB_OTG_READ_REG32(&pdev->regs.DREGS->DCTL);

    test_mode = req->wIndex >> 8;
    switch (test_mode)
    {
      case 1: // TEST_J
        dctl.b.tstctl = 1;
        break;

      case 2: // TEST_K
        dctl.b.tstctl = 2;
        break;

      case 3: // TEST_SE0_NAK
        dctl.b.tstctl = 3;
        break;

      case 4: // TEST_PACKET
        dctl.b.tstctl = 4;
        break;

      case 5: // TEST_FORCE_ENABLE
        dctl.b.tstctl = 5;
        break;
    }
    USB_OTG_WRITE_REG32(&pdev->regs.DREGS->DCTL, dctl.d32);
    USBD_CtlSendStatus(pdev);
  }

}


/**
* @brief  USBD_ClrFeature
*         Handle clear device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USBD_ClrFeature(USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  switch (pdev->dev.device_status)
  {
    case USB_OTG_ADDRESSED:
    case USB_OTG_CONFIGURED:
      if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
      {
        pdev->dev.DevRemoteWakeup = 0;
        //CLASS setup
        USBD_Class_Setup(pdev, req);
        USBD_CtlSendStatus(pdev);
      }
      break;

    default :
       USBD_CtlError(pdev , req);
      break;
  }
}

/**
* @brief  USBD_ParseSetupRequest
*         Copy buffer into setup structure
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USBD_ParseSetupRequest( USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  req->bmRequest     = *(uint8_t *)  (pdev->dev.setup_packet);
  req->bRequest      = *(uint8_t *)  (pdev->dev.setup_packet +  1);
  req->wValue        = SWAPBYTE      (pdev->dev.setup_packet +  2);
  req->wIndex        = SWAPBYTE      (pdev->dev.setup_packet +  4);
  req->wLength       = SWAPBYTE      (pdev->dev.setup_packet +  6);

  pdev->dev.in_ep[0].ctl_data_len = req->wLength  ;
  pdev->dev.device_state = USB_OTG_EP0_SETUP;
}

/**
* @brief  USBD_CtlError
*         Handle USB low level Error
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USBD_CtlError( USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ *req)
{
  if((req->bmRequest & 0x80) == 0x80)
  {
    DCD_EP_Stall(pdev , 0x80);
  }
  else
  {
    if(req->wLength == 0)
    {
       DCD_EP_Stall(pdev , 0x80);
    }
    else
    {
      DCD_EP_Stall(pdev , 0);
    }
  }
  USB_OTG_EP0_OutStart(pdev);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
