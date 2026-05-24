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

volatile aesDmaProgress_enum aesDmaState = AES_DMA_OFF;

/* USER CODE END 0 */

CRYP_HandleTypeDef hcryp;
uint32_t pKeyAES[4] = {0x2B7E1516,0x28AED2A6,0xABF71588,0x09CF4F3C};
uint32_t pInitVectAES[4] = {0x00010203,0x04050607,0x08090A0B,0x0C0D0E0F};
DMA_HandleTypeDef hdma_aes_in;
DMA_HandleTypeDef hdma_aes_out;

/* AES init function */
void MX_AES_Init(void)
{

  /* USER CODE BEGIN AES_Init 0 */

  /* USER CODE END AES_Init 0 */

  /* USER CODE BEGIN AES_Init 1 */

  /* USER CODE END AES_Init 1 */
  hcryp.Instance = AES;
  hcryp.Init.DataType = CRYP_DATATYPE_8B;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES_Init 2 */

  /* USER CODE END AES_Init 2 */

}

void HAL_CRYP_MspInit(CRYP_HandleTypeDef* crypHandle)
{

  if(crypHandle->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspInit 0 */

  /* USER CODE END AES_MspInit 0 */
    /* AES clock enable */
    __HAL_RCC_AES_CLK_ENABLE();

    /* AES DMA Init */
    /* AES_IN Init */
    hdma_aes_in.Instance = DMA1_Channel1;
    hdma_aes_in.Init.Request = DMA_REQUEST_AES_IN;
    hdma_aes_in.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_aes_in.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_aes_in.Init.MemInc = DMA_MINC_ENABLE;
    hdma_aes_in.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_aes_in.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_aes_in.Init.Mode = DMA_NORMAL;
    hdma_aes_in.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_aes_in) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(crypHandle,hdmain,hdma_aes_in);

    /* AES_OUT Init */
    hdma_aes_out.Instance = DMA1_Channel2;
    hdma_aes_out.Init.Request = DMA_REQUEST_AES_OUT;
    hdma_aes_out.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_aes_out.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_aes_out.Init.MemInc = DMA_MINC_ENABLE;
    hdma_aes_out.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_aes_out.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_aes_out.Init.Mode = DMA_NORMAL;
    hdma_aes_out.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_aes_out) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(crypHandle,hdmaout,hdma_aes_out);

    /* AES interrupt Init */
    HAL_NVIC_SetPriority(RNG_CRYP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RNG_CRYP_IRQn);
  /* USER CODE BEGIN AES_MspInit 1 */

  /* USER CODE END AES_MspInit 1 */
  }
}

void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef* crypHandle)
{

  if(crypHandle->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspDeInit 0 */

  /* USER CODE END AES_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_AES_CLK_DISABLE();

    /* AES DMA DeInit */
    HAL_DMA_DeInit(crypHandle->hdmain);
    HAL_DMA_DeInit(crypHandle->hdmaout);

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

void SecureComms_EncryptPayload(uint32_t *input, uint16_t inputSize, uint32_t *output) {
	if (HAL_CRYP_Encrypt(&hcryp, input, inputSize, output, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}
}

void SecureComms_DecryptPayload(uint32_t *input, uint32_t *output, uint16_t outputSize) {
	/* After encryption in CBC mode reinitialisation is required
	 so the last generated block in AES hardware module is not used.
	 It ensures that IV will be used and not the last block.
	 */
	if (hcryp.Init.KeyIVConfigSkip != CRYP_KEYIVCONFIG_ALWAYS) {
		HAL_CRYP_DeInit(&hcryp);
		MX_AES_Init();
	}

	if (HAL_CRYP_Decrypt(&hcryp, input, outputSize, output, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}
}

/* --- */

void SecureComms_EncryptPayload_DMA(uint32_t *input, uint16_t inputSize, uint32_t *output) {
    aesDmaState = AES_DMA_ENCRYPTING;

    if (HAL_CRYP_Encrypt_DMA(&hcryp, input, inputSize, output) != HAL_OK) {
        Error_Handler();
    }
}

void SecureComms_DecryptPayload_DMA(uint32_t *input, uint32_t *output, uint16_t outputSize) {
	aesDmaState = AES_DMA_DECRYPTING;

	/* After encryption in CBC mode reinitialisation is required
	 so the last generated block in AES hardware module is not used.
	 It ensures that IV will be used and not the last block.
	 */
	if (hcryp.Init.KeyIVConfigSkip != CRYP_KEYIVCONFIG_ALWAYS) {
		HAL_CRYP_DeInit(&hcryp);
		MX_AES_Init();
	}

	if (HAL_CRYP_Decrypt_DMA(&hcryp, input, outputSize, output) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp) {
    if (hcryp->Instance == AES) {

    	switch(aesDmaState) {
    	case AES_DMA_ENCRYPTING:
			aesDmaState = AES_DMA_ENCRYPTED;
			break;
    	case AES_DMA_DECRYPTING:
    		aesDmaState=AES_DMA_DECRYPTED;
    		break;
    	}

    }
}

/* USER CODE END 1 */

