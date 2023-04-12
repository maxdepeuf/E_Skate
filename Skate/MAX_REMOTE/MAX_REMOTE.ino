#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include<SoftwareSerial.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

SoftwareSerial ble(10, 11);

double speed_uart = 0;
double volt_uart = 0;
double int_uart = 0;
double intmot_uart = 0;

double const_speed = ((60.0 * 0.083 * 3.17) / 2.13) / 7000.0;

double power = 0;
double spee = 0;
double maxspee = 0;
double maxpower = 0;
double meanspee = 0;
double sumspee = 0;
int Pile_Pin = A1;
int Pot_Pin = A0;
double Pile_Value = 0;
int Pot_Value = 0;
int Pot;
const int nech=10;
int ech[nech];
int ind=0;
int moy=0;
int tot=0;


String data = "";
String stringGaz = "";
String command = "";

long t;

long t_co;

long tempstot;
long tinit;

double distance;

double vmin_batt = 33.6;
double vmax_batt = 40.0;

double vmin_pile = 2.96;
double vmax_pile = 4.2;

double v_bat = 0;
double p_bat = 0;

double v_pile = 0;
double v_pile_volt = 0;

double i = 1;

int State_Bat = 0;

void setup() {
  Serial.begin(115200);
  ble.begin(19200);

  pinMode(Pot_Pin, INPUT);
  pinMode(Pile_Pin, INPUT);

 for (int i = 0; i < nech; i++) {
    ech[i] = 0;
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  testscrolltext();
  display.clearDisplay();
  stringGaz = String();

  if (Serial.available() <= 0) {
    ble.println('A');
  }
  tinit = millis();
}

void loop() {
  ble.flush();
  if (ble.available() > 0) {
    while (ble.available()) {
      char inChar = (char)ble.read();
      if (inChar == '\n') {
        //Serial.println(command);
        data = command;
        command = "";
        t_co = millis();
      } else {
        command += inChar;
      }
    }
 if (int_uart != 0) {
    ble.println(stringGaz);
 }else{
  ble.println(String("0"));
 }
  }

 // Serial.println(data);
  if ((millis() - t_co) > 600) {
    volt_uart = 0;
  } else {
    
    
    speed_uart = ((data.substring(0, data.indexOf(','))).toDouble());
    speed_uart = abs(speed_uart);
    volt_uart = (data.substring(data.indexOf(',') + 1, data.indexOf(',', data.indexOf(',') + 1))).toDouble();
    int_uart = (data.substring((data.indexOf(',', data.indexOf(',') + 1)) + 1, data.indexOf(',', data.indexOf(',', data.indexOf(',') + 1) + 1))).toDouble();
    intmot_uart = (data.substring(data.indexOf(',', data.indexOf(',', data.indexOf(',') + 1) + 1) + 1, data.lastIndexOf(','))).toDouble();
    intmot_uart=abs(intmot_uart);
   // Serial.println(volt_uart);
    if (int_uart == 0) {
      volt_uart = volt_uart + 1.8;
    }
  }
  Pile_Value = analogRead(Pile_Pin);
  Pot_Value = analogRead(Pot_Pin);
  tot=tot-ech[ind];  
  Pot=map(Pot_Value, 0, 200, 63, 191);
         if (Pot<63) {
      Pot=63;
    }
      if (Pot>191) {
      Pot=191;
    }
  //Pot=map(Pot, 0, 200, 0, 255);
  ech[ind] = Pot;
 

    Serial.print(Pot);
  tot = tot + ech[ind];
  ind++;
  if (ind >= nech) {
    ind = 0;
  }
  moy=tot/nech;
  Serial.print(",");
  Serial.println(moy);
  stringGaz = String(Pot);
  //Serial.println(stringGaz);


  //Batterie
  v_pile_volt=(double(Pile_Value) - 0.0) * (5.0 -0.0) / (1023.0 - 0.0) + 0.0;
  v_pile=(v_pile_volt - vmin_pile) * (5.0 - 0.0) / (vmax_pile - vmin_pile) + 0.0;
  
  //v_pile = map(v_pile_volt, vmin_pile, vmax_pile, 0.0, 5.0);
  if (v_pile > vmax_pile) {
    v_bat = 5;
  } else if (v_pile < vmin_pile) {
    v_pile = 0;
  }

  //Serial.println(v_pile);
  display.drawRect(4 * (display.width() / 5) + 4, 0, display.width() / 5 - 2, display.height() / 3, WHITE);
  display.drawRect(4 * (display.width() / 5) + 2, 2, 3, 6, WHITE);
  if (v_pile <= 1) {
  } else if ((v_pile > 1)&&(v_pile <= 2)) {
    display.fillRect(4 * (display.width() / 5) + 5 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
  } else if ((v_pile > 2)&& (v_pile <= 3)){
    display.fillRect(4 * (display.width() / 5) + 5 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect(4 * (display.width() / 5) + 4 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
  } else if ((v_pile > 3)&& (v_pile <= 4)) {
    display.fillRect(4 * (display.width() / 5) + 5 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect(4 * (display.width() / 5) + 4 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect((4 * (display.width() / 5) + 3 * ((display.width() / 5) - 4) / 5) - 1, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
  } else if (v_pile > 4) {
    display.fillRect(4 * (display.width() / 5) + 5 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect(4 * (display.width() / 5) + 4 * ((display.width() / 5) - 4) / 5, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect((4 * (display.width() / 5) + 3 * ((display.width() / 5) - 4) / 5) - 1, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
    display.fillRect((4 * (display.width() / 5) + 2 * ((display.width() / 5) - 4) / 5) - 2, 2, ((display.width() / 5) - 4) / 5, (display.height() / 3) - 4, WHITE);
  }


  if (volt_uart <= 10) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 5);
    display.println("Please Wait");
    display.setCursor(0, 15);
    display.println("SK8 Not Connect");
  } else {
    //BatterieSkate
    display.drawRect(2, 0, display.width() / 5, display.height() / 3, WHITE);
    display.drawRect(0, 2, 3, 6, WHITE);
    //v_bat = map(volt_uart, vmin_batt, vmax_batt, 21.0, 0.0);
     v_bat=(volt_uart - vmin_batt) * (0.0 - 21.0) / (vmax_batt - vmin_batt) + 21.0;
  
    //p_bat = map(volt_uart, vmin_batt, vmax_batt, 0.0, 100.0);
p_bat=(volt_uart - vmin_batt) * (100.0- 0.0) / (vmax_batt - vmin_batt) + 0.0;
  
    if (p_bat < 0) {
      p_bat = 0;
    } else if (p_bat > 100) {
      p_bat = 100;
    }
    if (v_bat > 21) {
      v_bat = 21;
    } else if (v_bat < 0) {
      v_bat = 0;
    }

    display.fillRect(4 + v_bat, 2, (display.width() / 5) - 4 - v_bat, (display.height() / 3) - 4, WHITE);

    if (int_uart == 0) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(50, 10);
      display.print(p_bat, 0);
      display.print("%");

    } else {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor((display.width() / 5) + 4, 1);
      display.print(p_bat, 0);
      display.print("%");

      spee = (speed_uart * const_speed);
      maxspee = max(maxspee, spee);
      power = volt_uart * intmot_uart;
      sumspee = (sumspee + spee);
      meanspee = sumspee / i;
      i++;
      tempstot = millis() - tinit;
      distance = meanspee * 0.278 * tempstot * 0.001 * 0.001;
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(104, 15);
      display.println(distance, 1);
      display.setCursor(109, 25);
      display.println("km");
      //Vitesse
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(2, 16);
      display.print(spee, 0);
      display.setTextSize(1);
      display.println("km/h");

      if (millis() % 6000 <= 2000) {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(54, 9);
        display.println("  Batt");
        display.setCursor(63, 19);
        display.print(volt_uart, 1);
        display.println("V");

      } else if (millis() % 6000 <= 4000) {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(56, 2);
        display.println("  Max");
        display.setCursor(55, 12);
        display.println(" Speed");
        display.setCursor(60, 22);
        display.print(maxspee, 0);
        display.println("km/h");
      } else {
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(55, 12);
        display.println(" Power");
        display.setCursor(60, 22);
        display.print(power, 0);
        display.println("W");
      }
    }
  }
  display.display();
  display.clearDisplay();

};


void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.clearDisplay();
  display.println("Elec SK8");
  display.display();
  delay(1);
  display.startscrollright(0x00, 0x02);
  delay(3000);
  display.stopscroll();
}
