#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_timer.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "tm4c123gh6pm.h"

#define INT_TIMER0A_TM4C123 35
#define GREEN_LIGHT_DURATION 16000*5000-1
#define YELLOW_LIGHT_DURATION 16000*2000-1
#define GREEN_AFTER_RED_LIGHT_DURATION 16000*1000-1
#define PEDESTRIAN_LIGHT_DURATION 16000*2000-1

int state = 0; //state for traffic light, 0 for green, 1 for yellow, 2 for red
int roadState = 0; //state for which road has moving cars, 0 for east/west road and 1 for north/south road

char pedestrianStateArray[][50] = {"PEDESTRIAN EAST WEST GREEN LIGHT\n", "PEDESTRIAN EAST WEST RED LIGHT\n",
	"PEDESTRIAN NORTH SOUTH GREEN LIGHT\n", "PEDESTRIAN NORTH SOUTH RED LIGHT\n"};

char stateArray[2][3][50] = {{"CARS EAST WEST GREEN LIGHT\n", "CARS EAST WEST YELLOW LIGHT\n", "CARS EAST WEST RED LIGHT\n"},
                         {"CARS NORTH SOUTH GREEN LIGHT\n", "CARS NORTH SOUTH YELLOW LIGHT\n", "CARS NORTH SOUTH RED LIGHT\n"}
                        };

//function that prints string to UART
void printStringUART(char* string){
	while(*string){
		UARTCharPut(UART0_BASE, *(string++));
	};
}

void lightGreenEastWest(){
	//give high value to green light of east/west road, high value to red light of north/south road, and low to the rest
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_7), (GPIO_PIN_2 | GPIO_PIN_7));
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6), 0x00);
	
	printStringUART(stateArray[0][0]);
	printStringUART(stateArray[1][2]);
}

void lightYellowEastWest(){
	//give high value to yellow light of east/west road, high value to red light of north/south road, and low to the rest
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_3 | GPIO_PIN_7), (GPIO_PIN_3 | GPIO_PIN_7));
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6), 0x00);
	
	printStringUART(stateArray[0][1]);
	printStringUART(stateArray[1][2]);
}

void lightGreenNorthSouth(){
	//give high value to green light of north/south road, high value to red light of east/west road, and low to the rest
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_4 | GPIO_PIN_5), (GPIO_PIN_4 | GPIO_PIN_5));
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7), 0x00);
	
	printStringUART(stateArray[0][2]);
	printStringUART(stateArray[1][0]);
}

void lightYellowNorthSouth(){
	//give high value to yellow light of north/south road, high value to red light of east/west road, and low to the rest
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_4 | GPIO_PIN_6), (GPIO_PIN_4 | GPIO_PIN_6));
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7), 0x00);
	
	printStringUART(stateArray[0][2]);
	printStringUART(stateArray[1][1]);
}

void lightRedAll(){
	//give high value to red light of east/west road, high value to red light of north/south road, and low to the rest		
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_4 | GPIO_PIN_7), (GPIO_PIN_4 | GPIO_PIN_7));
	GPIOPinWrite(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6), 0x00);
	
	printStringUART(stateArray[0][2]);
	printStringUART(stateArray[1][2]);
}

void lightGreenEastWestPedestrian(){
	//open green light and close red light
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_PIN_0);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
	
	printStringUART(pedestrianStateArray[0]);
}

void lightRedEastWestPedestrian(){
	//open red light and close green light
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0x00);
	
	printStringUART(pedestrianStateArray[1]);
}

void lightGreenNorthSouthPedestrian(){
	//open green light and close red light
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0x00);
	
	printStringUART(pedestrianStateArray[2]);
}

void lightRedNorthSouthPedestrian(){
	//open red light and close green light
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);
	
	printStringUART(pedestrianStateArray[3]);
}

//starts timer to start counting down pedestrian traffic lights
void start_timer1(void){
    TimerEnable(TIMER1_BASE, TIMER_BOTH);
}

