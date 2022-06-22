#include "stm32f4xx.h"                  // Device header

/*Measured the distance from an ultrasonic sensor using input capture method.
Trig Pin: PD13
Echo Pin: PC6
*/
void Clocks_Init(void)
{
	while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY);  	//Wait till HSI clock is ready
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN;
}

void configureGPIOD(void)
{
	GPIOD->MODER |= GPIO_MODER_MODE13_1;
	GPIOD->AFR[1] |= GPIO_AFRH_AFSEL13_1; //Select TIM4 alternate function
}

void configureGPIOC(void)
{
	GPIOC->MODER |= GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
	GPIOC->AFR[0] |= GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL7_1;
}

void configureTIM4(void)
{
	//PWM output mode config for channel 2
	TIM4->PSC = 10-1;
	TIM4->ARR = 32000-1; 
	TIM4-> CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE; //PWM MODE 1 select, auto reload preload enable
	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CCR2 |= 24; //15 percent duty cycle
	TIM4->CCER |= TIM_CCER_CC2E;
	TIM4->CR1 |= TIM_CR1_CEN;
	
}

void configureTIM3(void)
{
	TIM3->PSC = 1600-1;
	//For Channel 1
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;	//IC1 is mapped to TI1
	TIM3->CCER &= ~TIM_CCER_CC2P;
	TIM3->CCER &= ~TIM_CCER_CC1NP;
	
	//For Channel 2
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_1; //IC2 is mapped to TI1
	TIM3->CCER |= TIM_CCER_CC2P;
	TIM3->CCER &= ~TIM_CCER_CC2NP;
	TIM3->CCMR1 |= TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1;
	
	TIM3->SMCR |= TIM_SMCR_TS_0 | TIM_SMCR_TS_2;
	TIM3->SMCR |= TIM_SMCR_SMS_2;
	TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM3->CR1 |= TIM_CR1_CEN;
	
}

uint16_t period;
uint16_t dutyCycle;
uint16_t distanceCM;

int main(void)
{
	Clocks_Init();
	configureGPIOD();
	configureGPIOC();
	configureTIM4();
	configureTIM3();
	
	while(1)
	{
		if((TIM3->SR & TIM_SR_CC1IF) == TIM_SR_CC1IF )
		{
			period = TIM3->CCR1;
		}
		if((TIM3->SR & TIM_SR_CC2IF) == TIM_SR_CC2IF)
		{
			dutyCycle = TIM3->CCR2;
		}
		distanceCM = (dutyCycle * 100) / 58; //distance(cm) = dutyCycle(microseconds)/58
	}
}
