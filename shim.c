#include <stdint.h>
#include <stm32f4xx.h>

int usb_test_main();

#define MYRCC ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))

void system_reinit(void) {
  volatile uint32_t StartUpCounter = 0, HSEStatus = 0;

//  MYRCC->CR |= (uint32_t)0x00000001;
  //nécessaire
  MYRCC->CFGR = 0x00000000;
  //nécessaire
  MYRCC->CR &= (uint32_t)0xFEF6FFFF;
//  MYRCC->PLLCFGR = 0x24003010;
  //nécessaire
  MYRCC->CR &= (uint32_t)0xFFFBFFFF;
  //nécessaire
  MYRCC->CIR = 0x00000000;
  //nécessaire
  MYRCC->CR |= ((uint32_t)((uint32_t)0x00010000));

  do {
    HSEStatus = MYRCC->CR & ((uint32_t)0x00020000);
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != ((uint16_t)0x0500)));
  if ((MYRCC->CR & ((uint32_t)0x00020000)) != RESET) {
    HSEStatus = (uint32_t)0x01;

    MYRCC->APB1ENR |= ((uint32_t)0x10000000); //RCC_APB1ENR_PWREN
    ((PWR_TypeDef *) (((uint32_t)0x40000000) + 0x7000))->CR |= ((uint16_t)0x4000);
    MYRCC->CFGR |= ((uint32_t)0x00000000); //RCC_CFGR_HPRE_DIV1 SYSCLK not divided
    MYRCC->CFGR |= ((uint32_t)0x00008000); //RCC_CFGR_PPRE2_DIV2 HCLK divided by 2
    MYRCC->CFGR |= ((uint32_t)0x00001400); //RCC_CFGR_PPRE1_DIV4 HCLK divided by 4
    /*
    PLL_M 8
    PLL_N 336
    PLL_P 2
    PLL_Q 7
    */
    MYRCC->PLLCFGR = 8 | (336 << 6) | (((2 >> 1) -1) << 16) | (((uint32_t)0x00400000)) | (7 << 24);
    MYRCC->CR |= ((uint32_t)0x01000000); //RCC_CR_PLLON

    while((MYRCC->CR & ((uint32_t)0x02000000)) == 0);

    ((FLASH_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3C00))->ACR = ((uint32_t)0x00000200) |((uint32_t)0x00000400) |((uint32_t)0x00000005);
    MYRCC->CFGR &= (uint32_t)((uint32_t)~(((uint32_t)0x00000003))); //RCC_CFGR_SW
    MYRCC->CFGR |= ((uint32_t)0x00000002); //RCC_CFGR_SW_PLL

    while ((MYRCC->CFGR & (uint32_t)((uint32_t)0x0000000C) ) != ((uint32_t)0x00000008));
  }

  #if 0
  MYRCC->CFGR &= ~((uint32_t)0x00800000);
  MYRCC->PLLI2SCFGR = (192 << 6) | (5 << 28);
  MYRCC->CR |= ((uint32_t)((uint32_t)0x04000000));

  while((MYRCC->CR & ((uint32_t)0x08000000)) == 0);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR = ((uint32_t)0x20000000) | 0x00;
  #endif
}

int main() {
//    system_reinit();
    return usb_test_main();
}
