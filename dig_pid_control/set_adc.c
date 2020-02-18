/*
 * set_adc.c
 *
 *  Created on: 5/02/2020
 *      Author: Samuel Hernández
 *  Based on:
 *      - TivaWare Driverlib User's Guide.pdf / Analog to digital converter (ADC) API
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


#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/qei.h"



void init_adc(uint32_t s_freq_adc)
{
    /*
     * TIMER CONFIGURATION
     */
    //
    // Enable the Timer0 peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //
    // Wait for the Timer0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    //
    // Configure TimerA as a half-width periodic.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    //
    // Set the count time for the the one-shot timer (TimerA).
    //
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/s_freq_adc);
    //
    //Enable the ADC trigger output by timer A.
    //
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    //
    // Enable the timer.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);

    /*
     * ADC CONFIGURATION
     */
    //
    // Enable the ADC0 module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //
    // Wait for the ADC0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {
    }
    // Enable the analog pin (PB5 in this case).
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    //
    // Enable the first sample sequencer to capture the value of channel 11 when
    // the ADC trigger timer occurs.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 1);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    //
    // Enable the ADC interruption
    //
    ADCIntClear(ADC0_BASE, 3);
    ADCIntEnable(ADC0_BASE, 3);
    //
    // Enable an interrupt in the interrupt controller.
    //
    //IntEnable(INT_ADC0SS3);
    //IntMasterEnable();
}
