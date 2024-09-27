#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"

#define startX 20
#define startY 20
#define blank 20


int color[12] =
{WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BR
RED,GRAY};

void RCC_Configure(void)
{  
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	
	/* USART1, USART2 TX/RX port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
   
   	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

    //A0 : CdS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_Configure(void) {
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = 
    ADC_InitStructure.ADC_ScanConvMode = 
    ADC_InitStructure.ADC_ContinuousConvMode = 
    ADC_InitStructure.ADC_ExternalTrigConv = 
    ADC_InitStructure.ADC_DataAlign = 
    ADC_InitStructure.ADC_NbrOfChannel = 
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig

    ADC_ITConfig

    ADC_Cmd

    ADC_ResetCalibration

    While(ADC_GetResetCalibrationStatus)

    ADC_StartCalibration

    While(ADC_GetCalibrationStatus)

    ADC_SoftwareStartConvCmd
 
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
	
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_EnableIRQ();
    NVIC_InitStructure.NVIC_IRQChannel = ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



int main() {
    // LCD 관련 설정은 LCD_Init에 구현되어 있으므로 여기서 할 필요 없음
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    ADC_Configure();
    NVIC_Configure();
    // ------------------------------------
    LCD_Init();
    Touch_Configuration();
    Touch_Adjust();
    LCD_Clear(WHITE);

    int posX, posY;
    while(1){
        // TODO : LCD 값 출력 및 터치 좌표 읽기
        //LCD부터!
        LCD_ShowString(startX, startY, "MON_Team10", GRAY, WHITE);
        
        //터치 좌표!
        Touch_GetXY(&posX, &posY, 1);
        Convert_Pos(posX, posY, &posX, &posY);

        LCD_ShowNum(startX, (startY + blank), (u32)posX, GRAY, WHITE);
        LCD_ShowNum(startX, (startY + (blank * 2)), (u32)posY, GRAY, WHITE);
    }
}