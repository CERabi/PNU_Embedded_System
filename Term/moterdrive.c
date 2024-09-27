#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"

void RCC_Configure(void) {
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
}

void GPIO_Configure(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Motordriver 1 pin(d2, d3, d4, d7) setting*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Motordriver 2 pin(d8, d9, d10, d12) setting*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/* Motor */
void motorfl(int n) {
  // Uses d2, d3
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
  }
}
void motorfr(int n) {
  // Uses d4, d7
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    GPIO_SetBits(GPIOD, GPIO_Pin_7);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_SetBits(GPIOD, GPIO_Pin_4);
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_SetBits(GPIOD, GPIO_Pin_4);
    GPIO_SetBits(GPIOD, GPIO_Pin_7);
  }
}
void motorbl(int n) {
  // Uses d8, d9
  // n == 0 front move high/low
  if(n == 0) {
    GPIO_ResetBits(GPIOD, GPIO_Pin_8);
    GPIO_SetBits(GPIOD, GPIO_Pin_9);
  }
  // n == 1 back move low/high
  if(n == 1) {
    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    GPIO_ResetBits(GPIOD, GPIO_Pin_9);
  }
  // n == 2 stop low/low
  if(n == 2) {
    GPIO_SetBits(GPIOD, GPIO_Pin_8);
    GPIO_SetBits(GPIOD, GPIO_Pin_9);
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
    GPIO_SetBits(GPIOE, GPIO_Pin_10);
    GPIO_SetBits(GPIOE, GPIO_Pin_12);
  }
}

int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();

  while (1) {
    // TODO: implement
    // Front move
  }
  return 0;
}
