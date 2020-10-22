
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// OLED PIN
#define OLED_MOSI   PA1
#define OLED_CLK   PA0
#define OLED_DC    PA3
#define OLED_CS    PA3
#define OLED_RESET PA2

// MOTOR PIN
# define X_ENgnd  PA5 //ENA-(ENA) stepper motor enable , active low     Gray 
# define X_EN_5v  PA4 //ENA+(+5V) stepper motor enable , active low     Orange
# define X_DIRgnd PA9 //DIR-(DIR) axis stepper motor direction control  Blue
# define X_DIR_5v PA8 //DIR+(+5v) axis stepper motor direction control  Brown
# define X_STPgnd PA7 //PUL-(PUL) axis stepper motor step control       Black
# define X_STP_5v PA6 //PUL+(+5v) axis stepper motor step control       RED


// KEY PIN

#define UP PB15
#define DOWN PB7
#define NEXT PB8
#define OK PB9


#define STOP PB12 //have interupt
#define START PB13
#define RESET PB14

// MICRO SWITCH
#define SW1 PB4
#define SW2 PB6
#define SW3 PB5

// states
#define ANGLE_STATE 2
#define SPEED_STATE 1
#define REPEAT_STATE 3
//#define DIRECT_STATE 4
#define OK_STATE 4
#define FINISH_STATE 6
#define WORKING_STATE 7

#define RESET_STATE 9
#define START_STATE 10
#define STOP_STATE 11


#define CW_DIR 1
#define CCW_DIR 0
#define STEP_PER_ROUND 3200   //1/16 MODE
#define RESULOTIAN_ANGLE 0.1125  //360/3200
#define ANGLE_STEP 0.45
#define MAX_ANGLE 51
#define MIN_ANGLE -51
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define MAX_SPEED 1000 //DEGRE per SEC
#define MIN_SPEED 3    // DEGRE per SEC
#define speed_step 50


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

uint8_t state = 0;
int Dir = CW_DIR;
uint8_t repeat_num = 0;
uint16_t speed_deg_per_secound = 500;
float current_angle = 0;
float next_angle = 0;
bool up_presed = false;
bool down_presed = false;
bool next_presed = false;
bool ok_presed = false;
bool reset_presed = false;
bool start_presed = false;
bool stop_presed = false;
bool sw1_presed = false;
bool sw2_presed = false;
bool sw3_presed = false;
bool interupt_flage = false;
void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    while (1) {
      Serial.println(F("SSD1306 allocation failed"));
    }
  }
   
  
  display.clearDisplay();
  display.clearDisplay();
  Serial.println("LCD default");
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println(F("WELLCOME"));
  //      display.setCursor(20, 30);
  //      display.println(F("START"));

  display.display();

  init_motor();

  init_keys();

  init_switch();

}
void loop()
{

  if (!interupt_flage) {


    check_key();

    // display_handler();

    //    Serial.println(digitalRead(RESET));
  }
  else {

    //        delay(200);
    display_handler();
    interupt_flage = false;
  }
  
  key_handler();

}

void init_motor() {
  pinMode (X_ENgnd , OUTPUT); //ENA-(ENA)
  pinMode (X_EN_5v , OUTPUT); //ENA+(+5V)
  pinMode (X_DIRgnd, OUTPUT); //DIR-(DIR)
  pinMode (X_DIR_5v, OUTPUT); //DIR+(+5v)
  pinMode (X_STPgnd, OUTPUT); //PUL-(PUL)
  pinMode (X_STP_5v, OUTPUT); //PUL+(+5v)

  digitalWrite (X_ENgnd,  LOW); //ENA-(ENA)
  digitalWrite (X_EN_5v, LOW); //ENA+(+5V) low=enabled
  digitalWrite (X_DIRgnd, LOW); //DIR-(DIR)
  digitalWrite (X_DIR_5v, LOW); //DIR+(+5v)
  digitalWrite (X_STPgnd, LOW); //PUL-(PUL)
  digitalWrite (X_STP_5v, LOW); //PUL+(+5v)
}
void init_switch() {
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);


  attachInterrupt(digitalPinToInterrupt(SW1), sw1_press, FALLING);
  attachInterrupt(digitalPinToInterrupt(SW2), sw2_press, FALLING);
  attachInterrupt(digitalPinToInterrupt(SW3), sw3_press, FALLING);
}

