#include <msp430.h> 

// Global variables
unsigned int ADC_value = 0;

int main(void) {

	// Stop watchdog timer for the debugging purpose
	WDTCTL = WDTPW + WDTHOLD;											// Stop watchdog timer
	ADC10CTL1 = INCH_1 + ADC10DIV_3 + ADC10SSEL_3;         							// Channel 1, ADC10CLK/3
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE; 				// Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
	ADC10AE0 |= 0x02;                         							// ADC input enable P1.1

	//[frequency set up]
	//graph p275
	DCOCTL = 0;             											// Select lowest DCOx and MODx
	BCSCTL1 = CALBC1_1MHZ;  											// Set range, divisions etc.
	DCOCTL = CALDCO_1MHZ; 												/// CALDCO_1MHZ -macro for simplifying getting 1Mhz frequency,
	//automatically setting the registers DCO (3bit) (p278) and MOD (5 bit)
	BCSCTL2 &= ~SELS; 													// clear bit responsible for sels (the value of mux) 0000 1000

	/*** GPIO Set-Up ***/
	P1DIR &= ~BIT1;														// p 1.1 as input of the potentiometer
	P1DIR |= BIT3;														// direction out 1.3 Potentiometer
	P1OUT &=~BIT3;														// grounding the port 1.3 (Potentiometer GND)
	//P1IN &= ~0x08;
	P1DIR |= BIT6;														// P1.6 to output; selecting the direction of output
	P1SEL |= BIT6;														// P1.6 to TA0.1 ; allow the individual GPIO pin to be associated with the internal peripheral module functions


	/*** Timer0_A Set-Up ***/
	TA0CCR0 |= 1000;													// Counter value
	TA0CCR1 |= 500;
	TA0CCTL0 |= OUTMOD_3 + CCIE + CCIFG + CM_1;								// Enable Timer0_A interrupts ; for interrupts
	TA0CCTL1 |= OUTMOD_3 + CCIFG;										// for PWM
	TA0CTL |= TASSEL_2 + MC_1 + ID_1;									// assigning many values, we set ID_1 divider (division of 1st degree), TASSEL_2 - SMCLK p370, MC_1 count up to ccr0

	_BIS_SR(LPM0_bits + GIE);											// Enter Low power mode 0 with interrupts enabled

}

#pragma vector=TIMER0_A0_VECTOR 										// Timer0 A0 interrupt service routine
__interrupt void Timer0_A1(void) {

	ADC10CTL0 |= ENC + ADC10SC;											// Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);													// Low Power Mode 0 with interrupts enabled
	TA0CCR1 = ADC10MEM;													// Assigns the value held in ADC10MEM to the register called TA0CCR1
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
	__bic_SR_register_on_exit(CPUOFF);        							// Return to active mode }
}

