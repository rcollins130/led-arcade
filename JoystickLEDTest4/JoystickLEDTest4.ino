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
#define FPS 45
#define BRIGHTNESS 255

#define USETRAIL true
#define USEGUN false
#define FLASH false
#define GUNRANGE 200
#define NUMGUNS 8
#define GUNPAUSE 750

#define USEENEMIES false
#define NUMENEMIES 10
#define ENEMYPAUSE 1000

#define JOYX 0
#define JOYY 1
#define JOYZ 2

struct JoystickPos {
  int x = 0;
  int y = 0;
  int z = 0;
};

struct gunParticle {
  bool positiveDir = true;
  int life = 0;
  int loc = 0;
  CRGB color = CHSV(200, 255, 175);
};

struct enemy {
  bool positiveDir = true;
  int loc = 0;
  int life = 0;
  int speed = 5; // frames / led
  int frame = 0;
  CRGB color = CHSV(0, 255, 175);
};

struct JoystickPos joy;
CRGBArray<NUM_LEDS> leds;

int dot = NUM_LEDS / 2; //dot starts in middle
CHSV color = CHSV(200, 255, 175);

CHSV trail[NUM_LEDS];

struct gunParticle gun[NUMGUNS];
bool last_direction_isPositive = true;
int nextGun = 0;
unsigned long gunTime = 0;

struct enemy enemies[NUMENEMIES];
unsigned long lastEnemyTime = 0;
int nextEnemy = 0;

unsigned long deathTime = 0;

void setup() {
  // put your setup code here, to run once:
  delay(1500);
  Serial.begin(9600);
  pinMode(JOYZ, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  clearStrip();
  leds[dot] = color;
}

void loop() {
  // put your main code here, to run repeatedly:
  //Get joystick info
  updateJoy();

  if (deathTime == 0) {
    //Update dot position and color
    color.h = (color.h + joy.x * 2) % 255;
    int newPos = dot + (-joy.y);
    if (joy.y < 0) {
      last_direction_isPositive = true;
    }
    if (joy.y > 0) {
      last_direction_isPositive = false;
    }
    if (newPos >= 0 && newPos < NUM_LEDS) {
      leds[dot] = CRGB::Black;
      dot = newPos;
    }

    //Update Trail
    if (USETRAIL) {
      if (joy.z == 0 || true) {
        trail[dot] = CHSV(color.h, 255, 150);
      }
      updateTrail();
    }

    //Update Gun
    if (USEGUN) {
      if ( (joy.z == 0) && gun[nextGun].life <= 0 && gunTime + GUNPAUSE < millis()) {
        gun[nextGun].positiveDir = last_direction_isPositive;
        gun[nextGun].loc = dot;
        gun[nextGun].life = GUNRANGE / 2;
        gun[nextGun].color = color;
        nextGun = (nextGun + 1) % NUMGUNS;
        gunTime = millis();
      }
      updateGuns();
    }

    //Update Enemies
    if (USEENEMIES) {
      if (lastEnemyTime + ENEMYPAUSE < millis() && enemies[nextEnemy].life == 0) {
        enemies[nextEnemy] = enemy();
        enemies[nextEnemy].life = 1;
        if (nextEnemy % (2) == 0) {
          enemies[nextEnemy].positiveDir = false;
          enemies[nextEnemy].loc = NUM_LEDS - 1;
        }
        lastEnemyTime = millis();
        nextEnemy = (nextEnemy + 1) % NUMENEMIES;
      }
      updateEnemies();
    }

    //Draw dot
    if (joy.z == 1) {
      leds[dot] = color;
      if (FLASH) leds[dot] %= (sin8(millis() / 2) / 2) + 128;
    }
    else {
      leds[dot] = color + CHSV(0, 0, 100);
    }
  }
  else {
    if (millis() - deathTime < 50) {
      leds(0, NUM_LEDS - 1) = CHSV(0, 255, 0);
      if (dot < 5) int origin = 5;
      leds(dot - 5, dot + 5) = CRGB(150, 0, 0);
    }
    else {
      for (int i = dot - 5; i <= dot + 5; i++) {
        if (leds[i].r > 0) {
          leds[i].r -= random8() /16;
        }
      }
    }
  }
  FastLED.show();
  delay(1000 / FPS );
}

void updateEnemies() {
  for (int i = 0; i < NUMENEMIES; i++) {
    enemies[i].frame++;
    leds[enemies[i].loc] = CRGB::Black;
    if (enemies[i].life > 0 && (enemies[i].frame % enemies[i].speed == 0)) {
      //move enemy
      if (enemies[i].positiveDir && enemies[i].loc + 1 < NUM_LEDS) {
        enemies[i].loc ++;
      }
      else if (!enemies[i].positiveDir && enemies[i].loc - 1 >= 0) {
        enemies[i].loc --;
      }
      else {
        enemies[i].loc = NUM_LEDS - 1;
        enemies[i].life = 0;
      }
    }
    if (enemies[i].life > 0) {
      leds[enemies[i].loc] = enemies[i].color;
    }
    for (int j = 0; j < NUMGUNS; j++) {
      if (gun[j].life > 0 && abs(gun[j].loc - enemies[i].loc) < 2) {
        leds[enemies[i].loc] = CRGB::Black;
        leds[gun[j].loc] = CRGB::Black;
        enemies[i] = enemy();
        gun[j].life = 0;
      }
    }
    if ( abs(enemies[i].loc - dot) < 1.5) {
      deathTime = millis();
    }
  }
}

void updateGuns() {
  for (int i = 0; i < NUMGUNS; i++) {
    //remove gun
    leds[gun[i].loc] = CRGB::Black;

    if (gun[i].life > 0) {
      //move gun particle, if possible
      if (gun[i].positiveDir && gun[i].loc + 2 < NUM_LEDS) {
        gun[i].loc += 2;
        gun[i].life--;
      }
      else if (!gun[i].positiveDir && gun[i].loc - 2 >= 0) {
        gun[i].loc -= 2;
        gun[i].life--;
      }
      else {
        gun[i].loc = NUM_LEDS - 1;
        gun[i].life = 0;
      }
    }
    //color gun location, if neccessary
    if (gun[i].life > 0) {
      leds[gun[i].loc] = gun[i].color;
    }
  }
}

void updateTrail() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (trail[i].v > 0) {
      int minus = random8() / 8;
      if (trail[i].v - minus > 0) {
        trail[i].v -= minus;
      }
      else {
        trail[i].v = 0;
      }
      leds[i] = trail[i];
    }
  }
}

void clearStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void updateJoy() {
  joy.x = treatValue(analogRead(JOYX));
  joy.y = treatValue(analogRead(JOYY));
  joy.z = digitalRead(JOYZ);
}

//Adjusts analog data from joystick to a value between -1 and 1
int treatValue(int data) {
  return (data * 3 / 1024) - 1;
}
