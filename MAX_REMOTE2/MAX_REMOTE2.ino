#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#if (SSD1306_LCDHEIGHT != 32)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int Pile_Pin = A1;//batt
const int Pot_Pin = A0;
const int But_Pin = 2;

int Pot_Value = 0;
int Pot_Value_M = 0;
int But_Value = 0;

String data_in = "";
String data_out = "";
boolean newline = false;
char inChar = 0;
long t;
long t_co;
long t_screen;
int Pot_Threshold = 10;
double Acc_Current;
double Acc_Current_Max = 80;
double Brake_Current;
double Brake_Current_Max = 60;
int Trig_buttonState;
int i ;
int state_Trigger;
int last_state_Trigger = 0;
int last_But_Value = LOW;
unsigned long lastDebounceTime = 0;
unsigned long lastDebounceTime_Trig = 0;
unsigned long debounceDelay = 50;

int But_buttonState;

const int RunningAverageCount = 10;
int RunningAverageBuffer[RunningAverageCount];
int NextRunningAverage;

int But_State;

int Pot_Value_F;
int state_Acc_Brake = 0;
int state_Reg = 0;
int state_Light = 0;

double speed_uart = 0;
double volt_uart = 0;
int int_uart = 0;
double intmot_uart = 0;

double const_speed = ((60.0 * 0.083 * 3.17) / 2.13) / 7000.0;

double power = 0;
double spee = 0;
double maxspee = 0;
double maxpower = 0;
double meanspee = 0;
double sumspee = 0;

double Pile_Value = 0;
double Pile_Value_F = 0;
int Pot;

String data = "";
String stringGaz = "";
String command = "";

long tt;

long tempstot;
long tinit;

double distance = 0;

double vmin_batt = 33.6;
double vmax_batt = 42.0;

double vmin_pile = 2.96;
double vmax_pile = 4.2;

double batt = 0;
double battpc = 0;
int battp = 0;
double pile = 0;
double pilepc = 0;
int pilep = 0;

int State_Bat = 0;

void setup() {
  Serial.begin(57600);
  while (!Serial) {
  }
  pinMode(But_Pin, INPUT_PULLUP);
  pinMode(Pot_Pin, INPUT);
  pinMode(Pile_Pin, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.clearDisplay();
  //testscrolltext();
  display.setRotation(1);
  t_co = millis();
  tinit = millis();
  t_screen=millis();
  Serial.println("<Remote>");
  tt=micros();
 Wire.setClock(400000);
 //1000000
 //3400000
}

void loop() {
  //delay(1);
  //Serial.println(tt-micros());
  button();
  trigger();
  data_out = (String)(String(Pot_Value) + "," + String(state_Acc_Brake) + "," + String(state_Reg) + "," + String(state_Light) + ",");
if (Serial.available() > 0){
  while (Serial.available() > 0) {
    inChar = Serial.read();
    if (inChar == '\n') {
      newline = true;
      break;
    } else {
      data_in = data_in + inChar;
    }
  }
}
  if (newline) {
    speed_uart = ((data_in.substring(0, data_in.indexOf(','))).toDouble());
    volt_uart = (data_in.substring(data_in.indexOf(',') + 1, data_in.indexOf(',', data_in.indexOf(',') + 1))).toDouble();
    int_uart = (data_in.substring((data_in.indexOf(',', data_in.indexOf(',') + 1)) + 1, data_in.indexOf(',', data_in.indexOf(',', data_in.indexOf(',') + 1) + 1))).toInt();
    intmot_uart = abs((data_in.substring(data_in.indexOf(',', data_in.indexOf(',', data_in.indexOf(',') + 1) + 1) + 1, data_in.lastIndexOf(','))).toDouble());
    data_in = "";
    newline = false;

    Serial.println(data_out);
    Serial.flush();
    t_co = millis();
  }
  if ((millis() - t_co) > 500) {
    int_uart = 0;
  }

  Pile_Value = analogRead(Pile_Pin);

  pile = ((Pile_Value) * (5.0)) / (1023.0);
  pilep = round((pile - vmin_pile) * (5) / (vmax_pile - vmin_pile));

  speed_uart = abs(speed_uart);
  batt = volt_uart;
  spee = (speed_uart * const_speed);
  //maxspee = max(maxspee, spee);
  //power = volt_uart * intmot_uart;

  //if ((millis() - tt) > 1000) {
 // display.display();
 // t_screen = millis();

 if (int_uart==0){
  screen();
  tt=millis();
  tinit=millis();
  //sumspee =0;
  //meanspee=0;
  i=0;
 }else{
      

     
//sumspee = (sumspee + spee);
 // meanspee = abs(sumspee / i);
  //i++;

        if (millis()-tt>2000){
    screen(); 
     tt=millis();
 }
//  tempstot = millis() - tinit;
  //distance = abs(meanspee * 0.278 * tempstot * 0.001 * 0.001);
 }



}
void trigger() {
  Pot_Value = analogRead(Pot_Pin);
  Pot_Value = (map(Pot_Value, 940, 660, 0, 100));
  if (Pot_Value > 100) {
    Pot_Value = 100;
  }
  if (Pot_Value < 0) {
    Pot_Value = 0;
  }

  if (Pot_Value > Pot_Threshold) {
    state_Trigger = 1;
  } else {
    state_Trigger = 0;
  }

  if (state_Trigger != last_state_Trigger) {
    lastDebounceTime_Trig = millis();
  }
  if ((millis() - lastDebounceTime_Trig) > debounceDelay) {
    if (state_Trigger != Trig_buttonState) {
      Trig_buttonState = state_Trigger;
      if (Trig_buttonState == 1) {
        if (state_Reg == 1) {
          state_Reg = 0;
        }
      }
    }
  }
  last_state_Trigger = state_Trigger;
}


void button() {
  But_Value = !digitalRead(But_Pin);
  if (But_Value != last_But_Value) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (But_Value != But_buttonState) {
      But_buttonState = But_Value;
      if (But_buttonState == 1) {
        if (state_Trigger == 1) {
          if (state_Acc_Brake == 0) {
            state_Reg++;
            if (state_Reg >= 2) {
              state_Reg = 0;
            }
          }
        } else {
          if (state_Reg == 1) {
            state_Reg = 0;
          }
          state_Acc_Brake++;
          if (state_Acc_Brake >= 2) {
            state_Acc_Brake = 0;
          }
        }
      }
    }
  }
  last_But_Value = But_Value;
}


