#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <MAX6675.h>
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset

int CS = 2;               // CS pin on MAX6675
int SO = 3;               // SO pin of MAX6675
int SCK1 = 4;             // SCK pin of MAX6675

int trigerPin = A0;      //пин реле

int units = 1;            // Units to readout temp (0 = raw, 1 = ˚C, 2 = ˚F)

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
MAX6675 temp(CS,SO,SCK1,units);

//style
const int backGroundColor = ST7735_BLACK;
const int labelColor = ST7735_GREEN;
const int valueColor = ST7735_YELLOW;
const int editedValueColor = ST7735_BLUE;
const int upTempColor = ST7735_BLUE;
const int downTempColor = ST7735_RED;

//VARIABLES
int modeState = 0;
boolean warmState = true;
boolean needDrawScreen = true;
float temperature = 0.0;     // Temperature output variable
float maxTemperature = 0.0;  // max Temperature output variable
int rate = 1;
int buttonStartState;
int buttonPluStartState;
int buttonPlusState;
int buttonMinusState;

//BUTTONS
const int buttonPinStart = 5;  // переключение старт/редактирование
const int buttonPinPlus = 6;   // Увеличиваем значение
const int buttonPinMinus = 7;  // Уменьшаем значение

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_REDTAB);

  pinMode(buttonPinStart, INPUT);
  pinMode(buttonPinPlus, INPUT);
  pinMode(trigerPin, OUTPUT);  
}

void loop() {
  int startState = digitalRead(buttonPinStart);
  buttonPlusState = digitalRead(buttonPinPlus);
  buttonMinusState = digitalRead(buttonPinMinus);

  if(startState!=buttonStartState){
    buttonStartState = startState;
    needDrawScreen = true;
  }

  if(buttonStartState == HIGH){ 
    getTemp();
    delay(1000);
    needDrawScreen= true;
  }
  else{
    settingLogic();
  }

  if(maxTemperature>temperature){
    digitalWrite(trigerPin, HIGH);
  }else{
  digitalWrite(trigerPin, LOW);
  }

  reloadScreen();
}

void  settingLogic(){
  if(buttonPlusState == HIGH){ 
    maxTemperature = maxTemperature + rate;
    needDrawScreen = true;
  }
  else if(buttonMinusState == HIGH){
    maxTemperature = maxTemperature - rate;
    needDrawScreen = true;
  }
}

void getTemp(){
  // Read the temp from the MAX6675
  temperature = temp.read_temp();

  if(temperature < 0) {                   
    Serial.print("Thermocouple Error on CS");
    Serial.println( temperature ); 
  } 
  else {
    Serial.print("Current Temperature: ");
    Serial.println( temperature ); 
  }
}
void reloadScreen(){
  if(needDrawScreen){
    if(buttonStartState == HIGH){ 
      drawWarmScreen();   
    }
    else{
      drawSettingScreen();
    }
    needDrawScreen = false;
  }
}

void drawWarmScreen(){
  //draw title ballon
  tft.fillScreen(backGroundColor); 
  tft.setTextSize(1); 
  tft.fillRoundRect(1, 1, 125, 20, 10, ST7735_GREEN);
  tft.setCursor(34, 8);
  tft.setTextColor(ST7735_BLUE);
  tft.print("WARM MODE!");

  //draw current temp area
  tft.fillRoundRect(1, 30, 80, 20, 10, ST7735_WHITE);
  tft.fillRoundRect(1, 40, 125, 50, 1, ST7735_WHITE);

  tft.setTextSize(1); 
  tft.setCursor(7, 32);
  tft.setTextColor(ST7735_BLUE);
  tft.print("CURRENT t(C)");

  tft.setTextSize(3); 
  tft.setCursor(7, 56);
  tft.setTextColor(ST7735_BLUE);
  tft.print(temperature);


  //draw info area
  tft.fillRoundRect(1, 100, 80, 20, 10, ST7735_WHITE);
  tft.fillRoundRect(1, 110, 125, 50, 1, ST7735_WHITE);

  tft.setTextSize(1); 
  tft.setCursor(7, 102);
  tft.setTextColor(ST7735_BLUE);
  tft.print("INFORMATION");
  tft.drawLine(70, 112, 70, 160, backGroundColor);

  tft.setCursor(7, 115);
  tft.setTextColor(backGroundColor);
  tft.print("MAX t(C)");

  tft.setCursor(80, 115);
  tft.setTextColor(backGroundColor);
  tft.print(maxTemperature);

  tft.setCursor(7, 130);
  tft.setTextColor(backGroundColor);
  tft.print("WARM STATE");

  tft.setCursor(80, 130);
  tft.setTextColor(backGroundColor);
  if(warmState){
    tft.print("ON");
  }
  else{
    tft.print("OFF");
  }

  tft.setCursor(7, 145);
  tft.setTextColor(backGroundColor);
  tft.print("MODE");

  tft.setCursor(80, 145);
  if(modeState==0){
    tft.print("HEATING");
  }
  else if(modeState==1){
    tft.print("WORKING");
  }
  else {
    tft.print("COOLING");
  }
}

void drawSettingScreen(){
  //draw title ballon
  tft.fillScreen(backGroundColor); 
  tft.setTextSize(1); 
  tft.fillRoundRect(1, 1, 125, 20, 10, ST7735_RED);
  tft.setCursor(34, 8);
  tft.setTextColor(ST7735_GREEN);
  tft.print("SETTING!");

  //draw current temp area
  tft.fillRoundRect(1, 30, 80, 20, 10, ST7735_WHITE);
  tft.fillRoundRect(1, 40, 125, 50, 1, ST7735_WHITE);

  tft.setTextSize(1); 
  tft.setCursor(7, 32);
  tft.setTextColor(ST7735_BLUE);
  tft.print("MAX t(C)");

  tft.setTextSize(3); 
  tft.setCursor(7, 56);
  tft.setTextColor(backGroundColor);
  tft.print(maxTemperature);

  //draw info area
  tft.fillRoundRect(1, 100, 80, 20, 10, ST7735_WHITE);
  tft.fillRoundRect(1, 110, 125, 50, 1, ST7735_WHITE);

  tft.setTextSize(1); 
  tft.setCursor(7, 102);
  tft.setTextColor(ST7735_BLUE);
  tft.print("INFORMATION");
  tft.drawLine(70, 112, 70, 160, backGroundColor);

  tft.setCursor(7, 115);
  tft.setTextColor(backGroundColor);
  tft.print("RATE");

  tft.setCursor(80, 115);
  tft.setTextColor(backGroundColor);
  tft.print(rate);
}















