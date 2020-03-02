/*
 * set_qei.c
 *
 *  Created on: 9/02/2020
 *      Author: Samuel Hernández
 *      Based on:
 *      - TivaWare Driverlib User's Guide.pdf / Quadrature encoder AP
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <uartstdio.h>
//#include <set_tmr.h>

#include <set_pwm.h>
#include <set_adc.h>
#include <set_uart.h>
#include <set_qei.h>
#include <stdio.h>
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

#define VEL_INT_FREQ    350
#define QEI0_PPR        419

void init_qei0()
{
    // Enable the clock for peripherals PortD and QEI0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);

    //
    // Wait for the QEI0 module to be ready.
    //
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_QEI0))
        {
        }

    // Configure the PD6, PD7 for QEI signals
        GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
        GPIOPinConfigure(GPIO_PD6_PHA0);
        GPIOPinConfigure(GPIO_PD7_PHB0);

    //Make sure quadrature encoder is off
        QEIDisable(QEI0_BASE);
        QEIIntDisable(QEI0_BASE, QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);

    // Configure the QEI0 to increment for both PhA and PhB for quadrature input with "QEI0_PPR" PPR
        QEIConfigure(QEI0_BASE, QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_NO_RESET | QEI_CONFIG_QUADRATURE | QEI_CONFIG_SWAP, QEI0_PPR);

    // Enable the QEI1
        QEIEnable(QEI0_BASE);

    //Enable noise filter
        //QEIFilterDisable(QEI0_BASE);
        //QEIFilterConfigure(QEI0_BASE, QEI_FILTCNT_17);
        //QEIFilterEnable(QEI0_BASE);

    // Configure the QEI0 for Velocity Calculation, Predivide by 1 at "VEL_INT_FREQ" Hz
        QEIVelocityDisable(QEI0_BASE);
        QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1,  SysCtlClockGet()/ VEL_INT_FREQ);
        QEIVelocityEnable(QEI0_BASE);

    // Enable processor interrupts.
        //IntMasterEnable();
        //IntEnable(INT_QEI0);


    //Enable Interrupt
        QEIIntEnable(QEI0_BASE, QEI_INTTIMER);
}



