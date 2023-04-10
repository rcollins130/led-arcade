/*
 * Menu- the menu where the player picks the next game to play
*/
//Local constants
#define NUM_GAMES 3
#define JOY_WAIT 150

int chosenLevel; //level currently selected
unsigned long moveTime; //time of last movement of selector

//strings dictating game icon patterns
char gunPatt[]  = "    r  b    y   ";
char pongPatt[] = "   b     b  rrr ";
char jumpPatt[] = "   g    bbbbbbb ";

//start and end of each level block
int games[NUM_GAMES][2] = {
  {0, (NUM_LEDS / 3)-1},
  {(NUM_LEDS / 3), (NUM_LEDS * 2/3)-1},
  {(NUM_LEDS* 2/3), NUM_LEDS-1}
};

void menuSetup(){
  clearStrip();
  drawStrip();

  chosenLevel = 0;
  moveTime = 0;
  difficulty = 1;
}

void menuLoop(){
  drawStrip();

  // Move selector
  if(joy.y == 0 && moveTime > 0){
    moveTime = 0;
  }
  if(joy.y !=0 && moveTime == 0){
    moveTime = millis();
  }
  if(joy.x < 0 && difficulty+0.1 <= 5){
    difficulty += 0.1;
  }
  if(joy.x > 0 && difficulty-0.1 >=1){
    difficulty -= 0.1;
  }
  if(millis() - moveTime > 200){
    if(joy.y < 0 && chosenLevel < NUM_GAMES-1){
      chosenLevel++;
    }
    if(joy.y > 0 && chosenLevel > 0){
      chosenLevel--;
    }
    moveTime = 0;
  }

  //Switch game to selected when joystick is pressed
  if(joy.z == 0){
    switch (chosenLevel){
      case 0:
      game = GameState::gunGame;
      break;
      case 1:
      game = GameState::pongGame;
      break;
      case 2:
      game = GameState::reactGame;
      break;
    }
  }

  //Highlight selected games
  leds(games[chosenLevel][0]+2, games[chosenLevel][0]+10 ) =
       CHSV( 105-(96/5 * difficulty) ,255,150);
  leds(games[chosenLevel][1]-10, games[chosenLevel][1]-2) =
       CHSV( 105-(96/5 * difficulty) ,255,150);
}

//Draw game sections and icon patterns
void drawStrip(){
  for(int i=0; i<NUM_GAMES; i++){
    leds(games[i][0]+2,games[i][1]-2) = CHSV(150, 255, 100);
  }

  drawPattern(gunPatt, 16, NUM_LEDS/6 - 8);
  drawPattern(pongPatt, 16, NUM_LEDS/2 -8);
  drawPattern(jumpPatt, 16, NUM_LEDS * 5/6 -8);
}

//Draw the icon pattern on the strip from a string
void drawPattern(char patt[], int len, int start){
  for(int i=0; i<len; i++){
    CRGB nextColor = CRGB::Black;
    if(patt[i]=='r') nextColor = CRGB::Red;
    else if(patt[i]=='g') nextColor = CRGB::Green;
    else if(patt[i]=='b') nextColor = CRGB::Blue;
    else if(patt[i]=='y') nextColor = CHSV(50, 255, 175); // yellow shade

    leds[start+i] = nextColor;
  }
}
