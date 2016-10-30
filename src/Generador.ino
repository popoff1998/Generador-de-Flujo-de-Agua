#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ClickEncoder.h"
#include <TimerOne.h>
//#include <TimerThree.h>
#include <Time.h>

//LIMITES
#define MAXVALUE 120
#define MINVALUE 0
#define PULSESFORLITER 450
#define PULSEWIDTH 10
#define TIMER1PERIOD 100

//Para el rotary
const int PinCLK=2;                   // Used for generating interrupts using CLK signal
const int PinDT=3;                    // Used for reading DT signal
const int PinSW=4;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define OUTPIN 53

ClickEncoder *encoder;
int16_t last, value;
unsigned long   waveTimer=99999999L;
unsigned long   timerCicles = 0;
int lastPulse;
bool doNone=false;

void doPulse()
{
//  int lapso;
//  cli();
//  lapso=now()-lastPulse;
//  lastPulse=now();
//  Serial.print("LAPSO: ");Serial.println(lapso);
  if (doNone) return;
  digitalWrite(13, LOW);
  digitalWrite(OUTPIN,HIGH);
  delayMicroseconds(PULSEWIDTH);
  digitalWrite(13, HIGH);
  digitalWrite(OUTPIN,LOW);
//  sei();
}

void timerIsr() {
  encoder->service();
  timerCicles++;
  if (timerCicles >= waveTimer )
  {
    timerCicles=0;
    doPulse();
  }
}


void setup()
{
  Serial.begin(115200);
  //Ponemos los pin en salida
  pinMode(13,OUTPUT);
  pinMode(OUTPIN,OUTPUT);
  Serial.println("Hola");
//Para el DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.clearDisplay();
//Para el encoder
  encoder = new ClickEncoder(PinCLK,PinDT,PinSW);
  Timer1.initialize(TIMER1PERIOD);
  Timer1.attachInterrupt(timerIsr);
//  Timer3.initialize(1000000);
//  Timer3.attachInterrupt(doPulse);
  last = -1;
}

void loop()
{
  value += encoder->getValue();
  //Comprobamos los limites
  if (value<MINVALUE) value=MINVALUE;
  if (value>MAXVALUE) value=MAXVALUE;

  if (value != last) {
    last = value;
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(value);
    if(doNone) display.println("*");
    display.display();
    Serial.print("Encoder Value: ");
    Serial.println(value);
    //value son l/min de momento entero
    long pulsosporsegundo=(PULSESFORLITER*(long)value)/60L;
    waveTimer = 10000L/pulsosporsegundo;
  }
  ClickEncoder::Button b = encoder->getButton();
  if (b == ClickEncoder::Clicked)
  {
    doNone = !doNone;
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(value);
    if(doNone) display.println("*");
    display.display();
  }
}
