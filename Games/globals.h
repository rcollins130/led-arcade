//LIBRARY INCLUSIONS

/*
 * FastLED library by Daniel Garcia
 * http://fastled.io/
*/
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
#include <math.h>

//CONSTANTS
//Led Constants
#define DATA_PIN 6 //Led Data Pin
#define FPS 45 //Frames per second. 45 recommended for playing
#define BRIGHTNESS 100 //overall brightness, from 0-255
#define NUM_LEDS 144 //Number of LEDS on strip
#define SCREENSAVER_TIMEOUT 500 //ticks till screensaver appears

//Joystick IO Ports
#define JOYX 0 //analog
#define JOYY 1 //analog
#define JOYZ 2 //digital

//Joystick Orientation- see documentation
#define WIRE_DIRECTION 1

//STRUCTS & ENUMS
//Position of joystick
struct JoyStickPos{
  signed char x = 0;
  signed char y = 0;
  bool z = true; //true indicates open switch, false indicates closed switch
} joy;

//Current game being played
enum GameState{
  menu,
  gunGame,
  pongGame,
  reactGame,
  screensaver
} game;

//GLOBAL VARIABLES- used in multiple games
CRGBArray<NUM_LEDS> leds; //contains colors of all leds on strip
GameState lastGame; //Last gamestate
float difficulty; //difficulty value
unsigned long deathTime; //time of death of player in a game
bool joyHeld; //has the joystick been held down
unsigned long ticksSinceMoved;

int dot; //position of player dot
bool last_direction_isPositive; //last direction of dot
CHSV dotColor; //color of dot
CHSV trail[NUM_LEDS]; //array containing colors of trail particles

//HELPER FUNCTIONS
//clears the led strip
void clearStrip();

//clears the trail array
void clearTrail();

//updates position of joystick variables
void updateJoy();

//Adjusts analog data from joystick to a value between -1 and 1
int treatValue(int data);

//Activates transition animation
void transition();

//Updates trail of player dot
void updateTrail();

//Setup and Loop functions
void menuSetup();
void menuLoop();
void gunSetup();
void gunLoop();
void pongSetup();
void pongLoop();
void reactSetup();
void reactLoop();
void screensaverSetup();
void screensaverLoop();
