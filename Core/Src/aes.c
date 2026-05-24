/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    aes.c
 * @brief   This file provides code for the configuration
 *          of the AES instances.
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
#include "aes.h"

/* USER CODE BEGIN 0 */

uint8_t PlaintxtData[] = "STM32U0_SECURE!!";
uint8_t CipherOutput[16] = { 0 };
uint8_t DecryptedData[16] = { 0 };

/* USER CODE END 0 */

CRYP_HandleTypeDef hcryp;
uint32_t pKeyAES[4] = { 0x2B7E1516, 0x28AED2A6, 0xABF71588, 0x09CF4F3C };
uint32_t pInitVectAES[4] = { 0x00010203, 0x04050607, 0x08090A0B, 0x0C0D0E0F };

/* AES init function */
void MX_AES_Init(void) {

	/* USER CODE BEGIN AES_Init 0 */

	/* USER CODE END AES_Init 0 */

	/* USER CODE BEGIN AES_Init 1 */

	/* USER CODE END AES_Init 1 */
	hcryp.Instance = AES;
	hcryp.Init.DataType = CRYP_DATATYPE_8B;
	hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
	hcryp.Init.pKey = (uint32_t*) pKeyAES;
	hcryp.Init.pInitVect = (uint32_t*) pInitVectAES;
	hcryp.Init.Algorithm = CRYP_AES_CBC;
	hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;
	hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
	hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
	if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN AES_Init 2 */

	/* USER CODE END AES_Init 2 */

}

void HAL_CRYP_MspInit(CRYP_HandleTypeDef *crypHandle) {

	if (crypHandle->Instance == AES) {
		/* USER CODE BEGIN AES_MspInit 0 */

		/* USER CODE END AES_MspInit 0 */
		/* AES clock enable */
		__HAL_RCC_AES_CLK_ENABLE();

		/* AES interrupt Init */
		HAL_NVIC_SetPriority(RNG_CRYP_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(RNG_CRYP_IRQn);
		/* USER CODE BEGIN AES_MspInit 1 */

		/* USER CODE END AES_MspInit 1 */
	}
}

void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *crypHandle) {

	if (crypHandle->Instance == AES) {
		/* USER CODE BEGIN AES_MspDeInit 0 */

		/* USER CODE END AES_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_AES_CLK_DISABLE();

		/* AES interrupt Deinit */
		/* USER CODE BEGIN AES:RNG_CRYP_IRQn disable */
		/**
		 * Uncomment the line below to disable the "RNG_CRYP_IRQn" interrupt
		 * Be aware, disabling shared interrupt may affect other IPs
		 */
		/* HAL_NVIC_DisableIRQ(RNG_CRYP_IRQn); */
		/* USER CODE END AES:RNG_CRYP_IRQn disable */

		/* USER CODE BEGIN AES_MspDeInit 1 */

		/* USER CODE END AES_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

void SecureComms_EncryptPayload(void) {
	if (HAL_CRYP_Encrypt(&hcryp, (uint32_t*) PlaintxtData, 16, (uint32_t*) CipherOutput, HAL_MAX_DELAY) != HAL_OK) {

		Error_Handler();
	}
}

void SecureComms_DecryptPayload(void) {
	/* CBC requires reinitialisation for summary symmetry */
	HAL_CRYP_DeInit(&hcryp);
	MX_AES_Init();

	if (HAL_CRYP_Decrypt(&hcryp, (uint32_t*) CipherOutput, 16, (uint32_t*) DecryptedData, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}

}

/* USER CODE END 1 */

