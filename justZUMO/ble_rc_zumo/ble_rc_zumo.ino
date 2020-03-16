//#include "BluetoothSerial.h"
#include <SoftwareSerial.h>
#define THROTTLE 'T'
#define STEERING 'S'
/*
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
*/

//BluetoothSerial SerialBT;
SoftwareSerial SerialBT(2,3);

int x = 0; //LDR top left
int y = 1; //LDR top rigt


void setup() {
  Serial.begin(115200);
  SerialBT.begin(115200);
 
}

byte buff[4];

void loop() {
 int throttle=analogRead(x);
 int steering=analogRead(y);
 Serial.println("..................");
 Serial.println(String(throttle));
 Serial.println(String(steering));

//  Serial.println(".");
/*
  int avt=(lt+rt)/2;
  int avd=(ld+rd)/2;
  int avl=(lt+ld)/2;
  int avr=(rt+rd)/2;

  int dvert=avt-avd;
  int dhoriz=avl-avr;
*/

 int n_t=packetTransBT(throttle,THROTTLE);
 if(n_t!=4)
    Serial.println("throttle value transmit FAIL");
    
  int n_s=packetTransBT(steering,STEERING);
   if(n_s!=4)
    Serial.println("steering value transmit FAIL");
}

int packetTransBT(int value,char marker){
  byte byt;
  buff[0]=72;
  buff[1]=marker;
  byt=lowByte(value);
  if(byt==0)
    buff[2]='0';
  else
    buff[2]=byt;
  byt=highByte(value);
  Serial.println(byt);
  if(byt==0)
    buff[3]='0';
  else
    buff[3]=byt;
  int n=SerialBT.write(buff,4);
  if(n==4)
    return n;
  return 0;
}

/*
  
  if(SerialBT.available()){
    Serial.write(SerialBT.read());
  }
*/
  
/*
  if(Serial.available()){
    
    BTSerial.write(Serial.read());
  }
*/


/*
void sendInt32(long integer){
  int intValue_16;
  intValue_16=integer&0xFFFF;
  
//  Serial.println(String(intValue_16));
  SerialBT.write(lowByte(intValue_16));
  SerialBT.write(highByte(intValue_16));

  intValue_16=integer>>16;
//  Serial.println(String(intValue_16));
  SerialBT.write(lowByte(intValue_16));
  SerialBT.write(highByte(intValue_16));
}
*/
