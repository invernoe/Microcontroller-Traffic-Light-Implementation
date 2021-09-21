# Microcontroller-Traffic-Light-Implementation
A C project built on a TM4c123GH6PM microcontroller that implements a traffic light of a crossroads with pedestrian buttons using interrupts

#Short Description:

This system is an implementation of a traffic light system. Consisting of 2 road
lights for cars (east/west road & north/south road) that contain red, yellow, and
green lEDs for each traffic light, and 2 pedestrian lights each containing red, and 
green lights to signify for pedestrians when they can pass through. The pedestrian
traffic lights each have 1 switch to activate them.

----------------------------

#Functionality:

The system manages the road traffic lights by using timers. One timer counts 5 seconds
for GREEN light, 2 seconds for YELLOW light, then 1 second where both lights are RED before
switching to the other road traffic light and continue the loop.

Whenever a pedestrian presses the pedestrian traffic light button, the normal sequence of the
road traffic lights stops to begin the pedestrian traffic light sequence, where the traffic light
for the designated button that was pressed is turned to GREEN, its corresponding road traffic light
is turned to RED, and the road opposite of the pedestrian traffic light is set to GREEN for 2 seconds.
After that duration is done the the pedestrian traffic light returns to its RED state while the road
traffic lights resume their behaviour from where they stopped when the button was pressed, then the button
for the designated pedestrian traffic light is locked for 1 second to avoid spamming the pedestrian button.

With every light change the system prints out its current state using UART serial communication, explaining
which road has which traffic light open currently for both pedestrian traffic lights and car traffic lights.

The system starts with the east/west road light as green and the north/south road
light as red and starting the counter at 5 seconds. While also initializing the 2
pedestrian lights as red.

----------------------------

#Basic Scenario:

1- All peripherals initialized and Timer0 starts counting down 5 seconds, GREEN light for east/west road and
RED light for north/south road are activated, RED light for both pedestrian lights are turned on.

2- Three Seconds pass and pedestrian at east/west road presses button, GREEN light for east/west pedestrian traffic
light turns on, while RED light for east/west car road and GREEN light for north/south car road are turned on, Timer1 starts
to countdown 2 seconds.

3- Timer1 ellapses; state of road restored: GREEN light for east/west road and RED light for north/south road are activated, while RED
light for east/west pedestrian road is activated. Timer0 is set to count for the remainder 2 seconds. Timer2 starts couting
down one second and further requests are locked.

4- Pedestrian presses button at east/west road 0.5 seconds after it is done, the request is cancelled because Timer2 did not ellapse.

5- Timer2 ellapses and pedestrian requests are now unlocked.

6- Timer0 ellapses, YELLOW light for east/west road and RED light for north/south road are activated. Timer0 set to countdown for 2 seconds.

7- Timer0 ellapses, RED light for east/west road and RED light for north/south road are activated. Timer0 set to countdown for 1 second.

8- Timer0 ellapses, RED light for east/west road and GREEN light for north/south road are activated. Timer0 set to countdown for 5 seconds.

----------------------------

#Peripheral Configuration:

Port A:
Pin 1 -> UART0 transmitter (output)

Pin 2 -> GREEN light output for east/west road
Pin 3 -> YELLOW light output for east/west road
Pin 4 -> RED light for output east/west road

Pin 5 -> GREEN light output for north/south road
Pin 6 -> YELLOW light output for north/south road
Pin 7 -> RED light output for north/south road

Port E:
Pin 0 -> GREEN light output for east/west pedestrian light
Pin 1 -> RED light output for east/west pedestrian light

Pin 2 -> GREEN light output for north/south pedestrian light
Pin 3 -> RED light output for north/south pedestrian light

Port C:
Pin 7 -> switch input for north/south pedestrian light

Port F:
Pin 4 -> switch input for east/west pedestrian light

=======================================================

Timers:
TIMER0 -> 32 bit one-shot countdown timer. Main timer for managing time for car traffic lights.
TIMER1 -> 32 bit one-shot countdown timer. Timer that counts 2 seconds for pedestrians to pass
TIMER2 -> 32 bit one-shot countdown timer. Timer that counts 1 second after east/west pedestrian light ellapses to limit button presses.
TIMER3 -> 32 bit one-shot countdown timer. Timer that counts 1 second after north/south pedestrian light ellapses to limit button presses.