void init_keys() {

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(NEXT, INPUT);
  pinMode(OK, INPUT);
  pinMode(RESET, INPUT);
  pinMode(START, INPUT);
  pinMode(STOP, INPUT);


  //  attachInterrupt(digitalPinToInterrupt(UP), up_press, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(DOWN), down_press, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(NEXT), next_press, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(OK), ok_press, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(RESET), reset_press, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(START), start_press, FALLING);
  attachInterrupt(digitalPinToInterrupt(STOP), stop_press, FALLING);


}

void check_key() {

  if (!digitalRead(UP)) {
    up_press();
  }
  if (!digitalRead(DOWN)) {
    down_press();
  }
  if (!digitalRead(NEXT)) {
    next_press();
  }
  if (!digitalRead(OK)) {
    ok_press();
  }

  if (!digitalRead(START)) {
    start_press();
  }

  if (!digitalRead(RESET)) {
    reset_press();
  }
}
void display_handler(void) {
  switch (state) {
    case START_STATE  :
      display.clearDisplay();
      Serial.println("LCD START_STATE");
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 20);
      display.println(F("PRESS"));
      display.println(F(""));
      display.println(F("START"));
      display.display();
      break;
    case WORKING_STATE :
      display.clearDisplay();
      Serial.println("LCD WORKING_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 30);
      display.println(F("WORKING"));
      display.display();
      break;
    case STOP_STATE :
      display.clearDisplay();
      Serial.println("LCD STOP_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_INVERSE);
      display.setCursor(20, 30);
      display.println(F("STOP"));
      display.display();
      delay(2000);
      state=OK_STATE;
      break;
    case RESET_STATE :
      display.clearDisplay();
      Serial.println("LCD RESET_STATE");
      display.setTextSize(2);
      display.setTextColor(SSD1306_INVERSE);
      display.setCursor(20, 30);
      display.println(F("RESETING"));
      display.display();
      break;


    case FINISH_STATE  :
      display.clearDisplay();
      Serial.println("LCD FINISH_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,30);
      display.println(F("FINISH"));
      display.display();
      break;
    case OK_STATE  :
      display.clearDisplay();
      Serial.println("LCD OK_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(20, 0);
      display.println(F("PRESS"));
      display.setCursor(20, 30);
      display.println(F("START"));
      display.display();
      break;
    case SPEED_STATE:
      display.clearDisplay();
      Serial.println("LCD SPEED_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(15, 0);
      display.println(F("SPEED"));
      display.setTextSize(2);

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(30, 40);
      display.print(speed_deg_per_secound);
      display.println(F(" D/S"));
      display.display();
      break;
    case ANGLE_STATE:
      display.clearDisplay();
      Serial.println("LCD ANGLE_STATE");
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(15, 0);
      display.println(F("ANGLE"));

      display.setTextSize(2);

      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 20);
      display.print(F("C_A:"));
      display.println(current_angle);
      display.setCursor(0, 40);
      display.print(F("N_A:"));
      display.println(next_angle);
      display.display();
      break;
    case REPEAT_STATE:
      display.clearDisplay();
      Serial.println("LCD REPEAT_STATE");
      display.setTextSize(3);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(15, 0);
      display.println(F("REAPET"));

      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(40, 40);
      display.println(repeat_num);
      display.display();
      break;
//    case DIRECT_STATE:
//      display.clearDisplay();
//      Serial.println("LCD DIRECT_STATE");
//      display.setTextSize(2);
//      display.setTextColor(SSD1306_WHITE);
//      display.setCursor(10, 0);
//      display.println(F("DIRECTION"));
//
//      display.setTextSize(2);
//
//      display.setTextColor(SSD1306_WHITE);
//      display.setCursor(40, 40);
//      if (Dir) {
//        display.println("CW");
//      }
//      else {
//        display.println("CCW");
//      }
//
//      display.display();
//      break;

  }



  //    delay(2000);
}
void key_handler() {


  if (stop_presed) {
    stop_presed = false;
    stop_working();
    state = STOP_STATE;
    Serial.println("stop_state");
    
  }
  else if (up_presed) {
    up_presed = false;
    Serial.println("up_press");
    switch (state) {
      case OK_STATE  :
        Serial.println(OK_STATE);
        break;
      case SPEED_STATE:
        Serial.println(SPEED_STATE);
        speed_deg_per_secound = speed_deg_per_secound + speed_step;
        if (speed_deg_per_secound > MAX_SPEED)speed_deg_per_secound = MAX_SPEED;
        Serial.println(speed_deg_per_secound);
        delay(100);
        break;
      case ANGLE_STATE:
        Serial.println(ANGLE_STATE);
        next_angle = next_angle + ANGLE_STEP;
        if (next_angle > MAX_ANGLE)next_angle = MAX_ANGLE;
        if (next_angle < MIN_ANGLE)next_angle = MIN_ANGLE;
        Serial.print("NX=");
        Serial.println(next_angle);
        delay(100);
        break;
      case REPEAT_STATE:
        Serial.println(REPEAT_STATE);
        repeat_num++;
        delay(300);
        break;
//      case DIRECT_STATE:
//        Serial.println(DIRECT_STATE);
//        if (Dir) {
//          Dir = CW_DIR;
//        }
//        else {
//          Dir = CCW_DIR;
//        }
//        delay(1000);
//        break;
    }

  }
  else if (down_presed) {
    down_presed = false;
    Serial.println("down_press");
    switch (state) {
      case OK_STATE  :
        Serial.println(OK_STATE);
        break;
      case SPEED_STATE:
        Serial.println(SPEED_STATE);
        speed_deg_per_secound = speed_deg_per_secound - speed_step;
        if (speed_deg_per_secound > MAX_SPEED)speed_deg_per_secound = MIN_SPEED;
        Serial.println(speed_deg_per_secound);
        delay(100);
        break;
      case ANGLE_STATE:
        Serial.println(ANGLE_STATE);
        next_angle = next_angle - ANGLE_STEP;
        if (next_angle > MAX_ANGLE)next_angle = MAX_ANGLE;
        if (next_angle < MIN_ANGLE)next_angle = MIN_ANGLE;
        Serial.print("NX=");
        Serial.println(next_angle);
        delay(100);
        break;
      case REPEAT_STATE:
        Serial.println(REPEAT_STATE);
        repeat_num--;
        delay(300);
        break;
//      case DIRECT_STATE:
//        Serial.println(DIRECT_STATE);
//        if (Dir) {
//          Dir = CW_DIR;
//        }
//        else {
//          Dir = CCW_DIR;
//        }
//        delay(500);
//        break;
    }


  }
  else if (next_presed) {
    next_presed = false;
    Serial.println("next_state");
    if (state < REPEAT_STATE) {
      state++;
    }
    else {
      state = SPEED_STATE;
    }
    state = state % OK_STATE;
    delay(300);
  }
  else if (ok_presed) {
    ok_presed = false;
    state = OK_STATE;
    Serial.println("ok_state");
  }
  else if (reset_presed) {
    reset_presed = false;
    state = RESET_STATE;
    reset_angle();
    Serial.println("reset_state");
  }
  else if (start_presed) {
    start_presed = false;
    start_working();
    state = START_STATE;
    Serial.println("WORKING");
  }
  else if (sw1_presed) {
    sw1_presed = false;
//    current_angle = 0; ///reset motor
//    state = OK_STATE;
    Serial.println("sw1_pressed(reset)");
  }

}

void stop_working() {
  Serial.println("STOP MOTOR");
  digitalWrite (X_EN_5v, HIGH);//ENA+(+5V) low=enabled
}

void start_working() {
  Serial.println("START MOTOR");
  digitalWrite (X_EN_5v, LOW);//ENA+(+5V) low=enabled
  state = WORKING_STATE;
  display_handler();
  MoveX();
  digitalWrite (X_EN_5v, HIGH);
  state = FINISH_STATE;
  display_handler();
  delay(3000);
}

void reset_angle() {
  Serial.println("rest  MOTOR");
  state = RESET_STATE;
  display_handler();
  digitalWrite (X_DIR_5v, Dir);
  digitalWrite (X_EN_5v, LOW);
  for (int i=0;i<600 &&digitalRead(SW1)&&digitalRead(SW2)&& digitalRead(SW3)&&digitalRead(X_EN_5v)==LOW;i++) { //rotate untile switched pressd
    digitalWrite (X_STP_5v, HIGH);
    delayMicroseconds (1000);
    digitalWrite (X_STP_5v, LOW);
    delayMicroseconds (1000);
  }
delay(500);
 next_angle=0;
 start_working();
   Serial.println("reset  finishd");
  state = STOP_STATE;
  display_handler();
   digitalWrite (X_EN_5v, HIGH);
}

void MoveX() { // **************************************************************    Move
  float xt;
  if (next_angle > current_angle)
  {

    Serial.println("CW");
    digitalWrite (X_DIR_5v, Dir);
    xt = RESULOTIAN_ANGLE ;
  }
  else
  {

    Serial.println("CCW");
    digitalWrite (X_DIR_5v, !Dir);
    xt = -RESULOTIAN_ANGLE;
  }

  int i = 0;

  int delay_step = 1000000 / speed_deg_per_secound; //microsecend_per_degree
  int partial_delay_step = delay_step / 10; //for 72 Mhz
  if (repeat_num == 0) {
    for (; abs(next_angle - current_angle) > RESULOTIAN_ANGLE && current_angle<MAX_ANGLE &&  current_angle>MIN_ANGLE ; current_angle = current_angle + xt)
    {
    
      digitalWrite (X_STP_5v, HIGH);
      delayMicroseconds (partial_delay_step);
      digitalWrite (X_STP_5v, LOW);
      delayMicroseconds (partial_delay_step);
    }
  }
  else {

    Serial.print("/next_angle=");
    Serial.println(next_angle);

    Serial.print("/current_angle=");
    Serial.println(current_angle);
    float margin = abs(next_angle - current_angle);

    Serial.print("margin=");
    Serial.println(margin);
    if (margin < RESULOTIAN_ANGLE) return;
    for (i = 0; i < repeat_num; i++) {
      next_angle =current_angle+ margin;
      for (; abs(next_angle - current_angle) > RESULOTIAN_ANGLE && digitalRead(X_EN_5v)==LOW &&current_angle<MAX_ANGLE &&  current_angle>MIN_ANGLE; current_angle = current_angle + xt)
      {
        digitalWrite (X_STP_5v, HIGH);
        delayMicroseconds (partial_delay_step);
        digitalWrite (X_STP_5v, LOW);
        delayMicroseconds (partial_delay_step);
      }
      digitalWrite(X_DIR_5v, !digitalRead(X_DIR_5v));
      next_angle =current_angle -2*margin;
      for (; abs(next_angle - current_angle) > RESULOTIAN_ANGLE && digitalRead(X_EN_5v)==LOW&&current_angle<MAX_ANGLE &&  current_angle>MIN_ANGLE; current_angle = current_angle - xt)
      {
        digitalWrite (X_STP_5v, HIGH);
        delayMicroseconds (partial_delay_step);
        digitalWrite (X_STP_5v, LOW);
        delayMicroseconds (partial_delay_step);
      }
      digitalWrite(X_DIR_5v, !digitalRead(X_DIR_5v));
      next_angle =current_angle + margin;
      for (; abs(next_angle - current_angle) > RESULOTIAN_ANGLE && digitalRead(X_EN_5v)==LOW&&current_angle<MAX_ANGLE &&  current_angle>MIN_ANGLE; current_angle = current_angle + xt)
      {
        digitalWrite (X_STP_5v, HIGH);
        delayMicroseconds (partial_delay_step);
        digitalWrite (X_STP_5v, LOW);
        delayMicroseconds (partial_delay_step);
      }
    }
  }
}



void bottom_pressed() {
  Serial.println("bottom_pressed");
}
void up_press() {
  Serial.println("//////////////up_press");
  up_presed = true;
  interupt_flage = true;
}
void down_press() {
  Serial.println("//////////////down_press");
  down_presed = true;
  interupt_flage = true;

}
void next_press() {
  Serial.println("//////////////next_press");
  next_presed = true;
  interupt_flage = true;

}

void ok_press() {
  Serial.println("//////////////ok_press");
  ok_presed = true;
  interupt_flage = true;
}
void reset_press() {
  Serial.println("//////////////reset_press");
  reset_presed = true;
  interupt_flage = true;
}
void start_press() {
  Serial.println("//////////////start_press");
  start_presed = true;
  interupt_flage = true;
}
void stop_press() {
  Serial.println("//////////////stop_press");
   Serial.println("STOP MOTOR");
    digitalWrite (X_EN_5v, HIGH);//ENA+(+5V) low=enabled
    state = STOP_STATE;
//  delay(300);
  stop_presed = true;
   interupt_flage = true;
  if (digitalRead(STOP) & (!interupt_flage)) {
//    interupt_flage = true;   
  }

}


void sw1_press() {
  Serial.println("//////////////sw1_press");
  current_angle=-50;
//  stop_working();
//  display_handler();
  
//  delay(300);
//  if (digitalRead(SW1) & (!interupt_flage)) {
//    sw1_presed = true;
//    interupt_flage = true;
////    stop_working();
//  }
}
void sw2_press() {
  Serial.println("//////////////sw2_press");
  current_angle=0;
//  display_handler();
}
void sw3_press() {
  Serial.println("//////////////sw3_press");
  current_angle=50;
//  display_handler();
}
