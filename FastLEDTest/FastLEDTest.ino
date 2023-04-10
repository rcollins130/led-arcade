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
#define PAUSE 0

CRGBArray<NUM_LEDS> leds;
CHSV color = CHSV(0,255,50);
double offset = double(255) / double(NUM_LEDS);

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //movingStrip();
  rainbow();
  //white();
}

void white(){
  leds(0,NUM_LEDS) = CRGB::White;
  FastLED.show();
}

void rainbow(){
  static uint8_t hue=0;
  leds(0,NUM_LEDS/2 - 1).fill_rainbow(hue++);
  leds(NUM_LEDS/2, NUM_LEDS-1) = leds(NUM_LEDS/2-1,0);
  leds.fadeToBlackBy(144);
  FastLED.show();

  delay(PAUSE);
}

void movingStrip(){
  color.hue = 0;
  leds[0] = color;
  FastLED.show();
  for(int n=1; n<NUM_LEDS; n++){
    delay(PAUSE);
    color.hue = n * offset ;
    leds[n] = color;
    Serial.println(color.hue);
    FastLED.show();
  }
  for(int n=NUM_LEDS-1; n>=0; n--){
    delay(PAUSE);
    leds[n]=CRGB::Black;
    //leds[n-1]=CRGB::White;
    FastLED.show();
  }
}
