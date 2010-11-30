
#include "isc.h"

//TODO: implement ISR macro attributes.

#ifdef ADC_ISC

	ISC* ADC_ISC_ptr = ADC_ISC;

	ISR(ADC_vect)
	{
		ADC_ISC_ptr->isr();
	}

#endif

#ifdef ANALOG_COMP_0_ISC

	ISC* ANALOG_COMP_0_ISC_ptr = ANALOG_COMP_0_ISC;

	ISR(ANALOG_COMP_0_vect)
	{
		ANALOG_COMP_0_ISC_ptr->isr();
	}

#endif

#ifdef ANALOG_COMP_1_ISC

	ISC* ANALOG_COMP_1_ISC_ptr = ANALOG_COMP_1_ISC;

	ISR(ANALOG_COMP_1_vect)
	{
		ANALOG_COMP_1_ISC_ptr->isr();
	}

#endif

#ifdef ANALOG_COMP_2_ISC

	ISC* ANALOG_COMP_2_ISC_ptr = ANALOG_COMP_2_ISC;

	ISR(ANALOG_COMP_2_vect)
	{
		ANALOG_COMP_2_ISC_ptr->isr();
	}

#endif

#ifdef ANALOG_COMP_ISC

	ISC* ANALOG_COMP_ISC_ptr = ANALOG_COMP_ISC;

	ISR(ANALOG_COMP_vect)
	{
		ANALOG_COMP_ISC_ptr->isr();
	}

#endif

#ifdef ANA_COMP_ISC

	ISC* ANA_COMP_ISC_ptr = ANA_COMP_ISC;

	ISR(ANA_COMP_vect)
	{
		ANA_COMP_ISC_ptr->isr();
	}

#endif

#ifdef CANIT_ISC

	ISC* CANIT_ISC_ptr = CANIT_ISC;

	ISR(CANIT_vect)
	{
		CANIT_ISC_ptr->isr();
	}

#endif

#ifdef EEPROM_READY_ISC

	ISC* EEPROM_READY_ISC_ptr = EEPROM_READY_ISC;

	ISR(EEPROM_READY_vect)
	{
		EEPROM_READY_ISC_ptr->isr();
	}

#endif

#ifdef EE_RDY_ISC

	ISC* EE_RDY_ISC_ptr = EE_RDY_ISC;

	ISR(EE_RDY_vect)
	{
		EE_RDY_ISC_ptr->isr();
	}

#endif

#ifdef EE_READY_ISC

	ISC* EE_READY_ISC_ptr = EE_READY_ISC;

	ISR(EE_READY_vect)
	{
		EE_READY_ISC_ptr->isr();
	}

#endif

#ifdef EXT_INT0_ISC

	ISC* EXT_INT0_ISC_ptr = EXT_INT0_ISC;

	ISR(EXT_INT0_vect)
	{
		EXT_INT0_ISC_ptr->isr();
	}

#endif

#ifdef INT0_ISC

	ISC* INT0_ISC_ptr = INT0_ISC;

	ISR(INT0_vect)
	{
		INT0_ISC_ptr->isr();
	}

#endif

#ifdef INT1_ISC

	ISC* INT1_ISC_ptr = INT1_ISC;

	ISR(INT1_vect)
	{
		INT1_ISC_ptr->isr();
	}

#endif

#ifdef INT2_ISC

	ISC* INT2_ISC_ptr = INT2_ISC;

	ISR(INT2_vect)
	{
		INT2_ISC_ptr->isr();
	}

#endif

#ifdef INT3_ISC

	ISC* INT3_ISC_ptr = INT3_ISC;

	ISR(INT3_vect)
	{
		INT3_ISC_ptr->isr();
	}

#endif

#ifdef INT4_ISC

	ISC* INT4_ISC_ptr = INT4_ISC;

	ISR(INT4_vect)
	{
		INT4_ISC_ptr->isr();
	}

#endif

#ifdef INT5_ISC

	ISC* INT5_ISC_ptr = INT5_ISC;

	ISR(INT5_vect)
	{
		INT5_ISC_ptr->isr();
	}

