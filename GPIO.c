#include <stm32l4xx.h>

typedef enum GPIO_Mode { GPIO_INPUT, GPIO_OUTPUT, GPIO_ALTERNATIVE, GPIO_ANALOG,
							GPIO_INPUT_PULLUP, GPIO_INPUT_PULLDOWN = 0x8 } GPIO_Mode;

void ClockInit(void);
void GPIO_Init(GPIO_TypeDef *port, unsigned int pin, GPIO_Mode mode);

int main(void)
{
	ClockInit();
	GPIO_Init(GPIOA, 0, GPIO_INPUT_PULLDOWN);
	GPIO_Init(GPIOA, 1, GPIO_INPUT_PULLDOWN);
	GPIO_Init(GPIOA, 2, GPIO_INPUT_PULLDOWN);
	GPIO_Init(GPIOA, 3, GPIO_INPUT_PULLDOWN);
	GPIO_Init(GPIOA, 5, GPIO_INPUT_PULLDOWN);
	GPIO_Init(GPIOA, 2, GPIO_OUTPUT);
	GPIO_Init(GPIOA, 8, GPIO_OUTPUT);
	
	while (1)
	{
		if (GPIOA->IDR & GPIO_IDR_ID1) GPIOB->BSRR = GPIO_BSRR_BS2;
		else if (GPIOA->IDR & GPIO_IDR_ID2) GPIOB->BSRR = GPIO_BSRR_BS8;
		else if (GPIOA->IDR & GPIO_IDR_ID3) GPIOB->BSRR = GPIO_BSRR_BS2;
		else if (GPIOA->IDR & GPIO_IDR_ID5) GPIOB->BSRR = GPIO_BSRR_BS8;
		else if (GPIOA->IDR & GPIO_IDR_ID0)
		{
			GPIOB->BSRR = GPIO_BSRR_BS2;
			GPIOB->BSRR = GPIO_BSRR_BR8;
		}
	}
}

void ClockInit(void)
{
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	RCC->PLLCFGR = RCC_PLLCFGR_PLLREN  |  (20 << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLM_0 | RCC_PLLCFGR_PLLSRC_HSI;
	
	RCC->CR |= RCC_CR_PLLON | RCC_CR_HSION;
	
	while (!((FLASH->ACR & FLASH_ACR_LATENCY_4WS) && (RCC->CR & RCC_CR_PLLRDY)
		&& (RCC->CR& RCC_CR_HSIRDY)));
	
	RCC->CFGR = RCC_CFGR_SW_PLL;
	RCC->CR &= ~RCC_CR_MSION;
}

void GPIO_Init(GPIO_TypeDef *port, unsigned int pin, GPIO_Mode mode)
{
	unsigned int modeIn32Bit = ((mode & 3) << (2 * pin));	
	unsigned int pullUpDown = ((mode >> 2) << (2 * pin));
	
	RCC->AHB2ENR |= (1 << (((unsigned int) port - GPIOA_BASE) >> 10));
	
	port->MODER |= modeIn32Bit;
	port->MODER &= (modeIn32Bit | ~(3 << (2 * pin)));
	
	port->PUPDR |= pullUpDown;
	port->PUPDR &= (pullUpDown | ~(3 << (2 * pin)));
}	
