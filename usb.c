#include "usb.h"

#include <stm32f4xx.h>

//#include "usbd_hid_core.h"
#include  "usbd_ioreq.h"

#include <gpio.h>
#include <irq.h>
#include <hardware.h>

//#define USEWAKEUP

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//extern USBD_Class_cb_TypeDef  USBD_HID_cb;

void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  uint32_t count = 0;
  const uint32_t utime = (120 * usec / 7);
  do
  {
    if ( ++count > utime )
    {
      return ;
    }
  }
  while (1);
}

#ifdef USEWAKUP
static void usbwakeuphandler()
{
  if (USB_OTG_dev.cfg.low_power)
  {
    /* Reset SLEEPDEEP and SLEEPONEXIT bits */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

    /* After wake-up from sleep mode, reconfigure the system clock */
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

static void usbhandler()
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void usb_fs_device_init()
{
  pin_t sof = make_pin(gpio_port_a, 8);
  pin_t vbus = make_pin(gpio_port_a, 9);
  pin_t id = make_pin(gpio_port_a, 10);
  pin_t dm = make_pin(gpio_port_a, 11);
  pin_t dp = make_pin(gpio_port_a, 12);
  gpio_config_alternate(sof, pin_dir_write, pull_none, 10);
  gpio_config_alternate(vbus, pin_dir_write, pull_none, 10);
  gpio_config_alternate(id, pin_dir_read, pull_up, 10);
  gpio_config_alternate(dm, pin_dir_write, pull_none, 10);
  gpio_config_alternate(dp, pin_dir_write, pull_none, 10);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;

  //set callbacks
  USB_OTG_dev.dev.class_cb = NULL;//&USBD_HID_cb;
  USB_OTG_dev.dev.usr_cb = NULL;
  USB_OTG_dev.dev.usr_device = NULL;

  set_irq_handler(OTG_FS_IRQ_handler, &usbhandler);
  #ifdef USEWAKUP
  set_irq_handler(OTG_FS_WKUP_IRQ_handler, &usbwakeuphandler);
  #endif

  //configure endpoints
  DCD_Init(&USB_OTG_dev , USB_OTG_FS_CORE_ID);

  //enable interrupts
  NVIC_InitTypeDef nvic;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //FreeRTOS crashes if NVIC_PriorityGroup_1
  nvic.NVIC_IRQChannel = OTG_FS_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 1;
  nvic.NVIC_IRQChannelSubPriority = 3;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);

}

void usb_send_report(uint8_t *buffer, uint16_t nb)
{
  if (USB_OTG_dev.dev.device_status == USB_OTG_CONFIGURED )
    DCD_EP_Tx (&USB_OTG_dev, HID_IN_EP, buffer, nb);
}

