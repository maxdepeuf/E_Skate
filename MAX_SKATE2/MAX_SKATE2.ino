#include "VescUart.h"
VescUart UART;

int state_Acc_Brake = 0;
int state_Reg = 0;
int state_Light = 0;
int Pot_Value = 0;
int Pot_Value_F = 0;
int gaz;
int Gaz_Pin = 9;
int Gaz_Pin2 = 6;
int State_bat = 7;
long t_co;
long t_f;
char inChar = 0;
String stringData;
bool newline = false;
String data_in = "";
String data_in2 = "";
String command = "";
String data_out = "";
boolean newData = false;
int state_uart;
float speed_reg;

long speed_uart;
float volt_uart;
float intmot_uart;
double vmin_batt = 33.6;
double vmax_batt = 42.0;
float Pot_Threshold = 10;

float Acc_Current;
float Acc_Current_F;
float Acc_Current_Max = 50;
float Brake_Current_Max = 30;
const int numReadings = 10;
long t;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;
float r;
const int RunningAverageCount = 10;
float RunningAverageBuffer[RunningAverageCount];
int NextRunningAverage;
const byte bat_led[] = {45, 47, 49, 51, 53};
int i;
long t_led_bat = 0;
int nb;
void setup() {
  Serial.begin(9600);//Serial
  //Serial1.begin(38400);//Ble
 // Serial2.begin(38400);//µSkate
    Serial1.begin(57600);//Ble
  Serial2.begin(115200);//µSkate
  while (!Serial) {
    ;
  }
  UART.setSerialPort(&Serial2);
  for (int i = 0; i <= 4 ; i++) {
    pinMode (bat_led[i], OUTPUT) ;
  }
  delay(1000);
  for (int i = 0 ; i <= 4 ; i++) {
    digitalWrite (bat_led[i], LOW) ; // éteint la DEL reliée à la broche
  }
  for (int k = 0 ; k <= 1 ; k++) {
    for (int i = 0 ; i <= 4 ; i++) {
      digitalWrite (bat_led[i], HIGH) ; // éteint la DEL reliée à la broche
      delay (40) ;
    }
    for (int i = 0 ; i <= 4 ; i++) {
      digitalWrite (bat_led[i], LOW) ; // éteint la DEL reliée à la broche
      delay (40) ;
    }
    for (int i = 0 ; i <= 4 ; i++) {
      digitalWrite (bat_led[4 - i], HIGH) ; // éteint la DEL reliée à la broche i
      delay (40) ;
    }
    for (int i = 0 ; i <= 4 ; i++) {
      digitalWrite (bat_led[4 - i], LOW) ; // éteint la DEL reliée à la broche i
      delay (40) ;
    }
  }
  for (int i = 0 ; i <= 4; i++) {
    nb = (int)(75. / (2.*(6 - i)));
    for (int k = 0 ; k <= nb ; k++) {
      digitalWrite (bat_led[i], HIGH) ;
      UART.setCurrent(10);
      delay(6 - i);
      UART.setCurrent(0);
      delay(6 - i);
    }
    delay (75) ;
  }
  for (int i = 0 ; i <= 4 ; i++) {
    digitalWrite (bat_led[i], LOW) ; // éteint la DEL reliée à la broche i
  }
  nb = (int)(75. / 2.);
  for (int i = 0 ; i <= 3 ; i++) {
    for (int k = 0 ; k <= nb ; k++) {
      digitalWrite (bat_led[4], HIGH);
      UART.setCurrent(20);
      delay(1);
      UART.setCurrent(0);
      delay(1);
    }
    digitalWrite (bat_led[4], LOW) ;
    delay (75) ;

  }
  UART.setCurrent(0);

  t_co = millis();
  Serial1.println("<Skate>");

}

