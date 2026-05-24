/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    rng.c
 * @brief   This file provides code for the configuration
 *          of the RNG instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rng.h"

/* USER CODE BEGIN 0 */

volatile uint32_t randomSeed = 0;
volatile uint8_t randomSeedReady = 0;

/* USER CODE END 0 */

RNG_HandleTypeDef hrng;

/* RNG init function */
void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

void HAL_RNG_MspInit(RNG_HandleTypeDef* rngHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspInit 0 */

  /* USER CODE END RNG_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG;
    PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* RNG clock enable */
    __HAL_RCC_RNG_CLK_ENABLE();

    /* RNG interrupt Init */
    HAL_NVIC_SetPriority(RNG_CRYP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RNG_CRYP_IRQn);
  /* USER CODE BEGIN RNG_MspInit 1 */

  /* USER CODE END RNG_MspInit 1 */
  }
}

void HAL_RNG_MspDeInit(RNG_HandleTypeDef* rngHandle)
{

  if(rngHandle->Instance==RNG)
  {
  /* USER CODE BEGIN RNG_MspDeInit 0 */

  /* USER CODE END RNG_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RNG_CLK_DISABLE();

    /* RNG interrupt Deinit */
  /* USER CODE BEGIN RNG:RNG_CRYP_IRQn disable */
		/**
		 * Uncomment the line below to disable the "RNG_CRYP_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(RNG_CRYP_IRQn); */
  /* USER CODE END RNG:RNG_CRYP_IRQn disable */

  /* USER CODE BEGIN RNG_MspDeInit 1 */

  /* USER CODE END RNG_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

uint32_t Security_GetRandomSeed(void) {
	uint32_t randomNumber = 0;

	//blocking mode: waiting for DRDY flag
	HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, &randomNumber);

	if (status == HAL_OK) {
		return randomNumber;
	} else {
		Error_Handler();
		return 0;
	}
}

/* --- */

uint32_t Security_GetRandomSeed_IT(void) {

	HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber_IT(&hrng);

	if (status != HAL_OK) {
		Error_Handler();
	}

	return status;
}

void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit) {
	if (hrng->Instance == RNG) {
		randomSeed = random32bit;
		randomSeedReady = 1;
	}
}

void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng) {
	if (hrng->Instance == RNG) {
		Error_Handler();
	}
}
/* USER CODE END 1 */

