#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#define NUM_LEDS 144
#define DATA_PIN 6
#define JOYX 0
#define JOYY 1
#define JOYZ 2

struct JoystickPos{
  int x = 0;
  int y = 0;
  int z = 0;
};

struct JoystickPos joy;
CRGB leds[NUM_LEDS];
int dot = NUM_LEDS/2;
CHSV color = CHSV(0,255,255);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(JOYZ, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  clearStrip();
  leds[dot] = color;
}

void loop() {
  // put your main code here, to run repeatedly:
  updateJoy();

  color.h = (color.h + joy.x*5) % 255;
  setDotPos(dot+ (-joy.y));
  
  Serial.println(dot);
  delay(20);
}

void setDotPos(int pos){
  if (pos>=0 && pos<NUM_LEDS){
    leds[dot] = CRGB::Black;
    dot = pos;
    if(joy.z ==1){
      leds[dot] = color;
    }
    else{
      leds[dot] = CRGB:: White;
    }
  }
  FastLED.show();
}

void clearStrip(){
  for(int i=0;i<NUM_LEDS;i++){
      leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void updateJoy(){
  joy.x = treatValue(analogRead(JOYX));
  //delay(30);
  joy.y = treatValue(analogRead(JOYY));
  //delay(30);
  joy.z = digitalRead(JOYZ);
}

//Adjusts analog data from joystick
int treatValue(int data){
  return (data * 11 / 1024) - 5;
}