void loop() {
  delay(1);
  led_bat(volt_uart);
  if ( UART.getVescValues() ) {
    speed_uart = ((UART.data.rpm));
    volt_uart = ((UART.data.inpVoltage));
    intmot_uart = ((UART.data.avgMotorCurrent));
  }
  data_out = (String)(String(speed_uart) + "," + String(volt_uart) + "," + String(1) + "," + String(intmot_uart) + ",");
  Serial.println(data_out);
  if (Serial1.available() > 0) {
    while (Serial1.available()) {
      inChar = Serial1.read();
      if (inChar == '\n') {
        newline = true;
        break;
      } else {
        data_in = data_in + inChar;
      }
    }
  }
  if (newline) {
    //Serial.println(data_in);
    Pot_Value = ((data_in.substring(0, data_in.indexOf(','))).toInt());
    state_Acc_Brake = (data_in.substring(data_in.indexOf(',') + 1, data_in.indexOf(',', data_in.indexOf(',') + 1))).toInt();
    state_Reg = (data_in.substring((data_in.indexOf(',', data_in.indexOf(',') + 1)) + 1, data_in2.indexOf(',', data_in.indexOf(',', data_in.indexOf(',') + 1) + 1))).toInt();
    state_Light = (data_in.substring(data_in.indexOf(',', data_in.indexOf(',', data_in.indexOf(',') + 1) + 1) + 1, data_in.lastIndexOf(','))).toInt();
    data_in = "";
    newline = false;
    Serial1.println(data_out);
    Serial1.flush();
    t_co = millis();
  }
  if ((millis() - t_co) > 500) {
    data_in = "";
    state_Acc_Brake = 0;
    state_Reg = 0;
    Acc_Current = 0;
    Acc_Current_F = 0;
    UART.setCurrent(0);
  } else {
    RunningAverageBuffer[NextRunningAverage++] = Pot_Value;
    if (NextRunningAverage >= RunningAverageCount)
    {
      NextRunningAverage = 0;
    }
    float RunningAverageTemperature = 0;
    for (int i = 0; i < RunningAverageCount; ++i)
    {
      Pot_Value_F += RunningAverageBuffer[i];
    }
    Pot_Value_F /= RunningAverageCount;
    //    Serial.print(Pot_Value_F);
    //    Serial.print(",");
    //    Serial.println(Pot_Value);
    //Serial.println(state_Acc_Brake);
    if (state_Reg == 0) {
      if (Pot_Value_F > int(Pot_Threshold)) {
        if (state_Acc_Brake == 0) {
          speed_reg = (float)speed_uart;
          Acc_Current = ((float)(Pot_Value_F) - (float)Pot_Threshold) * ((float)Acc_Current_Max) / (100.0 - (float)Pot_Threshold);
          Serial.println(Acc_Current);
          UART.setCurrent(Acc_Current);
        } else {
          Acc_Current = ((float)(Pot_Value_F) - (float)Pot_Threshold) * ((float)Brake_Current_Max) / (100.0 - (float)Pot_Threshold);
          UART.setBrakeCurrent(Acc_Current);
          Serial.println(-Acc_Current);
        }
      } else {
        Acc_Current = 0;
        UART.setCurrent(0);
        Serial.println(0);
      }
    } else {
      Acc_Current = Acc_Current;
      speed_reg = speed_reg;
      UART.setRPM(speed_reg);
      //Serial.println(speed_reg);
    }
  }
}


void led_bat(double volt) {
  if ((millis() - t_led_bat) >= 3000) {
    t_led_bat = millis();
    if (volt <= (vmin_batt)) {
      digitalWrite (bat_led[0], LOW) ;
      digitalWrite (bat_led[1], LOW) ;
      digitalWrite (bat_led[2], LOW) ;
      digitalWrite (bat_led[3], LOW) ;
      digitalWrite (bat_led[4], LOW) ;
    } else if ((volt > (vmin_batt)) && (volt <= ((vmax_batt - vmin_batt) / 5.) + vmin_batt)) {
      digitalWrite (bat_led[0], LOW) ;
      digitalWrite (bat_led[1], LOW) ;
      digitalWrite (bat_led[2], LOW) ;
      digitalWrite (bat_led[3], LOW) ;
      digitalWrite (bat_led[4], HIGH) ;

    } else if ((volt > ((vmax_batt - vmin_batt) / 5.) + vmin_batt) && (volt <= (2.*(vmax_batt - vmin_batt) / 5.) + vmin_batt)) {
      digitalWrite (bat_led[0], LOW) ;
      digitalWrite (bat_led[1], LOW) ;
      digitalWrite (bat_led[2], LOW) ;
      digitalWrite (bat_led[3], HIGH) ;
      digitalWrite (bat_led[4], HIGH) ;
    } else if ((volt > (2.*(vmax_batt - vmin_batt) / 5.) + vmin_batt) && (volt <= (3.*(vmax_batt - vmin_batt) / 5.) + vmin_batt)) {
      digitalWrite (bat_led[0], LOW) ;
      digitalWrite (bat_led[1], LOW) ;
      digitalWrite (bat_led[2], HIGH) ;
      digitalWrite (bat_led[3], HIGH) ;
      digitalWrite (bat_led[4], HIGH) ;
    } else if ((volt > (3.*(vmax_batt - vmin_batt) / 5.) + vmin_batt) && (volt <= (4.*(vmax_batt - vmin_batt) / 5.) + vmin_batt)) {
      digitalWrite (bat_led[0], LOW) ;
      digitalWrite (bat_led[1], HIGH) ;
      digitalWrite (bat_led[2], HIGH) ;
      digitalWrite (bat_led[3], HIGH) ;
      digitalWrite (bat_led[4], HIGH) ;
    } else if ((volt > (4.*(vmax_batt - vmin_batt) / 5.) + vmin_batt)) {
      digitalWrite (bat_led[0], HIGH) ;
      digitalWrite (bat_led[1], HIGH) ;
      digitalWrite (bat_led[2], HIGH) ;
      digitalWrite (bat_led[3], HIGH) ;
      digitalWrite (bat_led[4], HIGH) ;
    }
  }
}
