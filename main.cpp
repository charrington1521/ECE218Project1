/*! @mainpage Example 1.5 Modularized and with doxygen comments
* @date Friday, January 29, 2021
* @authors Pablo Gomez, Ariel Lutenberg and Eric Pernia
* @section genDesc General Description
*
* This is a preliminary implementation of the smart home system, where the
* code has been modularized using functions and documented using Doxygen.
* The entry point to the program documentation can be found at
* this \ref Example_1_5_Modularized_withDoxygenComments.cpp "link"
*
* @section genRem General Remarks
* [Write here relevant information about the program]
*
* @section changelog Changelog
*
* | Date | Description |
* |:----------:|:-----------------------------------------------|
* | 01/14/2024 | First version of program |
*
*
*/

//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============

DigitalIn passengerOccupancy(D4); /**< Object associated passenger seat contact*/
DigitalIn passengerSeatbelt(D5); /**< Object associated passenger seatbelt contact*/
DigitalIn driverOccupancy(D6); /**< Object associated driver seat contact*/
DigitalIn driverSeatbelt(D7); /**< Object associated driver seatbelt contact*/

DigitalIn ignitionButton(BUTTON1); /**< Object associated with attempted ignition*/

DigitalOut ignitionLED(LED2); /**< Object associated with sucessful igntion*/
DigitalOut occupantsReadyLED(LED1); /**< Object associated with all junion learner permit conditions being met*/

DigitalInOut ignitionErrorBuzzer(PE_10); /**< Object associated with unsucessfule ignition*/

UnbufferedSerial uartUsb(USBTX, USBRX, 115200); /**< Object associated with the uart process*/

//=====[Declaration and initialization of public global variables]=============

int ignitionAttempts = 0; /**< Tracks the number of times the ignition has been tried*/
bool welcomeMessageSent = false; /**< Tracks whether or not the driver has been welcomed*/

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
/**<
This function configures gasDetector, overTempDetector and aButton to dButton
with internal pull-down resistors.
@param none
*/

void outputsInit();
/**<
This function initializes the outputs of the system:
-# alarmLed = OFF
-# incorrectCodeLed = OFF
-# systemBlockedLed = OFF
*/

void ignitionUpdate();
/**<
Checks for input of ignition input assigns ON to the buzzer and ingnitionErrorBuzzer
if not all of the junior learner permit conditions are met. 
Also displays the requirements when those conditions aren't met, and informs
users of a sucessful ignition attempt otherwise. 
*/

void conditionsUpdate();
/**<
Detects driver and passenger conditions assigns ON to the occupantsReady LED
when all junior learner permit condtions are met and welcomes the driver when they seat. 
*/

void errorMessages();
/**<
Prints out an error message for each unfulfilled junior learner permit ignition
coniditon. 
*/

//=====[Main function, the program entry point after power on or reset]========

/**
* Calls functions to initialize the declared input and output objects, and to
* implement the system behavior.
* @param none
* @return The returned value represents the success
* of application.
*/
int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        if (ignitionAttempts < 1) {
            conditionsUpdate();
            ignitionUpdate();
        }
    }
}

//Implementation of global functions

void inputsInit() {
    passengerOccupancy.mode(PullDown);
    passengerSeatbelt.mode(PullDown);
    driverOccupancy.mode(PullDown);
    driverSeatbelt.mode(PullDown);

    ignitionErrorBuzzer.mode(OpenDrain);
    ignitionErrorBuzzer.input();
}

void outputsInit() {
    ignitionLED = OFF;
    occupantsReadyLED = OFF;
    ignitionErrorBuzzer = OFF;
}

void ignitionUpdate() {
    if (ignitionButton) {
        if (occupantsReadyLED.read()) {
            occupantsReadyLED = OFF;
            ignitionLED = ON;
            uartUsb.write("Engine started.\r\n", 17);       
        } else {
            ignitionErrorBuzzer.output();
            ignitionErrorBuzzer = LOW;
            
            uartUsb.write("Ignition inhibited\r\n", 20);
            errorMessages();
        }
        ignitionAttempts++;
    }
}

void errorMessages() {
    if (!driverOccupancy) {
        uartUsb.write("Driver seat not occupied\r\n", 26);
    }
    if (!driverSeatbelt) {
        uartUsb.write("Driver seatbelt not fastened\r\n", 30);
    }
    if (!passengerOccupancy) {
        uartUsb.write("Passenger seat not occupied\r\n", 29);
    }
    if (!passengerSeatbelt) {
        uartUsb.write("Passenger seatbelt not fastened\r\n", 33);
    }
}

void conditionsUpdate() {
    if (driverOccupancy && !welcomeMessageSent) {
        uartUsb.write("Welcome to enhanced alarm system model 218-W24\r\n", 48);
        welcomeMessageSent = true;
    }
    if (driverOccupancy && driverSeatbelt && passengerOccupancy && passengerSeatbelt) {
        occupantsReadyLED = ON;
    } else {
        occupantsReadyLED = OFF;
    }
}