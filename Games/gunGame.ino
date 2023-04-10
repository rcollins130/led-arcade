/*
* Gungame: A shooter game played with a single dot, which shoots
* other dots at enemies
*/

//CONSTANTS
//Stylistic
#define USETRAIL true
#define FLASH true

//Gun
#define GUNRANGE 200
#define NUMGUNS 8
#define GUNPAUSE 200

//Enemies
#define NUMENEMIES 10

//STRUCTS
//Describes charactaristics of a "gun" particle
struct gunParticle {
  bool positiveDir = true; //direction of movement
  int life = 0; //how many pixels it has moved
  int loc = 0; //location on strip
  CRGB color = CHSV(170, 255, 175); //color
};

//Describes charactaristics of an "enemy" particle
struct enemy {
  bool positiveDir = true; //direction
  int loc = 0; //location on strip
  int life = 0; //hits needed to "kill" enemy. 0 indicates dead
  int speed = 2; // frames / led. Note that this is an inverse of speed
  int frame = 0; //frames it's been around for
  bool blink = false; //does it blink or not
  CRGB color = CHSV(0, 255, 175); //color
};

struct gunParticle gun[NUMGUNS]; //array cointaining gun particles
int nextGun; //position of next avaliable gun particle in array
unsigned long gunTime; //time since last firing of gun

struct enemy enemies[NUMENEMIES]; //array containing enemy particles
unsigned long lastEnemyTime; //time of last enemy appearance
int nextEnemy; //next avaliable enemy in array
int enemyPause; //time to pause between enemies

//note - difficulty rating is a global defined in menu
int score; //current number of enemies killed
int origin; //spot of death

void gunSetup(){
  clearStrip();
  clearTrail();

  //initialise variables
  dot = NUM_LEDS /2;
  dotColor = CHSV(170, 255, 150);
  last_direction_isPositive = true;
  nextGun = 0;
  gunTime = 0;
  joyHeld = false;

  for(int i=0; i<NUMENEMIES; i++){
    enemies[i] = enemy();
  }
  for(int i=0; i<NUM_LEDS; i++){
    trail[i] = CHSV(0,0,0);
  }

  lastEnemyTime = millis(); //provides a pause before enemies appear
  nextEnemy = 0;
  enemyPause = 2000;
  deathTime = 0;
  score = 0;
  origin = 20;
}

//loops when GunGame is being played
void gunLoop(){
  //Update Trail
  if (USETRAIL && deathTime ==0) {
    trail[dot] = CHSV(dotColor.h, 255, 150);
    updateTrail();
  }

  //fires new gun particle if conditions are met
  if ((!joy.z) &&
  !joyHeld &&
  gun[nextGun].life <= 0 &&
  gunTime + GUNPAUSE < millis() &&
  deathTime ==0 ) {
    joyHeld = true;
    gun[nextGun].positiveDir = last_direction_isPositive;
    gun[nextGun].loc = dot;
    gun[nextGun].life = GUNRANGE / 2;
    gun[nextGun].color = dotColor;
    nextGun = (nextGun + 1) % NUMGUNS;
    gunTime = millis();
  }
  //update position of gun and enemy particles
  updateGuns();

  //Update Enemies
  // add a new enemy
  if (lastEnemyTime + enemyPause < millis() && enemies[nextEnemy].life <= 0) {
    enemies[nextEnemy] = enemy();
    enemies[nextEnemy].life = 1;
    if (random8() > 128) {
      enemies[nextEnemy].positiveDir = false;
      enemies[nextEnemy].loc = NUM_LEDS - 1;
    }
    //randomly place special enemies
    //chance of enemies appearing is based on score and difficulty
    int chance = (32 + score*difficulty);
    if (score>10 && random8() < (chance>192 ? 192:chance)){
      if(random8() > 96){
        enemies[nextEnemy].speed /= 2;
        enemies[nextEnemy].blink = true;
      }
      else if(random8() > 96){
        enemies[nextEnemy].speed *= 2;
        enemies[nextEnemy].life = 3;
        enemies[nextEnemy].color = CHSV(25, 255, 175);
      }
      else{
        enemies[nextEnemy].speed *= 3;
        enemies[nextEnemy].life = 5;
        enemies[nextEnemy].color = CHSV(50, 255, 175);
      }
    }
    lastEnemyTime = millis();
    nextEnemy = (nextEnemy + 1) % NUMENEMIES;

    //decrease enemy waiting time for every 10 enemies killed
    if(nextEnemy%10 == 0 && enemyPause > 300-difficulty*25){
      enemyPause /= 1+ 0.1*difficulty;
    }
  }
  updateEnemies();

  // if player is still alive, let him move
  if (deathTime == 0) {
    //Update dot position
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

    //Update Gun
    if(joy.z){
      joyHeld = false;
    }

    //Draw dot
    if (joy.z) {
      leds[dot] = dotColor;
      if (FLASH) leds[dot] %= (sin8( (millis() / 20) * score/20) / 2) + 128;
    }
    else {
      leds[dot] = dotColor + CHSV(0, 0, 100);
    }
  }
  // if the player is dead, do death animation
  else {
    if (millis() - deathTime < 50) {
      origin = dot;
      if (dot < 5) origin = 5;
      if (dot > NUM_LEDS-6) origin = NUM_LEDS-6;
      leds(origin - 5, origin + 5) = CRGB(150, 0, 0);
    }
    else if (millis() - deathTime < 5000){
      for (int i = origin - 5; i <= origin + 5; i++) {
        if (leds[i].r > 0) {
          leds[i].r -= random8() / 8;
        }
      }
    }
    //exit to menu after 5 seconds
    else{
      game = GameState::menu;
    }
  }

}

//Updates positions of enemies
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
    //determine if enemies have been hit by gun particle
    if(enemies[i].life > 0){
      for (int j = 0; j < NUMGUNS; j++) {
        if (gun[j].life > 0 && abs(gun[j].loc - enemies[i].loc) < 2) {
          leds[gun[j].loc] = CRGB::White;
          gun[j].life = 0;
          enemies[i].life -= 1;
          if(enemies[i].life == 0){
            enemies[i] = enemy();
            score += 1;

            //brighten dot every 20th kill
            if(score % 20 == 0 && dotColor.v <= 250){
              dotColor.v += 5;
              dotColor.s -= 2;
            }

          }
        }
      }
    }
    //determine if enemy has hit player dot
    if ( abs(enemies[i].loc - dot) < 1.5 && deathTime == 0) {
      deathTime = millis();
    }
    //draw enemies
    if (enemies[i].life > 0) {
      leds[enemies[i].loc] = enemies[i].color;
      if(enemies[i].blink){
        leds[enemies[i].loc] = enemies[i].color;
        leds[enemies[i].loc] %= ((sin8(millis())/1.5)+80);
      }
    }
  }
}

//update gun particles
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
    //draw gun location, if neccessary
    if (gun[i].life > 0) {
      leds[gun[i].loc] = gun[i].color;
    }
  }
}
