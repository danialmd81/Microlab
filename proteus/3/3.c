#include "main.h"
#include "lcd.h"  // کتابخانه LCD برای نمایش ساعت
#include <stdio.h>

RTC_HandleTypeDef hrtc;
char time_str[16];  // ذخیره رشته نمایش زمان

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C1_Init(void); // برای LCD با I2C

void display_time_on_lcd(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_RTC_Init();
    MX_I2C1_Init();  // اگر LCD از I2C استفاده می‌کند

    lcd_init();  // مقداردهی اولیه LCD
    lcd_clear();
    lcd_puts("Clock Started...");

    while (1)
    {
        display_time_on_lcd();
        HAL_Delay(1000);  // هر 1 ثانیه به‌روز شود
    }
}

// 📌 نمایش زمان روی LCD
void display_time_on_lcd(void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    sprintf(time_str, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
    
    lcd_clear();
    lcd_puts("Time: ");
    lcd_puts(time_str);
}

// 📌 مقداردهی اولیه RTC
void MX_RTC_Init(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }

    // مقدار اولیه ساعت 12:00:00 تنظیم می‌شود
    RTC_TimeTypeDef sTime = {12, 0, 0, RTC_HOURFORMAT12_AM};
    RTC_DateTypeDef sDate = {1, RTC_MONTH_JANUARY, 2025, RTC_WEEKDAY_MONDAY};

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

// 📌 مقداردهی اولیه I2C برای LCD
void MX_I2C1_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    I2C_HandleTypeDef hi2c1;
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}
