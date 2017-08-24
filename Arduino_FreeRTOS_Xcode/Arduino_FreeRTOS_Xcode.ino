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
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
void TaskProducer( void *pvParameters );
void TaskConsumer( void *pvParameters );
//void print_log_event(void);


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
    while (!Serial) {
        ;
    }

    //Hello world using string vector
    strings.push_back("Hello world");
    copy(strings.begin(),strings.end(),ostream_iterator<string>(cout," "));
    cout << endl;
    
    
//    // Set up tasks
//    xTaskCreate(
//                TaskBlink  //Task code, of type TaskFunction_t
//                ,  (const portCHAR *)"Blink"   // Task name
//                ,  128  // Stack size - check and adjust by reading the Stack Highwater
//                ,  NULL //Parameters
//                ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//                ,  NULL ); //Task handle of type TaskHandle_t*
//    
//    xTaskCreate(
//                TaskAnalogRead
//                ,  (const portCHAR *) "AnalogRead"
//                ,  128
//                ,  NULL
//                ,  1
//                ,  NULL );
    
    xTaskCreate(
                TaskProducer
                ,  (const portCHAR *) "Producer"
                ,  128
                ,  NULL
                ,  2
                ,  NULL );
    xTaskCreate(
                TaskConsumer
                ,  (const portCHAR *) "Consumer"
                ,  128
                ,  NULL
                ,  1
                ,  NULL );
    
    xQueue1 = xQueueCreate(
                10
                 ,  sizeof(int));
    
}

void loop()
{
    // Needed in Arduino
}


/******************  TASKS  ********************/

void TaskProducer(void *pvParameters)
{
    (void) pvParameters;
    
    while(1)
    {
        counter++;
        //This queue/message pair is not atomic and doesn't represent what's going on. How to correct this?
        xQueueSend( xQueue1, (void *)&counter, (TickType_t)3000 / portTICK_PERIOD_MS );
        cout << "Producer put in queue:" << counter << endl;
        vTaskDelay(500 / portTICK_PERIOD_MS);
        //delay(500);
    }
}

void TaskConsumer(void *pvParameters)
{
    (void) pvParameters;
    
    while(1)
    {
        int message;
        xQueueReceive( xQueue1, (void *)&message, (TickType_t)1000 / portTICK_PERIOD_MS );
        cout << "Consumed: " << message << endl;
        vTaskDelay(1500 / portTICK_PERIOD_MS);  // 1.5s
    }
}

/* Blink - Turns on an LED on for one second, then off for one second, repeatedly. */
void TaskBlink(void *pvParameters)
{
    (void) pvParameters;
    
    pinMode(LED_BUILTIN, OUTPUT);
    
    while(1)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay( 1000 / portTICK_PERIOD_MS ); //1000 ms
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}

/* AnalogReadSerial - Reads an analog input on pin 0, prints the result to the serial monitor.*/
void TaskAnalogRead(void *pvParameters)
{
    (void) pvParameters;
    
    while(1)
    {
        // read the input on analog pin 0:
        int sensorValue = analogRead(A0);
        // print out the value you read:
        Serial.println(sensorValue);
        vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
    }
}

//void print_log_event(void){
//    Serial.println("xQueue1 queue send failed");
//}
