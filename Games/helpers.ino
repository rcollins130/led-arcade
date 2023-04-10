
//HELPER FUNCTIONS
//clears the led strip of all colors
void clearStrip() {
  leds(0,NUM_LEDS) = CRGB::Black;
  FastLED.show();
}

//clears the trail array
void clearTrail(){
  for(int i=0; i<NUM_LEDS; i++){
    trail[i] = CHSV(0,0,0);
  }
}

//updates position of joystick variables, dependant on orientation of joystick
// See technical documentation for WIRE_DIRECTION notes
void updateJoy() {
  JoyStickPos prevJoy = joy;
  if(WIRE_DIRECTION == 0){
    joy.x = treatValue(analogRead(JOYX));
    joy.y = treatValue(analogRead(JOYY));
  }
  else if(WIRE_DIRECTION == 1){
    joy.x = -treatValue(analogRead(JOYY));
    joy.y = treatValue(analogRead(JOYX));
  }
  else if(WIRE_DIRECTION == 2){
    joy.x = -treatValue(analogRead(JOYX));
    joy.y = -treatValue(analogRead(JOYY));
  }
  else if(WIRE_DIRECTION == 3){
    joy.x = treatValue(analogRead(JOYY));
    joy.y = -treatValue(analogRead(JOYX));
  }
  joy.z = digitalRead(JOYZ)==1;

  if(prevJoy.x == joy.x && prevJoy.y == joy.y && prevJoy.z == joy.z){
    ticksSinceMoved += 1;
  }
  else{
    ticksSinceMoved = 0;
  }
}

//Adjusts analog data from joystick to a value between -1 and 1
int treatValue(int data) {
  return (data * 3 / 1024) - 1;
}

//immeditely activates transition animation
void transition(){
  leds(0,NUM_LEDS) = CRGB(0,0,150);
  FastLED.show();
  for(int i=0; i<125; i++){
    for(int j=0; j<NUM_LEDS; j++){
      leds[j].fadeToBlackBy(random8(16));
    }
    FastLED.show();
    delay(10);
  }
}

//updates the trail of the player dot
//Trail slowly dissapears
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
