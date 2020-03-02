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


#define VEL_INT_FREQ    995              // Interrupt frequency of QEI0
#define QEI0_PPR        419              // Pulses Per Rev of the QEI0

// Constantes para el PID
#define Ki 0.08
#define KD 0.0145
#define KP 10

// Período de muestreo
#define Delta_t 0.001

uint32_t const s_freq_adc = 80000;
uint32_t samples_adc[1]; //0 - 4096
uint32_t speed;
uint32_t pos;
uint32_t ref_pos;


uint32_t enco_Vel;
uint32_t enco_Pos;              // Variable to store the position of QEI0
int32_t enco_Dir;                // Variable to store the direction of QEI0
uint16_t enco_Vel_Rpm;              // Variable to store the RPM of QEI0
uint16_t count = 0;

float K_i = Ki*Delta_t ;
float K_D = KD/Delta_t ;
float K_P = KP;
float e_k_1 = 0;
float E_k = 0;

float e_k = 0;
float e_D = 0;
float u_k = 0;

void adc_read();
void int_handler_qei0();


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    init_pwm();
    init_adc(s_freq_adc);
    init_uart();
    init_qei0();
    // ISR register
    ADCIntRegister(ADC0_BASE, 3, adc_read);
    QEIIntRegister(QEI0_BASE, int_handler_qei0);

    while(1);

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
    enco_Vel_Rpm = QEIVelocityGet(QEI0_BASE) * VEL_INT_FREQ * 60 / QEI0_PPR;
    //UARTprintf("posicion %d:\r\n", enco_Pos);
    //UARTprintf("velocidad %d!\r\n", enco_Vel_Rpm);
    //QEIIntEnable(QEI0_BASE, QEI_INTTIMER);
}

void adc_read()
{
    count++;
    ADCIntClear(ADC0_BASE, 3);
    ADCSequenceDataGet(ADC0_BASE, 3, samples_adc);
    //reference
    speed = (uint32_t)((320.0*(float)samples_adc[0])/4096.0); //320.0 -> PWM period in clock ticks
    if (speed < 5)
            {
                speed = 5;
            }
    //enco_Pos = QEIPositionGet(QEI0_BASE);

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, speed);
    if (count == 1000)
        {
            count = 0;
            UARTprintf("velocidad %d!\r\n", enco_Vel);
        }

    // Algoritmo para el PID
/*
    e_k = speed - enco_Pos;
    e_D = e_k - e_k_1;
    E_k = E_k + e_k;
    u_k = K_P*e_k + K_i*E_k + K_D*e_D;
    e_k_1 = e_k;

    // Cotas sup e inf para u_k
    if (u_k > 29){
        u_k = 29;
    }
    if (u_k < -29){
        u_k = -29;
    }

    pos = (320/30)*(u_k);

    if (pos>0)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, pos);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 1);
    }
    else if (pos<=0)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pos);
    }

    UARTprintf("posicion %d:\r\n", enco_Pos);
*/


}