void timerInterrupt(void){
    TimerIntClear(TIMER0_BASE, 0x1); //clears interrupt flag for timer0A

    switch(state){
        case 0:
            //if timer was in first state, set load timer to yellow light duration and change light to yellow
            TimerLoadSet (TIMER0_BASE, TIMER_BOTH, YELLOW_LIGHT_DURATION);
            state = 1;

            if(roadState == 0){
                lightYellowEastWest();
            }else{
                lightYellowNorthSouth();
            }
            break;
        case 1:
            //if timer was in second state, set load timer to the 1 second delay before a green light duration and change light to red
            TimerLoadSet (TIMER0_BASE, TIMER_BOTH, GREEN_AFTER_RED_LIGHT_DURATION);
            state = 2;

            //we dont put an if statement because both conditions will do the same function
            lightRedAll();
            break;
        case 2:
            //if timer was in third state, set load timer to green light duration and change light to green
            TimerLoadSet (TIMER0_BASE, TIMER_BOTH, GREEN_LIGHT_DURATION);
            state = 0;
            roadState ^= 0x01; //change road state using XOR to toggle between the two roads

            if(roadState == 0){
                lightGreenEastWest();
            }else{
                lightGreenNorthSouth();
            }
            break;
    }

    TimerEnable(TIMER0_BASE, TIMER_BOTH); //re-enables timer to start counting again
}

void init_timer0(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerDisable(TIMER0_BASE, TIMER_BOTH);
    TimerConfigure (TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    //sets priority of timer0A interrupt to 4
    IntPrioritySet(INT_TIMER0A_TM4C123, 0x80000000); //NVIC_PRI4_R |= 0x80000000;
    TimerLoadSet (TIMER0_BASE, TIMER_BOTH, GREEN_LIGHT_DURATION); //set the timer duration to the 5 second green light duration at startup
    TimerIntClear(TIMER0_BASE, 0x1); //clears interrupt flag for timer0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister (TIMER0_BASE, TIMER_BOTH, timerInterrupt);
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

void init_timer1(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));

    TimerDisable(TIMER1_BASE, TIMER_BOTH);
    TimerConfigure (TIMER1_BASE, TIMER_CFG_ONE_SHOT);

    TimerLoadSet (TIMER1_BASE, TIMER_BOTH, PEDESTRIAN_LIGHT_DURATION); //set the timer duration to the 2 seconds of the pedestrian traffic light
    TimerIntClear(TIMER1_BASE, 0x1); //clears interrupt flag for timer1A
}


void restoreState(uint32_t remainingTime){
	
	switch(state){
		case 0:
			if(roadState == 0){
				lightYellowEastWest();
			}else{
				lightYellowNorthSouth();
			}
			break;
		case 1:
			lightRedAll();
			break;
		case 2:
			//if timer was in third state, set load timer to green light duration and change light to green
			if(roadState == 0){
				lightGreenEastWest();
			}else{
				lightGreenNorthSouth();
			}
			break;
	}
	
	TimerLoadSet (TIMER0_BASE, TIMER_BOTH, remainingTime); //sets the time to the remaining time it had before interruption
	TimerEnable(TIMER0_BASE, TIMER_BOTH); //re-enables the timer
}

bool hasOneSecondPassed(int EastOrNorth){ //0 for East, 1 for North
	if(EastOrNorth == 0){
		if(TimerValueGet(TIMER2_BASE, TIMER_A) == 0){
			return true;
		}
		return false;
	}else{
		if(TimerValueGet(TIMER3_BASE, TIMER_A) == 0){
			return true;
		}
		return false;
	}
}

void pedestrianTimerInterruptEastWest(void){
	uint32_t remainingTime = TimerValueGet (TIMER0_BASE, TIMER_A); //get the remaining value of the car traffic timer
	TimerDisable(TIMER0_BASE, TIMER_BOTH); //disable timer that counts remaining time for car traffic
	
	if(!hasOneSecondPassed(0)){ //if one Second has not passed on this timer being pressed then restore state and return function
		restoreState(remainingTime);
		return;
	}
	
	lightGreenEastWestPedestrian(); //light green light for east/west pedestrian light
	lightGreenNorthSouth(); //light north south car traffic light as green and east west traffic light as red

	start_timer1(); //start the timer so it can begin counting down
	while(TimerValueGet(TIMER1_BASE, TIMER_A) != 0); //if timer 1 has not ellapsed yet, stay in while loop
	TimerIntClear(TIMER1_BASE, 0x1); //clears interrupt flag for timer1A
	
	TimerEnable(TIMER2_BASE, TIMER_BOTH); //enable timer2 to start counting 1 second to limit consecutive button presses
	lightRedEastWestPedestrian(); //return pedestrian traffic light back to red
	restoreState(remainingTime); //restore state of traffic light and start timer with the remaining time
}

