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

#define VEL_INT_FREQ    5000
#define QEI0_PPR        30

void init_qei0()
{
    // Enable the clock for peripherals PortD and QEI0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
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

    // Configure the QEI0 to increment for both PhA and PhB for quadrature input with "QEI0_PPR" PPR
        QEIConfigure(QEI0_BASE, QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_NO_RESET | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP, QEI0_PPR);

    // Configure the QEI0 for Velocity Calculation, Predivide by 1 at "VEL_INT_FREQ" Hz
        QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, SysCtlClockGet() / VEL_INT_FREQ);
        QEIVelocityEnable(QEI0_BASE);

    // Enable the QEI1
        QEIEnable(QEI0_BASE);


}



