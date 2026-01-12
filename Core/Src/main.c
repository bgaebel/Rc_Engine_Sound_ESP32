#include "main.h"
#include "audioEngine.h"
#include "boardConfig.h"
#include "engineModel.h"
#include "rcInput.h"

I2S_HandleTypeDef hi2s2;
TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_spi2_tx;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S2_Init(void);
static void MX_TIM2_Init(void);

/***************** main *******************************************************
 * params: none
 * return: int
 * Description:
 * Application entry point for the STM32CubeIDE port.
 ******************************************************************************/
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S2_Init();
  MX_TIM2_Init();

  engineModelInit();
  rcInputInit(&htim2);
  rcInputStart();

  audioEngineInit(&hi2s2);
  audioEngineStart();

  uint32_t lastControlTickMs = HAL_GetTick();

  while (1)
  {
    uint32_t nowMs = HAL_GetTick();
    if ((nowMs - lastControlTickMs) >= 2U)
    {
      lastControlTickMs = nowMs;
      rcInputUpdate(nowMs);

      RcInputState rcState = rcInputGetState();
      engineModelUpdate(rcState.pulseWidthUs[1], rcState.signalValid[1], nowMs);
      audioEngineControlTick(nowMs);
    }
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  rcInputProcessCapture(htim);
}

/***************** SystemClock_Config *****************************************
 * params: none
 * return: void
 * Description:
 * Configures system clock. Replace with CubeMX-generated implementation.
 ******************************************************************************/
static void SystemClock_Config(void)
{
  __disable_irq();
  while (1)
  {
  }
}

/***************** MX_GPIO_Init ***********************************************
 * params: none
 * return: void
 * Description:
 * Initializes GPIO. Replace with CubeMX-generated implementation.
 ******************************************************************************/
static void MX_GPIO_Init(void)
{
}

/***************** MX_DMA_Init ************************************************
 * params: none
 * return: void
 * Description:
 * Initializes DMA. Replace with CubeMX-generated implementation.
 ******************************************************************************/
static void MX_DMA_Init(void)
{
}

/***************** MX_I2S2_Init ***********************************************
 * params: none
 * return: void
 * Description:
 * Initializes I2S2. Replace with CubeMX-generated implementation.
 ******************************************************************************/
static void MX_I2S2_Init(void)
{
}

/***************** MX_TIM2_Init ***********************************************
 * params: none
 * return: void
 * Description:
 * Initializes TIM2 for PWM input capture. Replace with CubeMX-generated implementation.
 ******************************************************************************/
static void MX_TIM2_Init(void)
{
}

/***************** Error_Handler **********************************************
 * params: none
 * return: void
 * Description:
 * Handles unrecoverable errors.
 ******************************************************************************/
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