#endif

#ifdef INT6_ISC

	ISC* INT6_ISC_ptr = INT6_ISC;

	ISR(INT6_vect)
	{
		INT6_ISC_ptr->isr();
	}

#endif

#ifdef INT7_ISC

	ISC* INT7_ISC_ptr = INT7_ISC;

	ISR(INT7_vect)
	{
		INT7_ISC_ptr->isr();
	}

#endif

#ifdef IO_PINS_ISC

	ISC* IO_PINS_ISC_ptr = IO_PINS_ISC;

	ISR(IO_PINS_vect)
	{
		IO_PINS_ISC_ptr->isr();
	}

#endif

#ifdef LCD_ISC

	ISC* LCD_ISC_ptr = LCD_ISC;

	ISR(LCD_vect)
	{
		LCD_ISC_ptr->isr();
	}

#endif


#ifdef LOWLEVEL_IO_PINS_ISC

	ISC* LOWLEVEL_IO_PINS_ISC_ptr = LOWLEVEL_IO_PINS_ISC;

	ISR(LOWLEVEL_IO_PINS_vect)
	{
		LOWLEVEL_IO_PINS_ISC_ptr->isr();
	}

#endif

#ifdef OVRIT_ISC

	ISC* OVRIT_ISC_ptr = OVRIT_ISC;

	ISR(OVRIT_vect)
	{
		OVRIT_ISC_ptr->isr();
	}

#endif

#ifdef PCINT0_ISC

	ISC* PCINT0_ISC_ptr = PCINT0_ISC;

	ISR(PCINT0_vect)
	{
		PCINT0_ISC_ptr->isr();
	}

#endif

#ifdef PCINT1_ISC

	ISC* PCINT1_ISC_ptr = PCINT1_ISC;

	ISR(PCINT1_vect)
	{
		PCINT1_ISC_ptr->isr();
	}

#endif

#ifdef PCINT2_ISC

	ISC* PCINT2_ISC_ptr = PCINT2_ISC;

	ISR(PCINT2_vect)
	{
		PCINT2_ISC_ptr->isr();
	}

#endif

#ifdef PCINT3_ISC

	ISC* PCINT3_ISC_ptr = PCINT3_ISC;

	ISR(PCINT3_vect)
	{
		PCINT3_ISC_ptr->isr();
	}

#endif

#ifdef PCINT_ISC

	ISC* PCINT_ISC_ptr = PCINT_ISC;

	ISR(PCINT_vect)
	{
		PCINT_ISC_ptr->isr();
	}

#endif

