#include <msp430.h> 
#include <math.h>

int numpeople = 0;
/**
 * ADC
 */


/* Set the ADC */
void setADC()
{
    // ADC Setup
    // Configure ADC A0 (pin 6.0)
    P6DIR &= ~BIT0;
    P6SEL |= BIT0;
    
    P6DIR &= ~BIT1;
    P6SEL |= BIT1;

    ADC12CTL2 = ADC12RES_2;     // 12-bit conversion
    ADC12CTL1 = ADC12SHP;       // Use sampling timer, set mode

    ADC12CTL0 = ADC12SHT1_15 | ADC12SHT0_15 | ADC12MSC | ADC12ON | ADC12TOVIE | ADC12ENC | ADC12SC;
    // Turn on ADC12, set sampling time
    // set multiple sample conversion

    ADC12IE = ADC12IE0;                         // Enable interrupt
    ADC12IFG &= ~ADC12IFG0;                     // Clear flag

}



/* Initialize UART */
void setUart()
{
        P4SEL |= BIT5 | BIT4;   // UART TX, UART RX
        UCA1CTL1 |=  UCSWRST;   // Resets state machine
        UCA1CTL1 |=  UCSSEL_1;  // ACLK
        UCA1BR0   =  3;         // 9600 Baud Rate
        UCA1BR1   =  0;         // 9600 Baud Rate
        UCA1MCTL |= UCBRS_3 | UCBRF_0;
        UCA1CTL1 &= ~UCSWRST;   // Initializes the state machine
        UCA1IE |= UCTXIE;       // Enables USCI_A1 TX Interrupt
        UCA1IE   |=  UCRXIE;    // Enables USCI_A1 RX Interrupt   
}


#pragma vector = ADC12_VECTOR
__interrupt void newADC(void)


switch(ADC12IV)
{
  case  6: {
      
     unsigned int ADC1 = ADC12MEM0; // ADC from the first IR sensor
     unsigned int ADC2 = ADC12MEM1; // ADC from the second IR sensor
     
     float volts = ADC1*(5/4095); // voltage value from the sensor * 5/4095;
     int distance = 13*pow(volts , -1); //calculate the distance in CM
     delay(2000);
     
 //  int distanceCM = 60.374 * pow(map(ir_val[NB_SAMPLE / 2], 0, 4095, 0, 5000)/1000.0, -1.16);
     
    if (ADC1 & (DistanceCM <100cm)) {
        if ( ADC2 ) {
 
        }
        
                numpeople ++
                       }    
        }
    if (ADC2 (DistanceCM <100cm) ) {
        if ( ADC1) {
        
                numpeople --
                       }    
        }
   return numpeople;

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	return 0;
}
