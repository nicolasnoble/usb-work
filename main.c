#include "main.h"
#include "usbd_hid_core.h"
#include "usbd_desc.h"

#include <FreeRTOS.h>
#include <task.h>

#include <gpio.h>

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;




//#define RTOS_DEBUG

void sendData()
{
  uint8_t buf[] = {0, 1, 0, 0};
  while(1)
  {
    USBD_HID_SendReport (&USB_OTG_dev, buf, 4);
    //vTaskDelay(100);
  }
}

void nothing(void){}
void nothing2(uint8_t foo){}

USBD_Usr_cb_TypeDef usrcb =
{
  nothing,
  nothing2,
  nothing,
  nothing,
  nothing,
  nothing,
  nothing,
};

int main(void)
{
  printf("test\n");
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
  USB_OTG_dev.dev.class_cb = &USBD_HID_cb;
  USB_OTG_dev.dev.usr_cb = &usrcb; //nothing
  USB_OTG_dev.dev.usr_device = &USR_desc;

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

/*
(gdb) continue
Continuing.
^C
Program received signal SIGINT, Interrupt.
0x08003b82 in general_C_handler (fault=<optimized out>, fault_data_extra=<optimized out>, lr=<optimized out>) at arm/Core/CM4F/handlers.c:73
73      BoardExceptionHandler(-1);
(gdb) bt
#0  0x08003b82 in general_C_handler (fault=<optimized out>, fault_data_extra=<optimized out>, lr=<optimized out>) at arm/Core/CM4F/handlers.c:73
#1  0x08000cba in general_handler () at arm/Core/CM4F/startup.s:215
#2  0x08000cba in general_handler () at arm/Core/CM4F/startup.s:215
...
*/
#ifdef RTOS_DEBUG
    xTaskCreate(sendData, (const signed char *)NULL, configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();
#else
    sendData();
#endif
    return 1;
}

