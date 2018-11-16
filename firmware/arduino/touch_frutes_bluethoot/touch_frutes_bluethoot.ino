/**
   Software: Touch Frutes Bluethoot
   Description:
   Date: 30/08/2018
   Author: RodrigueFAS
   Contact: franciscosouzaacer@gmail.com
*/

#include <Wire.h>

#define LED 13
#define BAUND_SPEED 9600
#define numElectrodes 12
#define PIN 10  // Which pin on the Arduino is connected to the NeoPixels?
#define NUMPIXELS 12 // How many NeoPixels are attached to the Arduino?

// char used for reading in Serial characters.
char inbyte = 0;

int Pin_IRQ = 2;            // Pin IRQ en el pin digital 2
int address_mpr = 0x5A;     //dirección de nuestro MPR121
boolean EstadoAnterior[12]; //Para Alamacenar el ultimo estado del valor del touch

#define PIEZO_01 A1
#define PIEZO_02 A2
#define PIEZO_03 A3

void setup() {
  Serial.begin(BAUND_SPEED);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(Pin_IRQ, INPUT);
  digitalWrite(Pin_IRQ, HIGH); //Habilitamos la resistencia pullup
  Wire.begin();
  mpr121_Config(address_mpr);

  pinMode(PIEZO_01, INPUT);
  pinMode(PIEZO_02, INPUT);
  pinMode(PIEZO_03, INPUT);
}

void loop() {

  //when serial values have been received this will be true.
  if (Serial.available() > 0) {
    inbyte = Serial.read();
    if (inbyte == '0') {
      digitalWrite(LED, LOW);
    }
    if (inbyte == '1') {
      digitalWrite(LED, HIGH);
    }
  }

  read_piezo();
  read_touch();

}

void read_piezo(){

  int value_piezo_01 = digitalRead(PIEZO_01);
  int value_piezo_02 = digitalRead(PIEZO_02);
  int value_piezo_03 = digitalRead(PIEZO_03);

  if(value_piezo_01){
    Serial.println(value_piezo_01);
    send_data(91);
  }
  if(value_piezo_02){
    send_data(92);
  }
  if(value_piezo_03){
    send_data(93);
  }

}

void read_touch() {
  if (!digitalRead(Pin_IRQ)){
    // Leemos los estados de los electrodos del MPR121
    Wire.requestFrom(address_mpr, 2);

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    uint16_t touched = ((MSB << 8) | LSB); // 12 de los 16 bits corresponden a los estados táctiles

    // Comprobamos si los electrodos se han presionado
    for (int i = 0; i < 12; i++){
      boolean EstadoActual = bitRead(touched, i);

      if (!(EstadoAnterior[i]) && EstadoActual) send_data(i);
      if (EstadoAnterior[i] && (!EstadoActual)){
        // Serial.print("Electrodo ");
        // Serial.println(i);
        // Serial.println(" ya no está siendo tocado");
      }

      EstadoAnterior[i] = EstadoActual;
    }
  }
}

  void send_data(int id){
  Serial.print('#');
  Serial.println(id);
  Serial.print('~');
  Serial.println();
}

void mpr121_Config(int address){

  set_register(address, 0x5E, 0x00); //ELE_CFG

  // Section A - Controls filtering when data is > baseline.
  set_register(address, 0x2B, 0x01); //MHD_R
  set_register(address, 0x2C, 0x01); //NHD_R
  set_register(address, 0x2D, 0x00); //NCL_R
  set_register(address, 0x2E, 0x00); //FDL_R

  // Section B - Controls filtering when data is < baseline.
  set_register(address, 0x2F, 0x01); //MHD_F
  set_register(address, 0x30, 0x01); //NHD_F
  set_register(address, 0x31, 0xFF); //NCL_F
  set_register(address, 0x32, 0x02); //FDL_F

  // Section C - Sets touch and release thresholds for each electrode
  int umbral_Touch = 0x44;
  int umbral_Release = 0x40;

  set_register(address, 0x41, umbral_Touch);   //Umbral Touch 0
  set_register(address, 0x42, umbral_Release); //Umbral Release 0

  set_register(address, 0x43, umbral_Touch);   //Umbral Touch 1
  set_register(address, 0x44, umbral_Release); //Umbral Release 1

  set_register(0x5A, 0x45, umbral_Touch);   //Umbral Touch 2
  set_register(0x5A, 0x46, umbral_Release); //Umbral Release 2

  set_register(address, 0x47, umbral_Touch);   //Umbral Touch 3
  set_register(address, 0x48, umbral_Release); //Umbral Release 3

  set_register(address, 0x49, umbral_Touch);   //Umbral Touch 4
  set_register(address, 0x4A, umbral_Release); //Umbral Release 4

  set_register(address, 0x4B, umbral_Touch);   //Umbral Touch 5
  set_register(address, 0x4C, umbral_Release); //Umbral Release 5

  set_register(address, 0x4D, umbral_Touch);   //Umbral Touch 6
  set_register(address, 0x4E, umbral_Release); //Umbral Release 6

  set_register(address, 0x4F, umbral_Touch);   //Umbral Touch 7
  set_register(address, 0x50, umbral_Release); //Umbral Release 7

  set_register(address, 0x51, umbral_Touch);   //Umbral Touch 8
  set_register(address, 0x52, umbral_Release); //Umbral Release 8

  set_register(address, 0x53, umbral_Touch);   //Umbral Touch 9
  set_register(address, 0x54, umbral_Release); //Umbral Release 9

  set_register(address, 0x55, umbral_Touch);   //Umbral Touch 10
  set_register(address, 0x56, umbral_Release); //Umbral Release 10

  set_register(address, 0x57, umbral_Touch);   //Umbral Touch 11
  set_register(address, 0x58, umbral_Release); //Umbral Release 11

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(address, 0x5D, 0x04); //FIL_CFG

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(address, 0x5E, 0x0C); // ELE_CFG  Enables all 12 Electrodes
}

void set_register(int address, unsigned char r, unsigned char v){
  Wire.beginTransmission(address);
  Wire.write(r);
  Wire.write(v);
  Wire.endTransmission();
}


