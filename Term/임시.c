#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "lcd.h"
#include "touch.h"

#define PC_ODR		*(volatile uint32_t *)0x4001100c
#define PC_IDR		*(volatile uint32_t *)0x40011008

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

volatile uint32_t ADC_Value[1];
char inputOperation = '\0';
int now_go = 1; // 1 for front, 2 for backward, 3 for stop

void RCC_Configure(void) {
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Motordriver 1 pin(d2, d3, d4, d7) setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Motordriver 2 pin(d8, d9, d10, d12) setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Motordriver pwm pins setting*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* ultrasonic trigger pin(c1, c2, c13) setting */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ultrasonic echo pin(c3, c4, c14) setting */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
       
  /* hit sensor pin(c0) setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* usart2 pin() setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DMA_Configure(void) 
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_Configure(void) {
  ADC_InitTypeDef ADC_InitStructure;
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);

  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);

  while(ADC_GetResetCalibrationStatus(ADC1)) ;

  ADC_StartCalibration(ADC1);

  while(ADC_GetCalibrationStatus(ADC1)) ;

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  
  ADC_DMACmd(ADC1, ENABLE);
}

void TIM_Configure(void) { // 시간 측정을 위한 카운터로 사용하기 위해 TIM2 이용
  uint16_t prescale = (uint16_t) 7200 - 1;
  
  TIM_TimeBaseStructure.TIM_Period = 10000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = prescale;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  // TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void USART2_Init(void) {
  USART_InitTypeDef USART_InitStructure;
  // Enable the USART2 peripheral
  USART_Cmd(USART2, ENABLE);

  USART_InitStructure.USART_BaudRate = 9600;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
  USART_InitStructure.USART_Parity = USART_Parity_No;  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(USART2, &USART_InitStructure);  

  // Enable the USART2 RX interrupts
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        inputOperation = USART_ReceiveData(USART2) & 0xFF;
    	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void NVIC_Configuration(void) {      
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);					

    // USART2 
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(USART2_IRQn);
}

/* Motor */
void motorfl(int n) {
  // Uses d2, d3
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  }
}
void motorfr(int n) {
  // Uses d4, d7
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_SetBits(GPIOD, GPIO_Pin_4);
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    GPIO_SetBits(GPIOD, GPIO_Pin_7);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
  }
}
void motorbl(int n) {
  // Uses d8, d9
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    GPIO_ResetBits(GPIOD, GPIO_Pin_9);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_8);
    GPIO_SetBits(GPIOD, GPIO_Pin_9);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_8);
    GPIO_ResetBits(GPIOD, GPIO_Pin_9);
  }
}
void motorbr(int n) {
  // Uses d10, d12
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_10);
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_SetBits(GPIOD, GPIO_Pin_10);
    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_10);
    GPIO_ResetBits(GPIOD, GPIO_Pin_12);
  }
}

void setFront() {
  motorfl(0);
  motorfr(0);
  motorbl(0);
  motorbr(0);
}
void setBack() {
  motorfl(1);
  motorfr(1);
  motorbl(1);
  motorbr(1);
}
void setRight() {
  motorfl(0);
  motorfr(2);
  motorbl(0);
  motorbr(2);
}
void setLeft() {
  motorfl(2);
  motorfr(0);
  motorbl(2);
  motorbr(0);
}
void setStop() {
  motorfl(2);
  motorfr(2);
  motorbl(2);
  motorbr(2);
}

/* Ultrasonic */
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



int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  TIM_Configure();
  ADC_Configure();
  DMA_Configure();
  LCD_Init();
  Touch_Configuration();
  LCD_Clear(WHITE);
  
  int drive_mode = 0; // 0 for Auto, 1 for Manual
  while (1) {
    // TODO: implement
    // Front move
    //uint32_t fr_sensor = getDistance(13, 14); // 초음파 센서 :앞이 trig / 뒤가 echo
    //uint32_t rt_sensor = getDistance(1, 3);
    //uint32_t lt_sensor = getDistance(2, 4);
    uint32_t if_stuck = ADC_Value[0]; // 충돌센서
    
    //LCD_ShowNum(40, 40, fr_sensor, 10 , RED, WHITE);
    //LCD_ShowNum(40, 60, rt_sensor, 10 , RED, WHITE);
    //LCD_ShowNum(40, 80, lt_sensor, 10 , RED, WHITE);
    LCD_ShowNum(40, 100, if_stuck, 4 , RED, WHITE);
    /*if(drive_mode == 0) {
      if(fr_sensor < 5) {
        setBack();
        delay_us(1000000);
      }
      else if(fr_sensor < 15) {
        if(rt_sensor < lt_sensor) {
          setLeft();
        }
        else if(lt_sensor < rt_sensor) {
          setRight();
        }
      }
      else if(lt_sensor < 5) {
          setRight();
      }
      else if(rt_sensor < 5) {
          setLeft();
      }
      else {
        setFront();
      }
    }
    else {
      if(now_go == 1) { setFront(); }
      else if(now_go == 2) { setBack(); }
      else if(now_go == 3) { setStop(); }
    }*/
  }
  
  return 0;
}