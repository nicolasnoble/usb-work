#include "main.h"
#include "usbd_hid_core.h"

#include <FreeRTOS.h>
#include <task.h>

#include <gpio.h>

#include <stm32f4xx.h> //For RCC

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#define RTOS_DEBUG

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

void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
  /* Reset SLEEPDEEP and SLEEPONEXIT bits */
  SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

  /* After wake-up from sleep mode, reconfigure the system clock */
  SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void sendData()
{
  uint8_t buf[] = {0, 1, 0, 0};
  while(1)
  {
    USBD_HID_SendReport (&USB_OTG_dev, buf, 4);
    //vTaskDelay(100);
  }
}

extern uint32_t SystemCoreClock;

/*
HSE_VALUE=8000000
PLL_M=8
USE_HSE_BYPASS

PLL_Q      7
PLL_N      336
PLL_P      2

 PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
336000000

 USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
48000000
 SYSCLK = PLL_VCO / PLL_P
168000000
*/

int main(void)
{
  printf("CPU family: PORT_CPU_FAMILY\n");
  printf("CPU subfamily: PORT_CPU_FAMILY_SUBFAMILY\n");
  printf("CPU flavor: PORT_CPU_FLAVOR\n");
  printf("CPU: PORT_CPU\n");
  printf("------------------------------\n");
  printf("HSE_VALUE=%d\n", HSE_VALUE);
  printf("HSI_VALUE=%d\n", HSI_VALUE);
  printf("------------------------------\n");
  printf("SystemCoreClock=%d\n", SystemCoreClock);
  printf("RCC->CR %0x\n", RCC->CR);
  printf("RCC->CFGR %0x\n", RCC->CFGR);
  printf("RCC->PLLCFGR %0x\n", RCC->PLLCFGR);

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
  USB_OTG_dev.dev.usr_cb = NULL;
  USB_OTG_dev.dev.usr_device = NULL;

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


#ifdef RTOS_DEBUG
    xTaskCreate(sendData, (const signed char *)NULL, configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();
#else
    sendData();
#endif
    return 1;
}

