//Includes
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include "inc/hw_memmap2.h"
#include "inc/hw_types2.h"
#include "inc/hw_gpio2.h"
#include "inc/hw_i2c2.h"
#include "driverlib/gpio2.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl2.h"
#include "driverlib/i2c.h"
#include "driverlib/i2c.c"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"

//Prototypes
void InitConsole(void); 
void Init1(void);
void Init2(void);
void CheckFirstByte(uint32_t first_byte);


//Determines the sample rate of the ADC 
uint32_t clkscalevalue = 6;
uint32_t SLAVE_ADDRESS = 0x1D;
//uint8_t SLAVE_ADDRESS_READ = 0x31;
uint32_t first_byte;

//*****************************************************************************
//
// Configure ADC0 for the temperature sensor input with a single sample.  Once
// the sample is done, an interrupt flag will be set, and the data will be
// read then displayed on the console via UART0.
//
//*****************************************************************************
int main(void)
{   
    // Set the clocking to run at 20 MHz (200 MHz / 5) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ); //changed from SYSCTL_SYSDIV_10 (CM)

    // Set up the serial console to use for displaying messages.  This is just
    // for this lab program and is not needed for ADC operation.
    InitConsole();
    
		Init2();
		Init1();
		
		UARTprintf("Initialization Complete...\n");

    // Sample the temperature sensor forever.  Display the value on the
    // console.
		
		
		
		
    while(1)
    {		
			I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
			I2CMasterDataPut(I2C2_BASE, 0x29);
			I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);
			
			while(I2CMasterBusy(I2C2_BASE)){};
			I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);
			I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
				
		  while(I2CMasterBusy(I2C2_BASE)){};
			first_byte = I2CMasterDataGet(I2C2_BASE);
					
				
			if(first_byte == 255){
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1, GPIO_PIN_1); // Red LED on, blue off
				UARTprintf("\n1");
			}
			else{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1, GPIO_PIN_2); // Blue LED on, red off
				UARTprintf("\n0");
			}
				
		 SysCtlDelay(SysCtlClockGet() / clkscalevalue);
			
			
    }
}


void CheckFirstByte(uint32_t first_byte)
{
	if(first_byte > 127){
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	}
	else{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2, 1);
	}
}


// This function inits all the pins and clocks we need for the accelo.

void Init1()
{
	// Enable the I2C module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

	// reset peripherals
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);
	
	// enable E GPIO port
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	
	//Wait at least 5 clock cycles
	SysCtlDelay(2);
	
	//Configure PE5 as serial data pin (SDA) and PE4 as serial clock pin (SCL)
	GPIOPinConfigure(GPIO_PE4_I2C2SCL);
	GPIOPinConfigure(GPIO_PE5_I2C2SDA);
	
	SysCtlDelay(2);
	
	// Set PE5 as SDA and PE4 as SDA
	GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
	GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);
	
	// Configure Master
	I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);
	
	// Configure Slave
	
	//I2CSlaveInit(I2C2_BASE, SLAVE_ADDRESS);
	
	//Write to control register 0x87
	I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, false);
	I2CMasterDataPut(I2C2_BASE, 0x20);
	
	I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	
	while(I2CMasterBusy(I2C2_BASE)){};
	I2CMasterSlaveAddrSet(I2C2_BASE, SLAVE_ADDRESS, true);
  I2CMasterDataPut(I2C2_BASE, 0x87);
	I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		
	while(I2CMasterBusy(I2C2_BASE)){};
}


void Init2()
{
	// enable port f
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlDelay(3); // wait for clock to restart
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // LEDS set to standard GPIO outputs
}

//PROVIDED FUNCTIONS.  NO NEED TO MODIFY
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the lab is running.
//
//*****************************************************************************
void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
   

		// Select the alternate (UART) function for these pins.   
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O. 9600 BAUD
    UARTStdioConfig(0, 9600, 16000000);
}
