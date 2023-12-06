
#define DECODE_NEC          // Includes Apple and Onkyo
#include <Arduino.h>

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

//Pines (pines micro pro)
int salida1 = 6; // motor s  (5)
int salida2 = 5; // motor i (6)

int salida3 = 9; // motor b (9)
int salida4 = 10; // sentido b (10)

 //diodos
int sd1 = 14; // (14)
int sd2 = 15; // (15)
int sd3 = 16; // (16)

int entrada1 = 2; // up down
int entrada2 = 3; // ippo (foward) 
int entrada3 = 4; // right left
int entrada4 = 5; // calibration

//PWM
unsigned int PWM1 = 0; // valor max 255
unsigned int PWM2 = 0; // valor max 255
unsigned int PWM3 = 0; // valor max 255

float V1 = 0;
float V2 = 0;
float V3 = 0;
float V4 = 0;

void setup() {
    Serial.begin(115200);
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void loop() {
    //prueba();
    //Serial.println(analogRead(1));

    //delay(1000);



    if (IrReceiver.decode()) {

        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);

        //if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        //    Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print more info
        //    IrReceiver.printIRResultRawFormatted(&Serial, true);
        //}
        Serial.println();
        IrReceiver.resume(); // Enable receiving of the next value

        
        //leds (3 comandos-colores control)
        if (IrReceiver.decodedIRData.command == 0x80) {
            digitalWrite(sd1, !digitalRead(sd1));
            delay(1000);
        }else if (IrReceiver.decodedIRData.command == 0x40) {
            digitalWrite(sd2,!digitalRead(sd2));
            delay(1000);
        } else if (IrReceiver.decodedIRData.command == 0xC0) {
            digitalWrite(sd3,!digitalRead(sd3));
            delay(1000);
        }

        //up and down (canales 2 comandos)
        else if (IrReceiver.decodedIRData.command == 0x20) {
          V1 = V1 + 30;
        } else if (IrReceiver.decodedIRData.command == 0xA0) {
          if (V1 != 0){
          V1 = V1 - 30;}
        }

        //left and right (flechas 2 comandos)
        else if (IrReceiver.decodedIRData.command == 0x60) {
          V3 = 10;
        } else if (IrReceiver.decodedIRData.command == 0xE0) {
          V3 = -10;
        }

        //ippo (flechas 2 comandos)
        else if (IrReceiver.decodedIRData.command == 0x10) {
          V2 = 10;
          salida3 = 10;
          salida4 = 9;
        } else if (IrReceiver.decodedIRData.command == 0x90) {
          V2 = 10;
          salida3 = 9;
          salida4 = 10;
        }

        //stop (ok 1 comando)
        else if(IrReceiver.decodedIRData.command == 0x50){
          V3 = 0;
          V2 = 0;
        }

        //calibration (numeros 11 comandos)
        else if(IrReceiver.decodedIRData.command == 0xD0){
          V4 = -10;
        }else if(IrReceiver.decodedIRData.command == 0x30){
          V4 = -8;
        }else if(IrReceiver.decodedIRData.command == 0xB0){
          V4 = -6;
        }else if(IrReceiver.decodedIRData.command == 0x70){
          V4 = -4;
        }else if(IrReceiver.decodedIRData.command == 0xF0){
          V4 = -2;
        }else if(IrReceiver.decodedIRData.command == 0x8){
          V4 = 0;
        }else if(IrReceiver.decodedIRData.command == 0x88){
          V4 = 2;
        }else if(IrReceiver.decodedIRData.command == 0x48){
          V4 = 4;
        }else if(IrReceiver.decodedIRData.command == 0xC8){
          V4 = 6;
        }else if(IrReceiver.decodedIRData.command == 0x28){
          V4 = 8;
        }else if(IrReceiver.decodedIRData.command == 0xA8){
          V4 = 10;
        }


        PWM1 = ((255*V1)/ 100) + ((255*V3)/ 100);
        PWM2 = ((255*V1)/ 100) + ((255*V4)/ 100) - ((255*V3)/ 100);
        PWM3 = (255*V2)/ 100;
        
        PWM1 = max(0,min(PWM1,255));
        PWM2 = max(0,min(PWM2,255));
        PWM3 = max(0,min(PWM3,255));

        analogWrite(salida1,PWM1); // salida PWM1 pin 5 (255) moto s
        analogWrite(salida2,PWM2); // salida PWM2 pin 6 (255) moto i

        digitalWrite(salida4,LOW);    // salida 0 pin 11 o 10
        analogWrite(salida3,PWM3); // salida PWM3 pin 10 o 11 (255) ippo
        delay(1000);
    }
}
