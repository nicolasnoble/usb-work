#include <stdint.h>
#include <stm32f4xx.h>

int usb_test_main();

void system_reinit(void) {
  volatile uint32_t StartUpCounter = 0, HSEStatus = 0;

  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR |= (uint32_t)0x00000001;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR = 0x00000000;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR &= (uint32_t)0xFEF6FFFF;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->PLLCFGR = 0x24003010;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR &= (uint32_t)0xFFFBFFFF;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CIR = 0x00000000;
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR |= ((uint32_t)((uint32_t)0x00010000));

  do {
    HSEStatus = ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR & ((uint32_t)0x00020000);
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != ((uint16_t)0x0500)));

  if ((((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR & ((uint32_t)0x00020000)) != RESET) {
    HSEStatus = (uint32_t)0x01;

    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->APB1ENR |= ((uint32_t)0x10000000);
    ((PWR_TypeDef *) (((uint32_t)0x40000000) + 0x7000))->CR |= ((uint16_t)0x4000);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR |= ((uint32_t)0x00000000);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR |= ((uint32_t)0x00008000);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR |= ((uint32_t)0x00001400);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->PLLCFGR = 8 | (336 << 6) | (((2 >> 1) -1) << 16) | (((uint32_t)0x00400000)) | (7 << 24);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR |= ((uint32_t)0x01000000);

    while((((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR & ((uint32_t)0x02000000)) == 0);

    ((FLASH_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3C00))->ACR = ((uint32_t)0x00000200) |((uint32_t)0x00000400) |((uint32_t)0x00000005);
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR &= (uint32_t)((uint32_t)~(((uint32_t)0x00000003)));
    ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR |= ((uint32_t)0x00000002);

    while ((((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR & (uint32_t)((uint32_t)0x0000000C) ) != ((uint32_t)0x00000008));
  }

  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CFGR &= ~((uint32_t)0x00800000);
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->PLLI2SCFGR = (192 << 6) | (5 << 28);
  ((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR |= ((uint32_t)((uint32_t)0x04000000));

  while((((RCC_TypeDef *) ((((uint32_t)0x40000000) + 0x00020000) + 0x3800))->CR & ((uint32_t)0x08000000)) == 0);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR = ((uint32_t)0x20000000) | 0x00;
}

int main() {
    system_reinit();
    return usb_test_main();
}
