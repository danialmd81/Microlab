#include "main.h"
#include "lcd.h"  // اضافه کردن کتابخانه LCD

TIM_HandleTypeDef htim2;
uint32_t capture_value = 0;
uint32_t previous_capture = 0;
uint32_t frequency = 0;
uint32_t timer_clock = 84000000;  // فرض می‌کنیم کلاک تایمر 84MHz است

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        
        // محاسبه دوره تناوب سیگنال ورودی
        if (capture_value > previous_capture)
        {
            uint32_t period = capture_value - previous_capture;
            frequency = timer_clock / (htim2.Init.Prescaler + 1) / period;
        }
        
        previous_capture = capture_value;

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);  // نمایش رخداد لبه بالارونده

        // نمایش مقدار فرکانس روی LCD
        char freq_str[16];
        sprintf(freq_str, "Freq: %lu Hz", frequency);
        lcd_clear();
        lcd_puts(freq_str);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();

    lcd_init();  // مقداردهی اولیه LCD
    lcd_clear();
    lcd_puts("Measuring...");

    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    while (1)
    {
        // می‌توان فرکانس را روی LCD به‌روزرسانی کرد
    }
}

void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 83;  // تبدیل 84MHz به 1MHz (1us رزولوشن)
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFFFFFF;  // حداکثر مقدار برای جلوگیری از سرریز سریع
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    TIM_IC_InitTypeDef sConfigIC = {0};
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;

    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}




///stm32f4xx_it.c

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)  // سرریز تایمر
    {
        TIM2->SR &= ~TIM_SR_UIF;
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
    }

    if (TIM2->SR & TIM_SR_CC1IF)  // وقفه لبه بالارونده
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
    }

    HAL_TIM_IRQHandler(&htim2);
}
