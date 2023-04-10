/*
* Pong: a 1-d, 1 player version of the original pong game, in which the player
* must match the color of the ball to hit it. The ball bounces off a wall,
* which is slowly destroyed.
*/

//Wall
#define WALLSIZE 10 //depth of wall
#define WALLLIFE 3 //hits taken to destroy each wall section
#define WALLLOC 108 //location of initial first wall section

//struct defining the ball
struct ball{
  int loc = NUM_LEDS/2; //location of ball
  unsigned long frame; //how many frames the ball has been in play
  float speed = 1; // speed, in leds / frame
  CHSV color = CHSV(170,255,170); //color of ball
} pongBall;

int wallFront; //current location of front of wall, relative to WALLOC
int wall[WALLSIZE]; //stores "health" of each wall section

void pongSetup(){
  clearStrip();
  clearTrail();

  //initialise variables
  dot = NUM_LEDS / 4;
  dotColor = CHSV(170,255,170);

  pongBall = ball();
  pongBall.speed = 1;

  wallFront = 0;
  for(int i=0; i<WALLSIZE; i++){
    wall[i] = WALLLIFE;
    leds[i+wallFront+WALLLOC] = CHSV(0,255,170);
  }

  leds[dot] = dotColor;
  leds[pongBall.loc] = pongBall.color;

  FastLED.show();
  delay(1000);
}

void pongLoop(){
  //Update dot position and color
  dotColor.h = (dotColor.h + joy.x * int(difficulty)*2 )%255;
  int newPos = dot + (-joy.y);
  if (newPos >= 0 && newPos < WALLLOC+wallFront) {
    leds[dot] = CRGB::Black;
    dot = newPos;
  }

  //Update Trail
  if (USETRAIL) {
    trail[dot] = CHSV(dotColor.h, 255, 150);
    updateTrail();
  }

  //Update wall and ball
  updateWall();
  updateBall();

  //Check if ball has passed player
  if(pongBall.loc <= 0 || pongBall.loc >= NUM_LEDS-1){
    game = GameState::menu;
  }

  //Draw dot
  leds[dot] = dotColor;
  if (FLASH) leds[dot] %= (sin8((millis()/5)) / 2) + 128;
}

void updateBall(){
  pongBall.frame++;
  leds[pongBall.loc] = CRGB::Black;

  //determine new location of ball
  int newPos = pongBall.loc;
  if(abs(pongBall.speed) > 1){
    newPos = pongBall.loc + pongBall.speed;
  }
  else if(pongBall.frame % int(3-(difficulty-1)/2) == 0){
    newPos = pongBall.loc + (pongBall.speed > 0 ? 1:-1);
  }

  //check for collision against wall
  if(newPos>=wallFront+WALLLOC && wall[wallFront]>0){
    newPos = wallFront+WALLLOC - 1;
    pongBall.speed = -pongBall.speed;
    pongBall.color.h = random8();
    wall[wallFront] -= 1;
  }
  //check for collision agaisnt dot
  if( newPos-dot <= 0 && newPos-dot > -2
      && colorsMatch(pongBall.color.h, dotColor.h, 50-difficulty*4)){
      newPos = dot + 1;
      pongBall.speed = -pongBall.speed;
  }

  //ensure ball does not go out of array bounds
  if(newPos < 0){
    newPos = 0;
  }
  //update ball position
  pongBall.loc = newPos;
  leds[pongBall.loc] = pongBall.color;
}

void updateWall(){
  //redraw wall
  for(int i=0; i<WALLSIZE; i++){
    leds[i+WALLLOC] = CHSV(0,255,(wall[i])*40);
  }
  //shift front of wall if previous front has been destroyed
  if(wall[wallFront]==0 && wallFront<WALLSIZE-1){
    wallFront++;
  }
}

//returns true if hue 1 and hue 2 are within tolerance of each other
bool colorsMatch(int h1, int h2, int tolerance){
  // angle comparison from
  // http://stackoverflow.com/questions/24943471/algorithm-for-testing-angle-equality
  return abs( (abs(h1-h2) + 128) % (256) - 128) <= tolerance;

}
