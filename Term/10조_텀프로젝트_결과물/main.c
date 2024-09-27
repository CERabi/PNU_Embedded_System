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

uint16_t input = '\0';
int drive_mode = 0; // 0 for Auto, 1 for Manual
int now_go = 1; // 1 for front, 2 for backward, 3 for stop
int stuck = 0;

void RCC_Configure(void) {
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Motordriver 1 pin(d2, d3, d4, d7) setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_7;
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
  GPIO_InitStructure.GPIO_Speed = 0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* usart2 pin() setting */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* buzzer pin(b0) setting*/
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void TIM_Configure(void) { //  ??              ?   ?      ?       TIM2  ? 
  uint16_t prescale = (uint16_t) 7200 - 1;
  
  TIM_TimeBaseStructure.TIM_Period = 10000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = prescale;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  // TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
   
  prescale = (uint16_t) 1000-1;

  TIM_TimeBaseStructure.TIM_Period = 36-1; 
  TIM_TimeBaseStructure.TIM_Prescaler = prescale-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0; // us
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
}

void NVIC_Configure(void) {      
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);			
    
    // Sensor Active
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    // USART2 
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           	
    NVIC_Init(&NVIC_InitStructure);

}

void USART2_Init(void) {
  USART_InitTypeDef USART_InitStructure;
  // Enable the USART2 peripheral
  USART_Cmd(USART2, ENABLE);

  USART_InitStructure.USART_BaudRate = 9600;  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  
  USART_InitStructure.USART_Parity = USART_Parity_No;  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(USART2, &USART_InitStructure);  

  // Enable the USART2 RX interrupts
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void bluetooth(void) {
    // drive mode
    if(input == 'a') { // auto
      drive_mode = 0;
      now_go = 3;
    }
    if(input == 'm') { // manual
      drive_mode = 1;
      now_go = 3;
    }
    // direction
    if(input== 'f') { // front
      now_go = 1;
    }
    if(input == 'b') { // back
      now_go = 2;
    }
    if(input == 's') { //stop
      now_go = 3;
    }
//    input = '\0';
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        input = USART_ReceiveData(USART2);
        bluetooth();
    	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

/* Motor */
void motorfl(int n) {
  // Uses d2, d3
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
    GPIO_ResetBits(GPIOD, GPIO_Pin_1);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    GPIO_SetBits(GPIOD, GPIO_Pin_1);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    GPIO_ResetBits(GPIOD, GPIO_Pin_1);
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

/* Buzzer */
void alert_buzzer(void) {
  for(int i = 0; i < 5; i++) {
    TIM3->PSC = 1000;
    TIM3->CCR1 = TIM3->ARR / 2;
    delay_us(700000);
    TIM3->PSC = 0;
    TIM3->CCR1 = 0;
    delay_us(300000);
  }
}

/* Crash sensor */
void EXTI_Configure(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
	    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line0)!= RESET) {
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == Bit_RESET) {                  
      now_go = 3;
      drive_mode = 1;
      stuck = 1;
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}


/* main function */
int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  NVIC_Configure();
  EXTI_Configure();
  TIM_Configure();
  USART2_Init();
  LCD_Init();
  Touch_Configuration();
  LCD_Clear(WHITE);
  TIM3->PSC = 0;
  TIM3->CCR1 = 0;
  
  while (1) {
    // TODO: implement
    
    // Front move
    uint32_t fr_sensor = getDistance(13, 14); //  trig / echo
    uint32_t rt_sensor = getDistance(1, 3);
    uint32_t lt_sensor = getDistance(2, 4);
    
    LCD_ShowNum(40, 40, fr_sensor, 10 , RED, WHITE);
    LCD_ShowNum(40, 60, rt_sensor, 10 , RED, WHITE);
    LCD_ShowNum(40, 80, lt_sensor, 10 , RED, WHITE);
    LCD_ShowNum(40, 100, drive_mode, 10 , RED, WHITE);
    LCD_ShowNum(40, 120, now_go, 10 , RED, WHITE);
    
    if(stuck == 1) {
      setStop();
      alert_buzzer();
      stuck = 0;
    }
    
    if(drive_mode == 0) {
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
    }
  }
  
  return 0;
}


