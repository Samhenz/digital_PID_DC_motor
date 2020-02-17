/*
 * set_uart.c
 *
 *  Created on: 7/02/2020
 *      Author: Samuel Hernández
 *      Based on:
 *      - TivaWare Driverlib User's Guide.pdf / (UART) API
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <uartstdio.h>

#include <stdio.h>
#include <set_pwm.h>
#include <set_adc.h>
#include <set_uart.h>
#include <set_qei.h>

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

#define ui32PortNum 0
#define ui32Baud 115200

void init_uart()
{
    //
    // Enable the UART0 module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Wait for the UART0 module to be ready.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    { }
    // Configure PA0 as UART0_Rx and PA1 as UART0_Tx
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Initialize the UART. Set the baud rate, number of data bits, turn off
    // parity, number of stop bits, and stick mode. The UART is enabled by the
    // function call.
    //

    /*UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));
    */
    UARTStdioConfig(ui32PortNum, ui32Baud, SysCtlClockGet());
    // Configure and enable the interrupt for UART0
    //IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTEnable(UART0_BASE);

    }
