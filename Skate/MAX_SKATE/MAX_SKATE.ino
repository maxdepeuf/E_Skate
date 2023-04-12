#include<SoftwareSerial.h>
#include <VescUart.h>
SoftwareSerial ble(11,12);
VescUart UART;

int gaz;
int Gaz_Pin=9;
int Gaz_Pin2=6;
int State_bat=7;
long t_co;
long t_f;

String stringData;
double speed_uart;
double volt_uart;
double int_uart;
double intmot_uart;

String data="";
String command="";

void setup() {
  Serial.begin(115200);
  ble.begin(19200);
  while (!Serial) {;}

  UART.setSerialPort(&Serial);
  
  pinMode(Gaz_Pin, OUTPUT);

  stringData = String();
  delay(1000);
  if (Serial.available() <= 0) {
    ble.print('A');
  }
}

void loop() {
  int_uart=digitalRead(State_bat);

/////////////////////////////

//if (millis()-t>=500){
// ble.print(stringData);
// ble.flush(); 
//t=millis();
//}

  if ( UART.getVescValues() ) {
  speed_uart=((UART.data.rpm));
  volt_uart=((UART.data.inpVoltage));
  intmot_uart=((UART.data.avgMotorCurrent));
  stringData= String(speed_uart) + "," + String(volt_uart) + "," + String(int_uart) +","+ String(intmot_uart)+",";
}
////////////////////////////
ble.flush();
if (ble.available()>0){
  while (ble.available()){
    char inChar =(char)ble.read();
    if (inChar=='\n'){
           Serial.print(command);
           data=command;  
           command = ""; 
           t_co=millis();
           
    }else{
      command += inChar;
    }
  }
  ble.println(stringData);
  //delay(100); 
  
}
  
  if ((millis()-t_co)>500){
    gaz=0;
}else{
  gaz=data.toInt();
}
           
analogWrite(Gaz_Pin,gaz);
analogWrite(Gaz_Pin2,gaz); 
///////////////////////////////////////////////  
}


  