#ifdef PSC0_CAPT_ISC

	ISC* PSC0_CAPT_ISC_ptr = PSC0_CAPT_ISC;

	ISR(PSC0_CAPT_vect)
	{
		PSC0_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef PSC0_EC_ISC

	ISC* PSC0_EC_ISC_ptr = PSC0_EC_ISC;

	ISR(PSC0_EC_vect)
	{
		PSC0_EC_ISC_ptr->isr();
	}

#endif

#ifdef PSC1_CAPT_ISC

	ISC* PSC1_CAPT_ISC_ptr = PSC1_CAPT_ISC;

	ISR(PSC1_CAPT_vect)
	{
		PSC1_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef PSC1_EC_ISC

	ISC* PSC1_EC_ISC_ptr = PSC1_EC_ISC;

	ISR(PSC1_EC_vect)
	{
		PSC1_EC_ISC_ptr->isr();
	}

#endif

#ifdef PSC2_CAPT_ISC

	ISC* PSC2_CAPT_ISC_ptr = PSC2_CAPT_ISC;

	ISR(PSC2_CAPT_vect)
	{
		PSC2_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef PSC2_EC_ISC

	ISC* PSC2_EC_ISC_ptr = PSC2_EC_ISC;

	ISR(PSC2_EC_vect)
	{
		PSC2_EC_ISC_ptr->isr();
	}

#endif

#ifdef SPI_STC_ISC

	ISC* SPI_STC_ISC_ptr = SPI_STC_ISC;

	ISR(SPI_STC_vect)
	{
		SPI_STC_ISC_ptr->isr();
	}

#endif

#ifdef SPM_RDY_ISC

	ISC* SPM_RDY_ISC_ptr = SPM_RDY_ISC;

	ISR(SPM_RDY_vect)
	{
		SPM_RDY_ISC_ptr->isr();
	}

#endif

#ifdef SPM_READY_ISC

	ISC* SPM_READY_ISC_ptr = SPM_READY_ISC;

	ISR(SPM_READY_vect)
	{
		SPM_READY_ISC_ptr->isr();
	}

#endif

#ifdef TIM0_COMPA_ISC

	ISC* TIM0_COMPA_ISC_ptr = TIM0_COMPA_ISC;

	ISR(TIM0_COMPA_vect)
	{
		TIM0_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIM0_COMPB_ISC

	ISC* TIM0_COMPB_ISC_ptr = TIM0_COMPB_ISC;

	ISR(TIM0_COMPB_vect)
	{
		TIM0_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIM0_OVF_ISC

	ISC* TIM0_OVF_ISC_ptr = TIM0_OVF_ISC;

	ISR(TIM0_OVF_vect)
	{
		TIM0_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TIM0_CAPT_ISC

	ISC* TIM0_CAPT_ISC_ptr = TIM0_CAPT_ISC;

	ISR(TIM0_CAPT_vect)
	{
		TIM0_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIM1_COMPA_ISC

	ISC* TIM1_COMPA_ISC_ptr = TIM1_COMPA_ISC;

	ISR(TIM1_COMPA_vect)
	{
		TIM1_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIM1_COMPB_ISC

	ISC* TIM1_COMPB_ISC_ptr = TIM1_COMPB_ISC;

	ISR(TIM1_COMPB_vect)
	{
		TIM1_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIM1_OVF_ISC

	ISC* TIM1_OVF_ISC_ptr = TIM1_OVF_ISC;

	ISR(TIM1_OVF_vect)
	{
		TIM1_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TIM1_CAPT_ISC

	ISC* TIM1_CAPT_ISC_ptr = TIM1_CAPT_ISC;

	ISR(TIM1_CAPT_vect)
	{
		TIM1_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIMER0_COMPA_ISC

	ISC* TIMER0_COMPA_ISC_ptr = TIMER0_COMPA_ISC;

	ISR(TIMER0_COMPA_vect)
	{
		TIMER0_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER0_COMPB_ISC

	ISC* TIMER0_COMPB_ISC_ptr = TIMER0_COMPB_ISC;

	ISR(TIMER0_COMPB_vect)
	{
		TIMER0_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER0_COMP_A_ISC

	ISC* TIMER0_COMP_A_ISC_ptr = TIMER0_COMP_A_ISC;

	ISR(TIMER0_COMP_A_vect)
	{
		TIMER0_COMP_A_ISC_ptr->isr();
	}

#endif


#ifdef TIMER0_COMP_ISC

	ISC* TIMER0_COMP_ISC_ptr = TIMER0_COMP_ISC;

	ISR(TIMER0_COMP_vect)
	{
		TIMER0_COMP_ISC_ptr->isr();
	}

#endif

#ifdef TIMER0_OVF0_ISC

	ISC* TIMER0_OVF0_ISC_ptr = TIMER0_OVF0_ISC;

	ISR(TIMER0_OVF0_vect)
	{
		TIMER0_OVF0_ISC_ptr->isr();
	}

#endif

#ifdef TIMER0_OVF_ISC

	ISC* TIMER0_OVF_ISC_ptr = TIMER0_OVF_ISC;

	ISR(TIMER0_OVF_vect)
	{
		TIMER0_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_CAPT1_ISC

	ISC* TIMER1_CAPT1_ISC_ptr = TIMER1_CAPT1_ISC;

	ISR(TIMER1_CAPT1_vect)
	{
		TIMER1_CAPT1_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_CAPT_ISC

	ISC* TIMER1_CAPT_ISC_ptr = TIMER1_CAPT_ISC;

	ISR(TIMER1_CAPT_vect)
	{
		TIMER1_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_CMPA_ISC

	ISC* TIMER1_CMPA_ISC_ptr = TIMER1_CMPA_ISC;

	ISR(TIMER1_CMPA_vect)
	{
		TIMER1_CMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_CMPB_ISC

	ISC* TIMER1_CMPB_ISC_ptr = TIMER1_CMPB_ISC;

	ISR(TIMER1_CMPB_vect)
	{
		TIMER1_CMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMP1_ISC

	ISC* TIMER1_COMP1_ISC_ptr = TIMER1_COMP1_ISC;

	ISR(TIMER1_COMP1_vect)
	{
		TIMER1_COMP1_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMPA_ISC

	ISC* TIMER1_COMPA_ISC_ptr = TIMER1_COMPA_ISC;

	ISR(TIMER1_COMPA_vect)
	{
		TIMER1_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMPB_ISC

	ISC* TIMER1_COMPB_ISC_ptr = TIMER1_COMPB_ISC;

	ISR(TIMER1_COMPB_vect)
	{
		TIMER1_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMPC_ISC

	ISC* TIMER1_COMPC_ISC_ptr = TIMER1_COMPC_ISC;

	ISR(TIMER1_COMPC_vect)
	{
		TIMER1_COMPC_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMPD_ISC

	ISC* TIMER1_COMPD_ISC_ptr = TIMER1_COMPD_ISC;

	ISR(TIMER1_COMPD_vect)
	{
		TIMER1_COMPD_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_COMP_ISC

	ISC* TIMER1_COMP_ISC_ptr = TIMER1_COMP_ISC;

	ISR(TIMER1_COMP_vect)
	{
		TIMER1_COMP_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_OVF1_ISC

	ISC* TIMER1_OVF1_ISC_ptr = TIMER1_OVF1_ISC;

	ISR(TIMER1_OVF1_vect)
	{
		TIMER1_OVF1_ISC_ptr->isr();
	}

#endif

#ifdef TIMER1_OVF_ISC

	ISC* TIMER1_OVF_ISC_ptr = TIMER1_OVF_ISC;

	ISR(TIMER1_OVF_vect)
	{
		TIMER1_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TIMER2_COMPA_ISC

	ISC* TIMER2_COMPA_ISC_ptr = TIMER2_COMPA_ISC;

	ISR(TIMER2_COMPA_vect)
	{
		TIMER2_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER2_COMPB_ISC

	ISC* TIMER2_COMPB_ISC_ptr = TIMER2_COMPB_ISC;

	ISR(TIMER2_COMPB_vect)
	{
		TIMER2_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER2_CMOP_ISC

	ISC* TIMER2_CMOP_ISC_ptr = TIMER2_CMOP_ISC;

	ISR(TIMER2_CMOP_vect)
	{
		TIMER2_CMOP_ISC_ptr->isr();
	}

#endif

#ifdef TIMER2_OVF_ISC

	ISC* TIMER2_OVF_ISC_ptr = TIMER2_OVF_ISC;

	ISR(TIMER2_OVF_vect)
	{
		TIMER2_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TIMER3_CAPT_ISC

	ISC* TIMER3_CAPT_ISC_ptr = TIMER3_CAPT_ISC;

	ISR(TIMER3_CAPT_vect)
	{
		TIMER3_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIMER3_COMPA_ISC

	ISC* TIMER3_COMPA_ISC_ptr = TIMER3_COMPA_ISC;

	ISR(TIMER3_COMPA_vect)
	{
		TIMER3_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER3_COMPB_ISC

	ISC* TIMER3_COMPB_ISC_ptr = TIMER3_COMPB_ISC;

	ISR(TIMER3_COMPB_vect)
	{
		TIMER3_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER3_COMPC_ISC

	ISC* TIMER3_COMPC_ISC_ptr = TIMER3_COMPC_ISC;

	ISR(TIMER3_COMPC_vect)
	{
		TIMER3_COMPC_ISC_ptr->isr();
	}

#endif

#ifdef TIMER3_OVF_ISC

	ISC* TIMER3_OVF_ISC_ptr = TIMER3_OVF_ISC;

	ISR(TIMER3_OVF_vect)
	{
		TIMER3_OVF_ISC_ptr->isr();
	}

#endif


#ifdef TIMER4_CAPT_ISC

	ISC* TIMER4_CAPT_ISC_ptr = TIMER4_CAPT_ISC;

	ISR(TIMER4_CAPT_vect)
	{
		TIMER4_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIMER4_COMPA_ISC

	ISC* TIMER4_COMPA_ISC_ptr = TIMER4_COMPA_ISC;

	ISR(TIMER4_COMPA_vect)
	{
		TIMER4_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER4_COMPB_ISC

	ISC* TIMER4_COMPB_ISC_ptr = TIMER4_COMPB_ISC;

	ISR(TIMER4_COMPB_vect)
	{
		TIMER4_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER4_COMPC_ISC

	ISC* TIMER4_COMPC_ISC_ptr = TIMER4_COMPC_ISC;

	ISR(TIMER4_COMPC_vect)
	{
		TIMER4_COMPC_ISC_ptr->isr();
	}

#endif

#ifdef TIMER4_OVF_ISC

	ISC* TIMER4_OVF_ISC_ptr = TIMER4_OVF_ISC;

	ISR(TIMER4_OVF_vect)
	{
		TIMER4_OVF_ISC_ptr->isr();
	}

#endif


#ifdef TIMER5_CAPT_ISC

	ISC* TIMER5_CAPT_ISC_ptr = TIMER5_CAPT_ISC;

	ISR(TIMER5_CAPT_vect)
	{
		TIMER5_CAPT_ISC_ptr->isr();
	}

#endif

#ifdef TIMER5_COMPA_ISC

	ISC* TIMER5_COMPA_ISC_ptr = TIMER5_COMPA_ISC;

	ISR(TIMER5_COMPA_vect)
	{
		TIMER5_COMPA_ISC_ptr->isr();
	}

#endif

#ifdef TIMER5_COMPB_ISC

	ISC* TIMER5_COMPB_ISC_ptr = TIMER5_COMPB_ISC;

	ISR(TIMER5_COMPB_vect)
	{
		TIMER5_COMPB_ISC_ptr->isr();
	}

#endif

#ifdef TIMER5_COMPC_ISC

	ISC* TIMER5_COMPC_ISC_ptr = TIMER5_COMPC_ISC;

	ISR(TIMER5_COMPC_vect)
	{
		TIMER5_COMPC_ISC_ptr->isr();
	}

#endif

#ifdef TIMER5_OVF_ISC

	ISC* TIMER5_OVF_ISC_ptr = TIMER5_OVF_ISC;

	ISR(TIMER5_OVF_vect)
	{
		TIMER5_OVF_ISC_ptr->isr();
	}

#endif

#ifdef TWI_ISC

	ISC* TWI_ISC_ptr = TWI_ISC;

	ISR(TWI_vect)
	{
		TWI_ISC_ptr->isr();
	}

#endif

#ifdef TXDONE_ISC

	ISC* TXDONE_ISC_ptr = TXDONE_ISC;

	ISR(TXDONE_vect)
	{
		TXDONE_ISC_ptr->isr();
	}

#endif

#ifdef TXEMPTY_ISC

	ISC* TXEMPTY_ISC_ptr = TXEMPTY_ISC;

	ISR(TXEMPTY_vect)
	{
		TXEMPTY_ISC_ptr->isr();
	}

#endif

#ifdef UART0_RX_ISC

	ISC* UART0_RX_ISC_ptr = UART0_RX_ISC;

	ISR(UART0_RX_vect)
	{
		UART0_RX_ISC_ptr->isr();
	}

#endif

#ifdef UART0_TX_ISC

	ISC* UART0_TX_ISC_ptr = UART0_TX_ISC;

	ISR(UART0_TX_vect)
	{
		UART0_TX_ISC_ptr->isr();
	}

#endif

#ifdef UART0_UDRE_ISC

	ISC* UART0_UDRE_ISC_ptr = UART0_UDRE_ISC;

	ISR(UART0_UDRE_vect)
	{
		UART0_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef UART1_RX_ISC

	ISC* UART1_RX_ISC_ptr = UART1_RX_ISC;

	ISR(UART1_RX_vect)
	{
		UART1_RX_ISC_ptr->isr();
	}

#endif

#ifdef UART1_TX_ISC

	ISC* UART1_TX_ISC_ptr = UART1_TX_ISC;

	ISR(UART1_TX_vect)
	{
		UART1_TX_ISC_ptr->isr();
	}

#endif

#ifdef UART1_UDRE_ISC

	ISC* UART1_UDRE_ISC_ptr = UART1_UDRE_ISC;

	ISR(UART1_UDRE_vect)
	{
		UART1_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef UART_RX_ISC

	ISC* UART_RX_ISC_ptr = UART_RX_ISC;

	ISR(UART_RX_vect)
	{
		UART_RX_ISC_ptr->isr();
	}

#endif

#ifdef UART_TX_ISC

	ISC* UART_TX_ISC_ptr = UART_TX_ISC;

	ISR(UART_TX_vect)
	{
		UART_TX_ISC_ptr->isr();
	}

#endif

#ifdef UART_UDRE_ISC

	ISC* UART_UDRE_ISC_ptr = UART_UDRE_ISC;

	ISR(UART_UDRE_vect)
	{
		UART_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USART0_RXC_ISC

	ISC* USART0_RXC_ISC_ptr = USART0_RXC_ISC;

	ISR(USART0_RXC_vect)
	{
		USART0_RXC_ISC_ptr->isr();
	}

#endif

#ifdef USART0_RX_ISC

	ISC* USART0_RX_ISC_ptr = USART0_RX_ISC;

	ISR(USART0_RX_vect)
	{
		USART0_RX_ISC_ptr->isr();
	}

#endif

#ifdef USART0_TXC_ISC

	ISC* USART0_TXC_ISC_ptr = USART0_TXC_ISC;

	ISR(USART0_TXC_vect)
	{
		USART0_TXC_ISC_ptr->isr();
	}

#endif

#ifdef USART0_TX_ISC

	ISC* USART0_TX_ISC_ptr = USART0_TX_ISC;

	ISR(USART0_TX_vect)
	{
		USART0_TX_ISC_ptr->isr();
	}

#endif

#ifdef USART0_UDRE_ISC

	ISC* USART0_UDRE_ISC_ptr = USART0_UDRE_ISC;

	ISR(USART0_UDRE_vect)
	{
		USART0_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USART1_RXC_ISC

	ISC* USART1_RXC_ISC_ptr = USART1_RXC_ISC;

	ISR(USART1_RXC_vect)
	{
		USART1_RXC_ISC_ptr->isr();
	}

#endif

#ifdef USART1_RX_ISC

	ISC* USART1_RX_ISC_ptr = USART1_RX_ISC;

	ISR(USART1_RX_vect)
	{
		USART1_RX_ISC_ptr->isr();
	}

#endif

#ifdef USART1_TXC_ISC

	ISC* USART1_TXC_ISC_ptr = USART1_TXC_ISC;

	ISR(USART1_TXC_vect)
	{
		USART1_TXC_ISC_ptr->isr();
	}

#endif

#ifdef USART1_TX_ISC

	ISC* USART1_TX_ISC_ptr = USART1_TX_ISC;

	ISR(USART1_TX_vect)
	{
		USART1_TX_ISC_ptr->isr();
	}

#endif

#ifdef USART1_UDRE_ISC

	ISC* USART1_UDRE_ISC_ptr = USART1_UDRE_ISC;

	ISR(USART1_UDRE_vect)
	{
		USART1_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USART2_RX_ISC

	ISC* USART2_RX_ISC_ptr = USART2_RX_ISC;

	ISR(USART2_RX_vect)
	{
		USART2_RX_ISC_ptr->isr();
	}

#endif

#ifdef USART2_TX_ISC

	ISC* USART2_TX_ISC_ptr = USART2_TX_ISC;

	ISR(USART2_TX_vect)
	{
		USART2_TX_ISC_ptr->isr();
	}

#endif

#ifdef USART2_UDRE_ISC

	ISC* USART2_UDRE_ISC_ptr = USART2_UDRE_ISC;

	ISR(USART2_UDRE_vect)
	{
		USART2_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USART3_RX_ISC

	ISC* USART3_RX_ISC_ptr = USART3_RX_ISC;

	ISR(USART3_RX_vect)
	{
		USART3_RX_ISC_ptr->isr();
	}

#endif

#ifdef USART3_TX_ISC

	ISC* USART3_TX_ISC_ptr = USART3_TX_ISC;

	ISR(USART3_TX_vect)
	{
		USART3_TX_ISC_ptr->isr();
	}

#endif

#ifdef USART3_UDRE_ISC

	ISC* USART3_UDRE_ISC_ptr = USART3_UDRE_ISC;

	ISR(USART3_UDRE_vect)
	{
		USART3_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USART_RXC_ISC

	ISC* USART_RXC_ISC_ptr = USART_RXC_ISC;

	ISR(USART_RXC_vect)
	{
		USART_RXC_ISC_ptr->isr();
	}

#endif

#ifdef USART_RX_ISC

	ISC* USART_RX_ISC_ptr = USART_RX_ISC;

	ISR(USART_RX_vect)
	{
		USART_RX_ISC_ptr->isr();
	}

#endif

#ifdef USART_TXC_ISC

	ISC* USART_TXC_ISC_ptr = USART_TXC_ISC;

	ISR(USART_TXC_vect)
	{
		USART_TXC_ISC_ptr->isr();
	}

#endif

#ifdef USART_TX_ISC

	ISC* USART_TX_ISC_ptr = USART_TX_ISC;

	ISR(USART_TX_vect)
	{
		USART_TX_ISC_ptr->isr();
	}

#endif

#ifdef USART_UDRE_ISC

	ISC* USART_UDRE_ISC_ptr = USART_UDRE_ISC;

	ISR(USART_UDRE_vect)
	{
		USART_UDRE_ISC_ptr->isr();
	}

#endif

#ifdef USI_OVERFLOW_ISC

	ISC* USI_OVERFLOW_ISC_ptr = USI_OVERFLOW_ISC;

	ISR(USI_OVERFLOW_vect)
	{
		USI_OVERFLOW_ISC_ptr->isr();
	}

#endif


#ifdef USI_USI_OVF_ISC

	ISC* USI_USI_OVF_ISC_ptr = USI_USI_OVF_ISC;

	ISR(USI_USI_OVF_vect)
	{
		USI_USI_OVF_ISC_ptr->isr();
	}

#endif


#ifdef USI_START_ISC

	ISC* USI_START_ISC_ptr = USI_START_ISC;

	ISR(USI_START_vect)
	{
		USI_START_ISC_ptr->isr();
	}

#endif


#ifdef USI_STRT_ISC

	ISC* USI_STRT_ISC_ptr = USI_STRT_ISC;

	ISR(USI_STRT_vect)
	{
		USI_STRT_ISC_ptr->isr();
	}

#endif


#ifdef USI_STR_ISC

	ISC* USI_STR_ISC_ptr = USI_STR_ISC;

	ISR(USI_STR_vect)
	{
		USI_STR_ISC_ptr->isr();
	}

#endif


#ifdef WATCHDOG_ISC

	ISC* WATCHDOG_ISC_ptr = WATCHDOG_ISC;

	ISR(WATCHDOG_vect)
	{
		WATCHDOG_ISC_ptr->isr();
	}

#endif


#ifdef WDT_OVERFLOW_ISC

	ISC* WDT_OVERFLOW_ISC_ptr = WDT_OVERFLOW_ISC;

	ISR(WDT_OVERFLOW_vect)
	{
		WDT_OVERFLOW_ISC_ptr->isr();
	}

#endif


#ifdef WDT_ISC

	ISC* WDT_ISC_ptr = WDT_ISC;

	ISR(WDT_vect)
	{
		WDT_ISC_ptr->isr();
	}

#endif