//#include "main.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

#include <stm32f4xx.h> //For RCC

#include "usb.h"

#define RTOS_DEBUG

void sendData()
{
  uint8_t buf[] = {0, 1, 0, 0};
  while(1)
  {
    usb_send_report(buf, 4);
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

  usb_fs_device_init();

#ifdef RTOS_DEBUG
    xTaskCreate(sendData, (const signed char *)NULL, configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();
#else
    sendData();
#endif
    return 1;
}

