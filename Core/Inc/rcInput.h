#ifndef RC_INPUT_H
#define RC_INPUT_H

#include "stm32f3xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#define RC_INPUT_CHANNEL_COUNT 4U

typedef struct
{
  uint16_t pulseWidthUs[RC_INPUT_CHANNEL_COUNT];
  bool signalValid[RC_INPUT_CHANNEL_COUNT];
  uint32_t lastUpdateMs[RC_INPUT_CHANNEL_COUNT];
} RcInputState;

/***************** rcInputInit ************************************************
 * params: TIM_HandleTypeDef *timerHandle
 * return: void
 * Description:
 * Initializes the RC input capture module with the given timer handle.
 ******************************************************************************/
void rcInputInit(TIM_HandleTypeDef *timerHandle);

/***************** rcInputStart ***********************************************
 * params: none
 * return: void
 * Description:
 * Starts input capture on all configured RC input channels.
 ******************************************************************************/
void rcInputStart(void);

/***************** rcInputProcessCapture **************************************
 * params: TIM_HandleTypeDef *timerHandle
 * return: void
 * Description:
 * Processes timer input capture callbacks to measure pulse widths.
 ******************************************************************************/
void rcInputProcessCapture(TIM_HandleTypeDef *timerHandle);

/***************** rcInputUpdate **********************************************
 * params: uint32_t nowMs
 * return: void
 * Description:
 * Updates signal validity based on the failsafe timeout.
 ******************************************************************************/
void rcInputUpdate(uint32_t nowMs);

/***************** rcInputGetState ********************************************
 * params: none
 * return: RcInputState
 * Description:
 * Returns the latest RC input state.
 ******************************************************************************/
RcInputState rcInputGetState(void);

#endif
