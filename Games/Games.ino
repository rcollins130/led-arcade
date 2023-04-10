/*
 * Main methods of script
*/

#include "globals.h"

//Called on initial startup of arduino
void setup() {
  //Initialise LED Strip
  delay(1000); //sanity delay
  Serial.begin(9600);
  pinMode(JOYZ, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  //Initialise game settings
  lastGame = GameState::menu;
  game = GameState::menu;
  ticksSinceMoved = 0;

  //Blank pause to emphasize a restart
  clearStrip();
  FastLED.show();
  delay(500);

  //Takes user to menu
  menuSetup();
}

//This loop is called indefinitely
void loop() {
  // Always updates position of joystick
  updateJoy();
  if(game == GameState::menu || game == GameState::screensaver){
    if(ticksSinceMoved > SCREENSAVER_TIMEOUT){
       game = GameState::screensaver;
    }
    else{
      game = GameState::menu;
    }
  }

  // sets up new scene, if needed
  if(game != lastGame){
    transition();
    switch (game){
      case menu:
        menuSetup();
        break;
      case gunGame:
        gunSetup();
        break;
      case pongGame:
        pongSetup();
        break;
      case reactGame:
        reactSetup();
        break;
      case screensaver:
        screensaverSetup();
        break;
    }
    lastGame = game;
  }
  // exectues loops, depending on selected game
  else{
    switch (game){
      case menu:
        menuLoop();
        break;
      case gunGame:
        gunLoop();
        break;
      case pongGame:
        pongLoop();
        break;
      case reactGame:
        reactLoop();
        break;
      case screensaver:
        screensaverLoop();
        break;
    }
  }
  // draws led strip
  FastLED.show();
  delay(1000 / FPS );
}