void testscrolltext() {

  display.setRotation(2);

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 10);
  display.clearDisplay();
  display.println("BroSK8");
  display.display();
  delay(1);
  display.startscrollright(0x00, 0x02);
  delay(3000);
  display.stopscroll();
}



void  screen() {
  display.clearDisplay();

  if (int_uart == 0) {
    display.setRotation(2);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(50, 5);
    display.print ("Connection");
    display.setCursor(60, 15);
    if (millis() % 1800 <= 300) {
    }
    else  if ((millis() % 1800 > 300) && (millis() % 1800 <= 600)) {
      display.print ("*");
    } else  if ((millis() % 1800 > 600) && (millis() % 1800 <= 900)) {
      display.print ("**");
    }
    else  if ((millis() % 1800 > 900) && (millis() % 1800 <= 1200)) {
      display.print ("***");
    }
       else  if ((millis() % 1800 > 1200) && (millis() % 1800 <= 1500)) {
      display.print ("****");
    }
        else if (millis() % 1800 > 1500) {
      display.print ("*****");
    }

    display.setRotation(1);
    display.fillRect(1, 17, 3, 6, WHITE);
    display.drawRect(3, 14, 28, 12, WHITE);
    if (pilep < 1) {
    } else if (pilep >= 1 && pilep < 2) {
      display.fillRect(25, 16, 4, 8, WHITE);
    } else if (pilep >= 2 && pilep < 3) {
      display.fillRect(25, 16, 4, 8, WHITE);
      display.fillRect(20, 16, 4, 8, WHITE);
    } else if (pilep >= 3 && pilep < 4) {
      display.fillRect(25, 16, 4, 8, WHITE);
      display.fillRect(20, 16, 4, 8, WHITE);
      display.fillRect(15, 16, 4, 8, WHITE);
    } else if (pilep >= 4 && pilep < 5) {
      display.fillRect(25, 16, 4, 8, WHITE);
      display.fillRect(20, 16, 4, 8, WHITE);
      display.fillRect(15, 16, 4, 8, WHITE);
      display.fillRect(10, 16, 4, 8, WHITE);
    } else if (pilep >= 5) {
      display.fillRect(25, 16, 4, 8, WHITE);
      display.fillRect(20, 16, 4, 8, WHITE);
      display.fillRect(15, 16, 4, 8, WHITE);
      display.fillRect(10, 16, 4, 8, WHITE);
      display.fillRect(5, 16, 4, 8, WHITE);
    }
    pilepc = (pile - vmin_pile) * (100.0) / (vmax_pile - vmin_pile);
    if (pilepc < 0) {
      pilepc = 0;
    }
    if (pilepc > 100) {
      pilepc = 100;
    }
    display.setTextSize(1);
    if (pilepc < 10) {
      display.setCursor(12, 30);
    } else if (pilepc > 9 && pilepc < 100) {
      display.setCursor(8, 30);
    } else if (pilepc = 100) {
      display.setCursor(4, 30);
    }
    display.print(pilepc, 0);
    display.println("%");


  
  } else {

    display.fillRect(1, 3, 3, 6, WHITE);
    display.drawRect(3, 0, 28, 12, WHITE);
    
    display.drawRect(6, 42, 20, 45, WHITE);
    display.drawRect(5, 41, 22, 47, WHITE);
    display.fillRect(10, 37, 12, 5, WHITE);
    //batt=42;
    battp = (batt - vmin_batt) * (39.0) / (vmax_batt - vmin_batt);
    if (battp < 0) {
      battp = 0;
    }
    if (battp > 39) {
      battp = 39;
    }
    display.fillRect(9, 45+(39-battp), 14,battp, WHITE);
    //display.fillRect(8 + (24 - battp), 48, 16,41, WHITE);

    if (pilep < 1) {
    } else if (pilep >= 1 && pilep < 2) {
      display.fillRect(25, 2, 4, 8, WHITE);
    } else if (pilep >= 2 && pilep < 3) {
      display.fillRect(25, 2, 4, 8, WHITE);
      display.fillRect(20, 2, 4, 8, WHITE);
    } else if (pilep >= 3 && pilep < 4) {
      display.fillRect(25, 2, 4, 8, WHITE);
      display.fillRect(20, 2, 4, 8, WHITE);
      display.fillRect(15, 2, 4, 8, WHITE);
    } else if (pilep >= 4 && pilep < 5) {
      display.fillRect(25, 2, 4, 8, WHITE);
      display.fillRect(20, 2, 4, 8, WHITE);
      display.fillRect(15, 2, 4, 8, WHITE);
      display.fillRect(10, 2, 4, 8, WHITE);
    } else if (pilep >= 5) {
      display.fillRect(25, 2, 4, 8, WHITE);
      display.fillRect(20, 2, 4, 8, WHITE);
      display.fillRect(15, 2, 4, 8, WHITE);
      display.fillRect(10, 2, 4, 8, WHITE);
      display.fillRect(5, 2, 4, 8, WHITE);
    }
    battpc = (batt - vmin_batt) * (100.0) / (vmax_batt - vmin_batt);
    if (battpc < 0) {
      battpc = 0;
    }
    if (battpc > 100) {
      battpc = 100;
    }
        if (battpc >=100) {
       display.setCursor(4, 21);
       display.setTextSize(1);
      display.println("Full");
    }else{
    display.setTextSize(2);
    if (battpc < 10) {
      display.setCursor(10, 18);
    } else {
      display.setCursor(2, 18);
    }
    display.print(battpc, 0);
    display.setTextSize(1);
    display.println("%");
    }
    //Speed
    display.setTextSize(2);
    display.setTextColor(WHITE);
    if (spee < 10) {
      display.setCursor(11, 102);

    } else {
      display.setCursor(5, 102);
    }
    if (spee>99){
      spee=99;
    }
    display.println(spee, 0);
    display.setTextSize(1);

    display.setCursor(5, 118);
    display.println("km/h");
    display.drawRect(0, 97, 32, 31, WHITE);


////    if (millis() % 8000 <= 4000) {
//      //Distance
//      display.setTextSize(1);
//      display.setCursor(3, 81);
//      display.println("Vmoy");
//      if (  meanspee < 10) {
//        display.setCursor(3, 91);
//
//      } else {
//        display.setCursor(0, 91);
//
//      }
//      display.println(meanspee, 2);
//      display.drawLine(1, 104, 27, 104, WHITE);
//      //Vmax
//      display.setTextSize(1);
//      display.setCursor(3, 109);
//      display.println("Vmax");
//      if ( maxspee < 10) {
//        display.setCursor(3, 118);
//      } else {
//        display.setCursor(0, 118);
//      }
//      display.println(maxspee, 2);

//    } else {
//      //Power
//      display.setTextSize(1);
//      display.setCursor(0, 74);
//      display.println("Power");
//      if (power > 9999) {
//        power = 9999;
//      }
//      display.print(power, 0);
//      display.println("W");
//      display.drawLine(1, 91, 27, 91, WHITE);
//      //Intensite
//      display.setTextSize(1);
//      display.setCursor(0, 94);
//      display.println("Inten");
//      if (intmot_uart > 99) {
//        power = 99;
//      }
//      display.print(intmot_uart, 1);
//      display.println("A");
//      display.drawLine(1, 111, 27, 111, WHITE);
//      //Volt
//      display.setTextSize(1);
//      display.setCursor(0, 113);
//      display.println("Volt");
//      display.print(volt_uart, 1);
//      display.println("V");
//    }

  }

       //if ((millis() - t_screen) > 60000) {
       //display.clearDisplay();
  display.display();
  //t_screen = millis();
 // }
}
