/*
* React: A game in which the player must press the joystick when the dot is
* over the base. The base shrinks with each hit.
*/

#define INITIAL_BASE_SIZE 40

int speed; //speed of dot, in leds per frame
int baseSize; //size of base

void reactSetup(){
  clearStrip();
  clearTrail();

  //initialise variables
  speed = int(difficulty) * 2;
  baseSize = INITIAL_BASE_SIZE;

  deathTime = 0;
  joyHeld = false;

  dot = 5;
  dotColor = CHSV(105,255,200);

  //draw base and dot, then pause
  drawBase(170, 170);
  leds[dot] = dotColor;
  FastLED.show();
  delay(1000);
}

void reactLoop(){

  //move dot to new location
  int newPos = dot;
  if(deathTime ==0){
    leds[dot] = CRGB::Black;
    newPos += speed;
    if(newPos <= 0){
      speed = -speed;
      newPos = 0;
      if (baseSize < NUM_LEDS - 10){
        baseSize++; //increase base size each time dot hits 0 wall
      }
    }
    if(newPos >= NUM_LEDS -1){
      speed = -speed;
      newPos = NUM_LEDS-1;
    }
  }
  //Update Trail
  if (USETRAIL) {
    //note - this loop is needed to fill in gaps if dot skips leds between frames
    for(int i=0; i <= abs(dot - newPos); i++){
      int j = dot<newPos ? dot+i:newPos+i;
      trail[j] = CHSV(dotColor.h, 255, 100);
    }
    updateTrail();
  }

  //update dot position
  dot = newPos;

  //Check button
  if(joy.z){
    joyHeld = false;
  }
  if(!joy.z && !joyHeld && baseSize > 1){
    joyHeld = true;
    int shift = 0;
    if(baseSize%2 == 1) shift = 1;
    // If player hit base, reduce it's size
    if(dot >= NUM_LEDS/2 - baseSize/2 - shift &&
       dot <= NUM_LEDS/2 + baseSize/2){
         drawBase(170, 0);
         baseSize -= 2;
         // Player has won when base size is 0 or 1
         if(baseSize < 2){
           deathTime = millis();
         }
       }
    // If player missed base, start the death sequence
    else{
      deathTime = millis();
    }
  }

  //If player has won, draw strip as green, wait 2 seconds
  if (baseSize < 2 && millis() - deathTime < 2000){
    drawBase(105, 170);
    leds[dot] = dotColor;
  }
  //If player is still in play, draw strip as blue
  else if(deathTime == 0){
    //draw dot and base
    drawBase(170, 170);
    leds[dot] = dotColor;
  }
  //If player has died, draw strip & dot as red, wait 2 seconds
  else if( millis() - deathTime < 2000){
    drawBase(0, 170);
    dotColor = CHSV(0,255,170);
    leds[dot] = CRGB::Red;
  }
  //After 2 second wait, go to menu
  else{
    game = GameState::menu;
  }
}

//Draw the base with the given hue and brightness
void drawBase(int hue, int value){
  int shift = 0;
  if(baseSize%2 == 1) shift = 1;
  leds(NUM_LEDS/2 - baseSize/2 - shift, NUM_LEDS/2 + baseSize/2) = CHSV(hue,255,value);
  //If red or green (indicating win or loss) base blinks
  if(hue == 0 || hue == 105){
    leds(NUM_LEDS/2 - baseSize/2 - shift, NUM_LEDS/2 + baseSize/2) %= (sin8((millis()/3)) / 2) + 128;
  }
}
