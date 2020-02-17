/*
 * set_pwm.c
 *
 *  Created on: 4/02/2020
 *      Author: Samuel Hernández
 *
 *      Based on:
 *      - https://www.egr.msu.edu/classes/ece480/capstone/fall15/group09/appnotes/JoshuaLambApplicationNote.pdf
 *      - TivaWare Driverlib User's Guide.pdf / Pulse Width Modulator (PWM)
 */

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

void init_pwm()
{
    //
    // Enable the PWM0 peripheral
    //
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    //
    // Wait for the PWM0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {
    }

    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinConfigure(GPIO_PB7_M0PWM1);

    GPIOPinTypePWM(GPIO_PORTB_BASE,GPIO_PIN_6 | GPIO_PIN_7);
    //
    // Configure the PWM generator for count down mode with immediate updates
    // to the parameters.
    //
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    //
    // Set the period. For a 50 KHz PWM frequency, the period = 1/50,000, or 20
    // microseconds. For a 80 MHz clock divided by 5, this translates to 320 clock ticks.
    // Use this value to set the period.
    //
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 320);
    //
    // Set the pulse width of PWM0 for a (1/320)*100% duty cycle.
    //
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 1);
    //
    // Set the pulse width of PWM1 for a 75% duty cycle.
    //
    //PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 150);
    //
    // Start the timers in generator 0.
    //
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    //
    // Enable the outputs.
    //
    PWMOutputState(PWM0_BASE, (PWM_OUT_0_BIT | PWM_OUT_1_BIT), true);
}


