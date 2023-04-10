#define JOYX 0
#define JOYY 1
#define JOYZ 2

struct JoystickPos{
  int x = 0;
  int y = 0;
  int z = 0;
};

struct JoystickPos joy;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(JOYZ, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateJoy();
  delay(300);
}

void updateJoy(){
  joy.x = treatValue(analogRead(JOYX));
  //delay(30);
  joy.y = treatValue(analogRead(JOYY));
  //delay(30);
  joy.z = digitalRead(JOYZ);

  Serial.print("(");
  Serial.print(joy.x);
  Serial.print(", ");
  Serial.print(joy.y);
  Serial.print(") ");
  Serial.println(joy.z);
}

//Adjusts analog data from joystick
int treatValue(int data){
  return (data * 11 / 1024) - 5;
}
