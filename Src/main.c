/**
  * Demo: Write Option Bytes
  * 
  * Board: PY32F003W1XS (SOP16)
  * 
  * This demo shows how to config reset pin as gpio output
  */

#include "main.h"
#include "py32f0xx_bsp_printf.h"
#include "py32f0xx_bsp_clock.h"


static void APP_GPIOConfig(void);
static void APP_FlashSetOptionBytes(void);

int main(void)
{
  BSP_RCC_HSI_24MConfig();

  LL_mDelay(1000);

  if(READ_BIT(FLASH->OPTR, FLASH_OPTR_NRST_MODE) == OB_RESET_MODE_RESET)
  {
    /* This will reset the MCU */
    APP_FlashSetOptionBytes();
    printf("Configurate RESET as GPIO\r\n");
  }
  else
  {
    printf("RESET has been configurated as GPIO\r\n");
  }
  /* Don't config GPIO before changing the option bytes */
  APP_GPIOConfig();
  BSP_USART_Config(115200);
  printf("PY32F0xx GPIO Example\r\nClock: %ld\r\n", SystemCoreClock);

  while (1)
  {
    LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
    printf("echo\r\n");
    LL_mDelay(1000);
  }
}

static void APP_GPIOConfig(void)
{
  // PA0
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
  // PF0 -> GPIO output, PF2 -> analog
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
  LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
  // PY32F003W1XS: PF0 and PF2 share the same pin, it's recommended to set PF2 as analog
  LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_2, LL_GPIO_MODE_ANALOG);
}

static void APP_FlashSetOptionBytes(void)
{
  FLASH_OBProgramInitTypeDef OBInitCfg;

  LL_FLASH_Unlock();
  LL_FLASH_OB_Unlock();

  OBInitCfg.OptionType = OPTIONBYTE_USER;
  OBInitCfg.USERType = OB_USER_BOR_EN | OB_USER_BOR_LEV | OB_USER_IWDG_SW  | OB_USER_NRST_MODE | OB_USER_nBOOT1;
  /*
   * The default value: OB_BOR_DISABLE | OB_BOR_LEVEL_3p1_3p2 | OB_IWDG_SW | OB_WWDG_SW | OB_RESET_MODE_RESET | OB_BOOT1_SYSTEM;
  */
  OBInitCfg.USERConfig = OB_BOR_DISABLE | OB_BOR_LEVEL_3p1_3p2 | OB_IWDG_SW  | OB_RESET_MODE_GPIO | OB_BOOT1_SYSTEM;
  LL_FLASH_OBProgram(&OBInitCfg);

  LL_FLASH_Lock();
  LL_FLASH_OB_Lock();
  /* Reload option bytes */
  LL_FLASH_OB_Launch();
}

void APP_ErrorHandler(void)
{
  while (1);
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  while (1);
}
#endif /* USE_FULL_ASSERT */
