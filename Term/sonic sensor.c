#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "lcd.h"
#include "touch.h"

void delay_us(uint32_t us) {
  if(us > 1) {
    uint32_t count = us * 8 - 6;
    while(count--);
  }
  else {
    uint32_t count = 2;
    while(count--);
  }
}

void RCC_Configure(void) {
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
  // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
  GPIO_InitTypeDef GPIO_InitStructure;

  // 초음파 trigger
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // 초음파 echo
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void TIM_Configure(void) { // 시간 측정을 위한 카운터로 사용하기 위해 TIM2 이용
  prescale = (uint16_t) 7200 - 1;

  TIM_TimeBaseStructure.TIM_Period = 10000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = prescale;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  // TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void trig_pulse(uint16_t trigPin)  {
  PC_ODR |= (1 << trigPin); // GPIO_SetBits
  delay_us(11);
  PC_ODR &= ~(1 << trigPin); // GPIO_ResetBits
  delay_us(11);
}

uint32_t getDistance(uint16_t trigPin, uint16_t echoPin) {
  uint32_t echo;
  trig_pulse(trigPin); // give trig pulse to u_sonic sensor

  while((PC_IDR & (1 << echoPin)) != (1 << echoPin)); // wait echo pin status turns HIGH
  echo = TIM2->CNT;

  while((PC_IDR & (1 << echoPin)) == (1 << echoPin));
  echo = TIM2->CNT - echo;

  return echo;
}

int main() {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  TIM_Configure();
  // ------------------------------------
  LCD_Init();
  Touch_Configuration();
  LCD_Clear(WHITE);

  while(1){
    LCD_ShowNum(40, 40, getDistance(), 10, RED, WHITE);
    delay();
  }
}
