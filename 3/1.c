#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

TIM_HandleTypeDef htim2;

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM2_Init();

	HAL_TIM_Base_Start_IT(&htim2);

	while (1)
	{
	}
}

void MX_TIM2_Init(void)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 8399;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 5000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim->Instance == TIM2)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

void MX_GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
