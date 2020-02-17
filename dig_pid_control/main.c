#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <set_pwm.h>
#include <set_adc.h>
#include <set_uart.h>
#include <set_qei.h>
#include <uartstdio.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_GPIO.h"
//#include "inc/hw_uart.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/qei.h"

//#include "utils/uartstdio.h"

#define VEL_INT_FREQ    5000               // Interrupt frequency of QEI0
#define QEI0_PPR        419              // Pulses Per Rev of the QEI0


uint32_t const s_freq_adc = 10000;
uint32_t samples_adc[1]; //0 - 4096
uint32_t speed;


uint32_t enco_Vel;
uint32_t enco_Pos;              // Variable to store the position of QEI0
int32_t enco_Dir;                // Variable to store the direction of QEI0
uint16_t enco_Vel_Rpm;              // Variable to store the RPM of QEI0

void motor_speed();
void com_uart();
void int_handler_qei0();


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    init_pwm();
    init_adc(s_freq_adc);
    init_uart();
    init_qei0();
    //UARTprintf("Hello world %d!\r\n", 1234);
    /*
    uint8_t Text[] = {"Vel RPM: "};
    uint8_t i;
    for (i = 0; i<sizeof(Text); i++ )
        UARTCharPutNonBlocking(UART0_BASE, Text[i]);
        */

    while(1)
    {

        // ISR register
        ADCIntRegister(ADC0_BASE, 3, motor_speed);

        //registers the handler to be called when a quadrature encoder interrupt occurs
        void (*int_handler_qei0_ptr)(void) = int_handler_qei0;
        int_handler_qei0_ptr();
        UARTprintf("velocidad %d!\r\n", enco_Vel_Rpm);
        QEIIntRegister(QEI0_BASE, *int_handler_qei0_ptr);
        QEIIntEnable(QEI0_BASE, QEI_INTTIMER);
        //com_uart();



    }
}

void motor_speed()
{
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, samples_adc);
    speed = (uint32_t)((320.0*(float)samples_adc[0])/4096.0); //320.0 -> PWM period in clock ticks
    if (speed < 5)
            {
                speed = 5;
            }
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, speed);

}

void com_uart(){
    // ISR for UART interrupt handling
    // Clear the asserted UART interrupts
    UARTIntClear(UART0_BASE, UARTIntStatus(UART0_BASE, true));
    // While there is a character available at input
    while(UARTCharsAvail(UART0_BASE)){
        // Echo the character back to the user
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

void int_handler_qei0(){
    // ISR for Quadrature Encoder Interface 1
    // Clear the interrupt that is generated
    QEIIntClear(QEI0_BASE, QEIIntStatus(QEI0_BASE, true));
    // Calculate the number of quadrature ticks in "1 / VEL_INT_FREQ" time period
    enco_Vel = QEIVelocityGet(QEI0_BASE);
    // Update the position reading of the encoder
    enco_Pos = QEIPositionGet(QEI0_BASE);
    // Update the direction reading of the encoder
    enco_Dir = QEIDirectionGet(QEI0_BASE);
    // Calculate the velocity in RPM
    enco_Vel_Rpm = enco_Vel * VEL_INT_FREQ * 60 / QEI0_PPR;

    //UARTprintf("velocidad %d!\r\n", enco_Vel_Rpm);
}
