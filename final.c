#include <msp430.h>
#include <intrinsics.h>
#include <stdint.h>

// Sonic sensor define parameters

#define TRIGGER_PIN BIT1   // P6.1
#define ECHO_PIN BIT3  // P1.3
#define LED_PIN BIT0   // P1.0
#define DISTANCE_THRESHOLD 60  // cm
#define MEASURE_INTERVAL 2048  // ~250 ms

// IR sensor parameters
int CM = 0;
int adc = 0;
int counter = 0;
int i = 0;



// Data Transmission
volatile unsigned char byte = 0;
volatile unsigned char data[8];


//Sonic sensor functions
uint16_t lastCount = 0;
uint32_t distance = 0;

void triggerMeasurement() {
    // Start trigger
    P6OUT |= TRIGGER_PIN;

    // Wait a small amount of time with trigger high, > 10us required (~10 clock cycles at 1MHz MCLK)
    __delay_cycles(10);

    // End trigger
    P6OUT &= ~TRIGGER_PIN;
}

void sonic(){

    // Configure trigger pin, low to start
    P6DIR |= TRIGGER_PIN;
    P6OUT &= ~TRIGGER_PIN;

    // Configure LED, off to start
    P1DIR |= LED_PIN;
    P1OUT &= ~LED_PIN;

    // Configure echo pin as capture input to TA0CCR2
    P1DIR &= ~ECHO_PIN;
    P1SEL |= ECHO_PIN;

    // Set up TA0 to capture in CCR2 on both edges from P1.3 (echo pin)
    TA0CCTL2 = CM_3 | CCIS_0 | SCS | CAP | CCIE;

    // Set up TA0 to compare CCR0 (measure interval)
    TA0CCR0 = MEASURE_INTERVAL;
    TA0CCTL0 = CCIE;

    // Set up TA0 with ACLK / 4 = 8192 Hz
    TA0CTL = TASSEL__ACLK | ID__4 | MC__CONTINUOUS | TACLR;



    for (;;)
    {
        triggerMeasurement();

        // Wait for echo start
        __low_power_mode_3();

        lastCount = TA0CCR2;

        // Wait for echo end
        __low_power_mode_3();

        distance = TA0CCR2 - lastCount;
        distance *= 34000;
        distance >>= 14;  // division by 16384 (2 ^ 14)

       if (distance <= DISTANCE_THRESHOLD)
        {
            // Turn on LED
            P1OUT |= LED_PIN;
        }
        else
        {
            // Turn off LED
            P1OUT &= ~LED_PIN;
        }

        // Wait for the next measure interval tick
        __low_power_mode_3();
    }

}



// Serial communication protocol

void setUart()
{
       P4SEL |= BIT5 | BIT4;   // UART TX, UART RX
       UCA1CTL1 |=  UCSWRST;   // Resets state machine
       UCA1CTL1 |=  UCSSEL_2;  // SMCLK
       UCA1BR0   =  9;         // 9600 Baud Rate
       UCA1BR1   =  0;         // 9600 Baud Rate
       UCA1MCTL |= UCBRS_3 | UCBRF_0;
       UCA1CTL1 &= ~UCSWRST;   // Initializes the state machine
       UCA1IE |= UCTXIE;       // Enables USCI_A1 TX Interrupt
       UCA1IE   |=  UCRXIE;    // Enables USCI_A1 RX Interrupt

}


void setADC()
{
    // ADC Setup
         // Configure ADC A0 (pin 6.0)
         P6DIR &= ~BIT0;
         P6SEL |= BIT0;

         ADC12CTL2 = ADC12RES_2;     // 12-bit conversion
         ADC12CTL1 = ADC12SHP;       // Use sampling timer, set mode

         ADC12CTL0 = ADC12SHT1_15 | ADC12SHT0_15 | ADC12MSC | ADC12ON | ADC12TOVIE | ADC12ENC | ADC12SC;
         // Turn on ADC12, set sampling time
         // set multiple sample conversion

         ADC12IE = ADC12IE0;                         // Enable interrupt
         ADC12IFG &= ~ADC12IFG0;                     // Clear flag
}


int main(void)
{
    P4DIR |= BIT7;     // P1.0 output

    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
   setUart();
   setADC();
   sonic();

 __bis_SR_register(GIE);                     // LPM0, ADC_ISR will force exit
  while (1);

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR (void) {
    // Measure interval tick
    __low_power_mode_off_on_exit();
    TA0CCR0 += MEASURE_INTERVAL;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR (void) {
    // Echo pin state toggled
    __low_power_mode_off_on_exit();
    TA0IV = 0;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
 switch(ADC12IV)
 {
 case  6: // Vector  6:  ADC12IFG0
 {
     adc = ADC12MEM0;
     CM = 10*(4800/(adc-20));
     /*if (CM <= 140)
             {
                 // Turn on LED
                 P4OUT &= BIT7;
             }
             else
             {
                 // Turn off LED
                 P4OUT &= ~BIT7;
             }

     if (CM <= 140)
         { if (distance <= DISTANCE_THRESHOLD)
             {
                 counter --;

             }
         }
     else*/
     if (distance < DISTANCE_THRESHOLD){
             {
                 counter ++;
             }
     }


   // Send to ASICC characters
             byte = 7;
             unsigned int SendTemp = counter; // Move two decimal places
             int i;
             for (i = 5; i < 8;)
             {

                 {
                     data[5] = 66;
                     i++;
                     data[6] = 65;
                     i++;
                     data[7] = 35;
                     i++;
                 }

             }
             data[4] = ' ';
             for (i = 1; i < 4; i++)
             {

                 {
                     data[i] = (SendTemp % 10) + '0';
                     SendTemp = SendTemp / 10;
                 }

             }
             data[0] = '\n';




             UCA1TXBUF = data[7]; // Ready to transmit
             break;
       }
         default:
 }
}

/* UART Interrupt*/
#pragma vector=USCI_A1_VECTOR
__interrupt void UartStart(void)
{
   if (UCA1IFG & UCTXIFG) // Check USCI A1 Interrupt Flags and USCI Transmit Interrupt Flag
       {
           UCA1IFG &= ~UCTXIFG; // Clear the TX flag
           if (byte > 0)
           {
               byte--;
               UCA1TXBUF = data[byte]; // Transmit message
           }
           else
           {
               __delay_cycles(100000); // Delay message
               ADC12CTL0 |= ADC12SC; // Start conversion
           }
       }

   if (UCA1IFG & UCRXIFG)
   {
       unsigned char setData = UCA1RXBUF; // Read data and clear flag
       counter = setData; // Set the new temperature to target
   }
}


