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
#define QEI0_PPR        30              // Pulses Per Rev of the QEI0

//PID values
#define Ki 0.08
#define KD 0.0145
#define KP 10

//Sample period PID
#define Delta_t 0.001

float K_i = Ki*Delta_t ;
float K_D = KD/Delta_t ;
float K_P = KP;
float e_k_1 = 0;
float E_k = 0;
float e_k = 0;
float e_D = 0;
float u_k = 0;

uint32_t const s_freq_adc = 10000;
uint32_t samples_adc[1]; //0 - 4096
uint32_t speed;
uint32_t ref_pos;


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

    while(1)
    {
        // ISR register
        ADCIntRegister(ADC0_BASE, 3, motor_speed);

        // Analog read of reference sensor
        ref_pos = (uint32_t)((320.0*(float)samples_adc[0])/4096.0); //320.0 -> PWM period in clock ticks


        if (ref_pos < 5)
        {
            ref_pos = 5;
        }


        //PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, speed);

        // Update the position reading of the encoder
        enco_Pos = (float)QEIPositionGet(QEI0_BASE);
        //registers the handler to be called when a quadrature encoder interrupt occurs
        //void (*int_handler_qei0_ptr)(void) = int_handler_qei0;
        //int_handler_qei0_ptr();

        /*
        // *******************************PID algorithm**************************************
        e_k = ref_pos - enco_Pos;
        e_D = e_k - e_k_1;
        E_k = E_k + e_k;
        u_k = K_P*e_k + K_i*E_k + K_D*e_D;
        e_k_1 = e_k;

        if (u_k > 6){
            u_k = 6;
          }
          if (u_k < 0){
            u_k = 0;
          }

        speed = (uint32_t)((320/6)*(u_k));


        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, speed);
        */
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ref_pos);

        UARTprintf("posicion %d:\r\n", enco_Pos);


    }
}

void motor_speed()
{
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, samples_adc);

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

}
