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
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

uint16_t prescale;
int led_status = -1;
int PWM = 1500;
void TIM2_Configure(void){
   prescale = (uint16_t) (SystemCoreClock / 10000);
 
   TIM_TimeBaseStructure.TIM_Period = 10000; 
   TIM_TimeBaseStructure.TIM_Prescaler = prescale;
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   TIM_Cmd(TIM2, ENABLE);
   TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}


void TIM3_Configure(void) {
   prescale = (uint16_t) (SystemCoreClock / 1000000);
   
   TIM_TimeBaseStructure.TIM_Period = 20000-1; 
   TIM_TimeBaseStructure.TIM_Prescaler = prescale-1;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = 1500; // us
   TIM_OC3Init(TIM3, &TIM_OCInitStructure);
   
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
   TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
   TIM_ARRPreloadConfig(TIM3, ENABLE);
   TIM_Cmd(TIM3, ENABLE);
}

void RCC_Configure(void)
{  
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
        
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
       
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
	
    // TODO: fill the arg you want
   
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
int count = 0;
     
void LED1_ON(void){
  GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

void LED1_OFF(void){
  GPIO_ResetBits(GPIOD, GPIO_Pin_2);
}

void LED2_ON(void){
  GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

void LED2_OFF(void){
  GPIO_ResetBits(GPIOD, GPIO_Pin_3);
}

void motor_left(void){
 PWM = PWM + 100;
 if(PWM > 1500)
   PWM = 0;
 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
 TIM_OCInitStructure.TIM_Pulse = PWM +700 ; // us
 
 TIM_OC3Init(TIM3, &TIM_OCInitStructure);
}

void motor_right(void){
 PWM = PWM - 100;
 if(PWM <500)
   PWM = 1500;
 
 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
 TIM_OCInitStructure.TIM_Pulse = PWM+700 ; // us
 
 TIM_OC3Init(TIM3, &TIM_OCInitStructure);
}

void TIM2_IRQHandler(void){
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET){
    if(led_status == 1) {
      if (count%2==0){
        LED1_ON();
        motor_right();
      }
      else {
        LED1_OFF();
        motor_right();
      }
      if(count==0) LED2_ON();
      if(count==5) LED2_OFF();
      
      count++;
      count = count % 10;      
    }
    else {
      motor_left();
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}


void TIM3_IRQHandler(void){
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET){
    if(led_status == 1) {
      if (count%2==0)LED1_ON();
      else LED1_OFF();
      if(count==0) LED2_ON();
      if(count==5) LED2_OFF();
      count++;
      count = count % 10;      
    }
      
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}


int main() {
    // LCD 관련 설정은 LCD_Init에 구현되어 있으므로 여기서 할 필요 없음
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    TIM2_Configure();
    TIM3_Configure();
    NVIC_Configure();
    // ------------------------------------
    LCD_Init();
    Touch_Configuration();
    LED1_ON();
    LED2_ON();
    Touch_Adjust();
    LCD_Clear(WHITE);
    
    
    uint16_t posX, posY;
    LCD_ShowString(40, 40, "TEAM10", BLACK, WHITE); //팀명 출력
    
    LCD_DrawRectangle(40,80,80,120);
    LCD_ShowString(40, 100, "  BUT", RED, WHITE); //버튼 그리기
    LCD_ShowString(40, 60, "OFF", RED, WHITE);
        
    while(1){
        Touch_GetXY(&posX, &posY, 1); //터치 좌표 얻기
        Convert_Pos(posX, posY, &posX, &posY); // 변환
        
        if(led_status==1) {
          LCD_ShowString(40, 60, "OFF", RED, WHITE);
          LCD_ShowNum(40,180,count,2,RED, WHITE);
          LED1_ON();
          LED2_ON();
        }
        else {
          LCD_ShowString(40, 60, "ON ", RED, WHITE);
          LCD_ShowNum(40,180,count,2,RED, WHITE);
        }
        
        if(40<posX&&80>posX&&80<posY&&120>posY){
          led_status = -1* led_status;
        }
        
        

    }
}