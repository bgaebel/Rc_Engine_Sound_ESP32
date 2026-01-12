#include "rcInput.h"
#include "boardConfig.h"

static TIM_HandleTypeDef *inputTimer = NULL;
static RcInputState rcState;
static uint32_t risingEdgeTicks[RC_INPUT_CHANNEL_COUNT];
static bool waitingForFallingEdge[RC_INPUT_CHANNEL_COUNT];

static uint32_t rcInputGetTimerPeriod(TIM_HandleTypeDef *timerHandle)
{
  return timerHandle->Init.Period;
}

static uint32_t rcInputGetChannelIndex(uint32_t channel)
{
  if (channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    return 0U;
  }
  if (channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    return 1U;
  }
  if (channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    return 2U;
  }
  return 3U;
}

static uint32_t rcInputGetActiveChannel(TIM_HandleTypeDef *timerHandle)
{
  if (timerHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
    return TIM_CHANNEL_1;
  }
  if (timerHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    return TIM_CHANNEL_2;
  }
  if (timerHandle->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    return TIM_CHANNEL_3;
  }
  return TIM_CHANNEL_4;
}

/***************** rcInputInit ************************************************
 * params: TIM_HandleTypeDef *timerHandle
 * return: void
 * Description:
 * Initializes the RC input capture module with the given timer handle.
 ******************************************************************************/
void rcInputInit(TIM_HandleTypeDef *timerHandle)
{
  inputTimer = timerHandle;
  for (uint32_t i = 0U; i < RC_INPUT_CHANNEL_COUNT; i++)
  {
    rcState.pulseWidthUs[i] = RC_PULSE_NEUTRAL_US;
    rcState.signalValid[i] = false;
    rcState.lastUpdateMs[i] = 0U;
    risingEdgeTicks[i] = 0U;
    waitingForFallingEdge[i] = false;
  }
}

/***************** rcInputStart ***********************************************
 * params: none
 * return: void
 * Description:
 * Starts input capture on all configured RC input channels.
 ******************************************************************************/
void rcInputStart(void)
{
  if (inputTimer == NULL)
  {
    return;
  }

  HAL_TIM_IC_Start_IT(inputTimer, RC_INPUT_TIMER_CHANNEL_1);
  HAL_TIM_IC_Start_IT(inputTimer, RC_INPUT_TIMER_CHANNEL_2);
  HAL_TIM_IC_Start_IT(inputTimer, RC_INPUT_TIMER_CHANNEL_3);
  HAL_TIM_IC_Start_IT(inputTimer, RC_INPUT_TIMER_CHANNEL_4);
}

/***************** rcInputProcessCapture **************************************
 * params: TIM_HandleTypeDef *timerHandle
 * return: void
 * Description:
 * Processes timer input capture callbacks to measure pulse widths.
 ******************************************************************************/
void rcInputProcessCapture(TIM_HandleTypeDef *timerHandle)
{
  if (timerHandle != inputTimer)
  {
    return;
  }

  uint32_t channelIndex = rcInputGetChannelIndex(timerHandle->Channel);
  uint32_t activeChannel = rcInputGetActiveChannel(timerHandle);
  uint32_t captureValue = HAL_TIM_ReadCapturedValue(timerHandle, activeChannel);

  if (!waitingForFallingEdge[channelIndex])
  {
    risingEdgeTicks[channelIndex] = captureValue;
    waitingForFallingEdge[channelIndex] = true;
    __HAL_TIM_SET_CAPTUREPOLARITY(timerHandle, activeChannel, TIM_INPUTCHANNELPOLARITY_FALLING);
    return;
  }

  uint32_t period = rcInputGetTimerPeriod(timerHandle);
  uint32_t pulseTicks;
  if (captureValue >= risingEdgeTicks[channelIndex])
  {
    pulseTicks = captureValue - risingEdgeTicks[channelIndex];
  }
  else
  {
    pulseTicks = (period + 1U) - risingEdgeTicks[channelIndex] + captureValue;
  }

  uint32_t pulseWidthUs = (pulseTicks * 1000000U) / RC_INPUT_TIMER_HZ;
  if (pulseWidthUs > 0xFFFFU)
  {
    pulseWidthUs = 0xFFFFU;
  }

  rcState.pulseWidthUs[channelIndex] = (uint16_t)pulseWidthUs;
  rcState.lastUpdateMs[channelIndex] = HAL_GetTick();
  rcState.signalValid[channelIndex] = true;

  waitingForFallingEdge[channelIndex] = false;
  __HAL_TIM_SET_CAPTUREPOLARITY(timerHandle, activeChannel, TIM_INPUTCHANNELPOLARITY_RISING);
}

/***************** rcInputUpdate **********************************************
 * params: uint32_t nowMs
 * return: void
 * Description:
 * Updates signal validity based on the failsafe timeout.
 ******************************************************************************/
void rcInputUpdate(uint32_t nowMs)
{
  for (uint32_t i = 0U; i < RC_INPUT_CHANNEL_COUNT; i++)
  {
    if ((nowMs - rcState.lastUpdateMs[i]) > RC_FAILSAFE_TIMEOUT_MS)
    {
      rcState.signalValid[i] = false;
    }
  }
}

/***************** rcInputGetState ********************************************
 * params: none
 * return: RcInputState
 * Description:
 * Returns the latest RC input state.
 ******************************************************************************/
RcInputState rcInputGetState(void)
{
  return rcState;
}
