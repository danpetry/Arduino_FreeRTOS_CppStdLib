//
// Arduino_FreeRTOS_Xcode
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Daniel Petry
// 				Daniel
//
// Date			25/04/2017 21:31
// Version		<#version#>
//
// Copyright	© Daniel Petry, 2017
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.8 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters


// Include application, user and local libraries
#include <Arduino_FreeRTOS.h>
#include <StandardCplusplus.h>
#include <serstream>
#include <string>
#include <vector>
#include <iterator>
#include <queue.h>
#include <avr/pgmspace.h>

// Define structures and classes


// Define variables and constants
int counter = 0;
QueueHandle_t xQueue1;

// Prototypes
// define two tasks for Blink & AnalogRead
//void TaskBlink( void *pvParameters );
//void TaskAnalogRead( void *pvParameters );
void TaskProducer( void *pvParameters );
void TaskConsumer( void *pvParameters );
void print_log_event(void);


// Utilities
#define traceTASK_INCREMENT_TICK(xTickCount) print_log_event();

// Functions

using namespace std;
// Add setup code
// <iostream> declares cout/cerr, but the application must define them
// because it's up to you what to do with them.
namespace std
{
    ohserialstream cout(Serial);
} 

vector<string> strings;

// the setup function runs once when you press reset or power the board
void setup() {
    
    Serial.begin(9600);
    
    strings.push_back("Hello,");
    strings.push_back("world!");
    copy(strings.begin(),strings.end(),ostream_iterator<string>(cout," "));
    cout << endl;
    
    cout << "Hello, world." << endl;
    
    cout << F("I use up absolutely no RAM space whatsoever") << endl;
    cout << F("so you can use as many F() strings as you want!") << endl;
    
    float fmax = __FLT_MAX__, fmin = __FLT_MIN__;
    cout.precision(7);
    cout << "Float " << scientific << fmax << endl;
    cout << "Float " << scientific << fmin << endl;
    
    cout << "+OK" << endl;
    
    int n = 3;
    int (*button)[3] = new int[n][3];
    
    // initialize serial communication at 9600 bits per second:
    
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
    }
    
    // Now set up two tasks to run independently.
//    xTaskCreate(
//                TaskBlink
//                ,  (const portCHAR *)"Blink"   // A name just for humans
//                ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
//                ,  NULL
//                ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//                ,  NULL );
//    
//    xTaskCreate(
//                TaskAnalogRead
//                ,  (const portCHAR *) "AnalogRead"
//                ,  128  // Stack size
//                ,  NULL
//                ,  1  // Priority
//                ,  NULL );
    
    xTaskCreate(
                TaskProducer
                ,  (const portCHAR *) "Producer"
                ,  128  // Stack size
                ,  NULL
                ,  2  // Priority
                ,  NULL );
    xTaskCreate(
                TaskConsumer
                ,  (const portCHAR *) "Consumer"
                ,  128  // Stack size
                ,  NULL
                ,  1  // Priority
                ,  NULL );
    
    xQueue1 = xQueueCreate(
                10
                 ,  sizeof(int));
    
    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskProducer(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    
    for (;;)
    {
        counter++;
        cout << "Producer: ";
        cout << counter << endl;
        xQueueSend( xQueue1, (void *)&counter, (TickType_t)10 );
        delay(10);
        //vTaskDelay(10);  // 1.5s
    }
}

void TaskConsumer(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    
    for (;;)
    {
        int message;
        xQueueReceive( xQueue1, (void *)&message, (TickType_t)10 );
        cout << "Consumer: ";
        cout << message << endl;
        vTaskDelay(10);  // 1.5s
    }
}
void TaskBlink(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    
    /*
     Blink
     Turns on an LED on for one second, then off for one second, repeatedly.
     
     Most Arduinos have an on-board LED you can control. On the UNO, LEONARDO, MEGA, and ZERO
     it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care
     of use the correct LED pin whatever is the board used.
     
     The MICRO does not have a LED_BUILTIN available. For the MICRO board please substitute
     the LED_BUILTIN definition with either LED_BUILTIN_RX or LED_BUILTIN_TX.
     e.g. pinMode(LED_BUILTIN_RX, OUTPUT); etc.
     
     If you want to know what pin the on-board LED is connected to on your Arduino model, check
     the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
     
     This example code is in the public domain.
     
     modified 8 May 2014
     by Scott Fitzgerald
     
     modified 2 Sep 2016
     by Arturo Guadalupi
     */
    
    // initialize digital LED_BUILTIN on pin 13 as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    
    for (;;) // A Task shall never return or exit.
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    }
}

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
    (void) pvParameters;
    
    /*
     AnalogReadSerial
     Reads an analog input on pin 0, prints the result to the serial monitor.
     Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
     Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
     
     This example code is in the public domain.
     */
    
    for (;;)
    {
        // read the input on analog pin 0:
        int sensorValue = analogRead(A0);
        // print out the value you read:
        Serial.println(sensorValue);
        vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    }
}

void print_log_event(void){
    Serial.println("xQueue1 queue send failed");
}