void pedestrianTimerInterruptNorthSouth(void){
	uint32_t remainingTime = TimerValueGet (TIMER0_BASE, TIMER_A); //get the remaining value of the car traffic timer
	TimerDisable(TIMER0_BASE, TIMER_BOTH); //disable timer that counts remaining time for car traffic
	
	if(!hasOneSecondPassed(1)){ //if one Second has not passed on this timer being pressed then restore state and return function
		restoreState(remainingTime);
		return;
	}
	
	lightGreenNorthSouthPedestrian(); //light green light for north/south pedestrian light
	lightGreenEastWest(); //light east/west car traffic light as green and north/south traffic light as red

	start_timer1(); //start the timer so it can begin counting down
	while(TimerValueGet(TIMER1_BASE, TIMER_A) != 0); //if timer 1 has not ellapsed yet, stay in while loop
	TimerIntClear(TIMER1_BASE, 0x1); //clears interrupt flag for timer1A
	
	TimerEnable(TIMER3_BASE, TIMER_BOTH); //enable timer3 to start counting 1 second to limit consecutive button presses
	lightRedNorthSouthPedestrian(); //return pedestrian traffic light back to red
	restoreState(remainingTime); //restore state of traffic light and start timer with the remaining time
}

//port A will be assigned to all 6 leds that make the traffic lights
void init_portA(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2); //GREEN LIGHT FOR EAST/WEST ROAD
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3); //YELLOW LIGHT FOR EAST/WEST ROAD
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4); //RED LIGHT FOR EAST/WEST ROAD
    
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_5); //GREEN LIGHT FOR NORTH/SOUTH ROAD
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6); //YELLOW LIGHT FOR NORTH/SOUTH ROAD
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7); //RED LIGHT FOR NORTH/SOUTH ROAD
}

//port E will be assigned to 4 leds that make the east/west north/south pedestrian light
void init_portE(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0); //GREEN LIGHT FOR EAST/WEST PEDESTRIAN LIGHT
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1); //RED LIGHT FOR EAST/WEST PEDESTRIAN LIGHT
    
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2); //GREEN LIGHT FOR NORTH/SOUTH PEDESTRIAN LIGHT
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3); //RED LIGHT FOR NORTH/SOUTH PEDESTRIAN LIGHT
    
    lightRedEastWestPedestrian();
    lightRedNorthSouthPedestrian();	//initialize leds by making both lights red
}

//function to initialize port D and F to work as input switches
void init_switches(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF | SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF | SYSCTL_PERIPH_GPIOD));
    
    GPIOIntRegister(GPIO_PORTF_BASE, pedestrianTimerInterruptEastWest); //interrupt handler for pedestrian east west light
    GPIOIntRegister(GPIO_PORTD_BASE, pedestrianTimerInterruptNorthSouth); // interrupt handler for pedestrian north south light

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //SWITCH FOR EAST/WEST PEDESTRIAN LIGHT
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7); //SWITCH FOR NORTH/SOUTH PEDESTRIAN LIGHT
    
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_7, GPIO_RISING_EDGE);
    
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4); //enable interrupts for both pins
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_7);
}


void init_oneSecondTimers(void){ //initialize timers that will count one second after the pedestrian lights are done
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2 | SYSCTL_PERIPH_TIMER3);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2 | SYSCTL_PERIPH_TIMER3));
    
    //Timer2 for east west pedestrian light limit
    TimerDisable(TIMER2_BASE, TIMER_BOTH);
    TimerConfigure (TIMER2_BASE, TIMER_CFG_ONE_SHOT);
    
    TimerLoadSet (TIMER2_BASE, TIMER_BOTH, GREEN_AFTER_RED_LIGHT_DURATION); //set the timer duration to the 1 second
    TimerIntClear(TIMER2_BASE, 0x1); //clears interrupt flag for timer2A
    
    //Timer3 for north south pedestrian light limit
    TimerDisable(TIMER3_BASE, TIMER_BOTH);
    TimerConfigure (TIMER3_BASE, TIMER_CFG_ONE_SHOT);
    
    TimerLoadSet (TIMER3_BASE, TIMER_BOTH, GREEN_AFTER_RED_LIGHT_DURATION); //set the timer duration to the 1 second
    TimerIntClear(TIMER3_BASE, 0x1); //clears interrupt flag for timer2A
}

void initUart0(void){  
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));
    
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

int main()
{

	//enable interrupts on the cpu
	IntMasterEnable();
	init_portA();
	init_portE();
	initUart0();
	init_timer0();
	init_timer1();
	init_oneSecondTimers();
	init_switches();
	
	while(1){  		
    }
  
 	return 0;
}
