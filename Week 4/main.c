

#define RCC_APB2ENR *((volatile unsigned int *)0x40021018)

#define GPIOA_CRL *(volatile unsigned int *)0x40010800 //switch Configuration Regiester Low
#define GPIOA_IDR *(volatile unsigned int *)0x40010808 //switch Input Data Register

#define GPIOB_CRH *(volatile unsigned int *)0x40010C04 //switch Configuration Regiester high
#define GPIOB_IDR *(volatile unsigned int *)0x40010C08 //switch Input Data Register

#define GPIOC_CRL *(volatile unsigned int *)0x40011000 //switch Configuration Regiester Low
#define GPIOC_CRH *(volatile unsigned int *)0x40011004 //switchConfiguration Regiester Low
#define GPIOC_IDR *(volatile unsigned int *)0x40011008 //switch Input Data Register

#define GPIOD_CRL *(volatile unsigned int *)0x40011400 //LED Configuration Regiester Low
#define GPIOD_BSRR *(volatile unsigned int *)0x40011410 //LED Bit Set/Reset Register

void pd2off() { //only pd 2 led on
  GPIOD_BSRR |= 0x00000004;  
}
void pd3off() { //only pd 3 led on
  GPIOD_BSRR |= 0x00000008; 
}



void pd2on() { //only pd 2 led off
  GPIOD_BSRR |= 0x00040000;  
}
void pd3on() { //only pd 3 led off
  GPIOD_BSRR |= 0x00080000; 
}

void delay(){
  int i;
  for(i = 0; i<10000000;i++){
  
  }
}


int main(void){
  
  RCC_APB2ENR |=  0x0000003C;
  
  GPIOA_CRL &= 0xFFFFFFF0; // reset
  GPIOA_CRL |= 0x00000008; // switch PA 0 input mode 
  GPIOA_IDR |= 0x00000000; // switch Input Data Register reset 
  
  GPIOB_CRH &= 0xFFFFF0FF ; // reset
  GPIOB_CRH |= 0x00000800; // switch PB 10 input mode 
  GPIOB_IDR |= 0x00000000; // switch Input Data Register reset 
 
  GPIOC_CRL &= 0xFF0FFFFF; // reset
  GPIOC_CRL |= 0x00800000; // switch PC 4 input mode 
  GPIOC_IDR |= 0x00000000; // switch Input Data Register reset 
  
  GPIOC_CRH &= 0xFFF0FFFF; // reset
  GPIOC_CRH |= 0x00080000; // switch PC 13 input mode 
  GPIOC_IDR |= 0x00000000; // switch Input Data Register reset
  
  GPIOD_CRL &= 0x0FF000FF; // reset
  GPIOD_CRL |= 0x30033300; // led PD 2,3,4,7 output push-pull
  
  while(1){
    if(~GPIOC_IDR & 0x10){    //0100
      pd2on();
      pd3off();// switch 1¹ø ´©¸£¸é PD2, PD3 °¡ ÄÑÁü
    }
    else if(~GPIOB_IDR & 0x400){  //10 0000 0000 
      pd2off();
      pd3on(); //switch 2¹ø ´©¸£¸é PD2, PD3 LED °¡ ²¨Áü
    }
    else if(~GPIOC_IDR & 0x2000) {  //10 0000 0000 0000
      pd2on();
      pd3off();
      
      delay();
      
      pd2off();
      pd3on();
     
      delay();
      
      pd3off();
      pd2off();
      
      delay();
    }
    else if(~GPIOA_IDR & 0x1){  //0000 0001
      pd2off();
      pd3off();// switch 4 ´©¸£¸é PD3, PD7 LED °¡ ²¨Áü
    }
  }
  
  return 0;
  
}