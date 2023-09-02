#include <Adafruit_TinyUSB.h>
#include <Wire.h>
#include <SPI.h>
#include <TMCStepper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "RP2040_PWM.h"
#include "LittleFS.h"

#include "UI_Tree.h"

#include "data_struct.h"


// Definice callback funkce pro zobrazení
void displayPage1() {
    // kód pro zobrazení stránky 1
}

void displayPage2() {
    // kód pro zobrazení stránky 2
}


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 11  // GPIO11 (Reset pin)
#define OLED_DC    10  // GPIO10 (Data/Command pin)
#define OLED_CS     9  // GPIO9 (Chip Select pin)
#define OLED_CLK   6
#define OLED_MOSI   7
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

Data status{};
ToolSet toolset;
Configuration config;

LittleFSConfig cfg;

int cursor{0};

#define MOTOR_CS  5
#define MOTOR_EN  3
TMC5130Stepper driver(MOTOR_CS, 1.0f);


#define BTN_BACK  12
#define BTN_SET   13
#define BTN_L     8
#define BTN_R     15

#define BTN_EXT   26
bool btn_ext;
bool btn_ext_last;

RP2040_PWM* PWM_Instance;

#define PWR_PIN_USB 21
#define PWR_PIN_EXT 1
bool pwr_state_usb;
bool pwr_state_usb_last;
bool pwr_state_ext;
bool pwr_state_ext_last;

#define BUZZER    14

#define INT_TEMP 27
//MicroDS18B20<INT_TEMP> sensor1;

unsigned long dispensing_start;


#define LED1_T 20
#define LED1_B 22
int LEDstate = 0;

Adafruit_USBD_CDC USBSer;

#define BUFFER_SIZE 128

char buffer[BUFFER_SIZE];
int bufferIndex = 0;
bool newData = false;

long FuturePos = 0;


Tree menu(&toolset);
// DisplayData displayData;

void beep(float freq, uint32_t duration){
  if(config.buzzer){
    PWM_Instance->enablePWM();
    PWM_Instance->setPWM(BUZZER, freq, 50.0f);
    delay(duration);
    PWM_Instance->setPWM(BUZZER, freq, 0.0f);
    PWM_Instance->disablePWM();
  }
  digitalWrite(BUZZER, 0);
}






/////////////////////////////////
//////////////////////////////////
///// DRAWERS 

void draw_init_page(Adafruit_SH1106G *display){
  display->clearDisplay();
  
  display->setTextSize(2);
  display->fillRoundRect(19, 26, 92, 23, 6, SH110X_WHITE);

  display->setTextColor(SH110X_BLACK, SH110X_WHITE);
  display->setCursor(20, 31);
  display->println(F("DISPENSER"));

  display->setTextColor(SH110X_WHITE);
  display->setTextSize(1);
  display->setCursor(23, 53);
  display->println(F("www.ust.cz"));

  draw_top_row(display, "Initialization ...");

  display->display();  // Zobrazí zapsaný text
}


void draw_init_page(ToolSet *toolset, Node *currentNode){
  
  toolset->display->clearDisplay();
  
  toolset->display->setTextSize(2);
  toolset->display->fillRoundRect(19, 26, 92, 23, 6, SH110X_WHITE);

  toolset->display->setTextColor(SH110X_BLACK, SH110X_WHITE);
  toolset->display->setCursor(20, 31);
  toolset->display->println(F("DISPENSER"));

  toolset->display->setTextColor(SH110X_WHITE);
  toolset->display->setTextSize(1);
  toolset->display->setCursor(23, 53);
  toolset->display->println(F("www.ust.cz"));

  draw_top_row(toolset->display, "SN: ....");

  toolset->display->display();  // Zobrazí zapsaný text
}



//void draw_home_page(Adafruit_SH1106G *display, Node *currentNode, Data *status){
void draw_home_page(ToolSet *toolset, Node *currentNode){
  toolset->display->clearDisplay();
  
  draw_top_row(toolset->display, "AMFOC01 - home");

  toolset->display->setCursor(1, 15);
  toolset->display->printf("USB: %s", toolset->status->pwr_usb?"Conn":"N/C");
  toolset->display->setCursor(65, 15);
  toolset->display->printf("12V: %s", toolset->status->pwr_ext?"Conn":"N/C");
  toolset->display->setCursor(1, 27);
  //toolset->display->printf("Mot: %s %d", toolset->status->motor_movement ? "In move" : "Steady ", toolset->status->motor_position);
  toolset->display->setCursor(1, 39);
  toolset->display->printf("Tep: %d C", toolset->status->temp);

  toolset->display->display();  // Zobrazí zapsaný text
}


void draw_movement_action_page(ToolSet *toolset, Node* current){
  toolset->display->clearDisplay();  
  draw_top_row(toolset->display, current->name);

  toolset->display->setCursor(1, 16);
  toolset->display->printf("%sPreset: %i / 10", (cursor==0)?"> ":"", toolset->config->preset);
  toolset->display->setCursor(4, 26);
  toolset->display->printf("%sDuration: %d ms", (cursor==1)?"> ":"", toolset->config->duration);
  toolset->display->setCursor(4, 40);
  toolset->display->printf("%sInterval: %d ms", (cursor==2)?"> ":"", toolset->config->interval);
  
  toolset->display->setCursor(4, 53);
  toolset->display->printf("%sSave", (cursor==3)?"> ":"");


  toolset->display->display();  // Zobrazí zapsaný text
}


void action_control_set(){
  cursor += 1;
  if(cursor>3){
    cursor=0;
  };
}

void action_control_left(){
    switch (cursor) {
      case 0:
        toolset.config->preset -=1;
        break;
      case 1:
        toolset.config->duration -= 10;
        if(toolset.config->duration < 0){toolset.config->duration=0;}
        break;
      case 2:
        toolset.config->interval -= 10;
        if(toolset.config->interval < 0){toolset.config->interval=0;}
        if(toolset.config->interval < toolset.config->duration){toolset.config->duration = toolset.config->interval;}
        break;
      case 3:
       
        break;
      default:
        cursor = 0;
        break;
    }
}
void action_control_right(){
    switch (cursor) {
      case 0:
        toolset.config->preset += 1;
        break;
      case 1:
        toolset.config->duration += 10;
        if(toolset.config->interval < toolset.config->duration){toolset.config->interval = toolset.config->duration;}

        break;
      case 2:
        toolset.config->interval += 10;
        break;
      case 3:
        break;
      default:
        cursor = 0;
        break;
    }
}












void draw_config_1st(ToolSet *toolset, Node* current){
  toolset->display->clearDisplay();
  
  draw_top_row(toolset->display, "Configuration 1");

  toolset->display->setCursor(1, 13);
  toolset->display->printf("%sBuzzer: %s", (cursor==0)?"> ":"", toolset->config->buzzer?"On":"Off");
  toolset->display->setCursor(1, 24);
  toolset->display->printf("%sStpRng: %i", (cursor==1)?"> ":"", toolset->config->step_range);
  toolset->display->setCursor(1, 35);
  toolset->display->printf("%sMotSpd: %i", (cursor==2)?"> ":"", toolset->config->default_speed_step);
  toolset->display->setCursor(1, 46);
  toolset->display->printf("%SSteps/mm: %i", (cursor==3)?"> ":"", toolset->config->steps_per_mm);

  toolset->display->display(); 
}

void draw_config_set(){
  cursor += 1;
  if(cursor>3){
    cursor=0;
  };
}

void draw_config_left(){
    switch (cursor) {
      case 0:
        toolset.config->buzzer = false;
        break;
      case 1:
        toolset.config->step_range -= 1000;
        break;
      case 2:
        toolset.config->default_speed_step -= 1;
        if(toolset.config->default_speed_step<0) {toolset.config->default_speed_step=0;}
        break;
      case 3:
        toolset.config->steps_per_mm -= 10;
        break;
      default:
        cursor = 0;
        break;
    }
}
void draw_config_right(){
    switch (cursor) {
      case 0:
        toolset.config->buzzer = true;
        break;
      case 1:
        toolset.config->step_range += 1000;
        break;
      case 2:
        toolset.config->default_speed_step += 1;
        if(toolset.config->default_speed_step>10) {toolset.config->default_speed_step=10;}
        break;
      case 3:
        toolset.config->steps_per_mm += 10;
        break;
      default:
        cursor = 0;
        break;
    }
}

void draw_config_back(){
    save_config(&config);
}

/*
root
  |
  |__Home (simple status overview)
  |
  |__Operation (presets, time, interval, save, delete)
  |__Settings

*/



void load_config(Configuration *config){
  File file = LittleFS.open("/configuration.dat", "r");
  if (!file) {
    Serial.println("File open failed!");
    return;
  }

  file.read((uint8_t*)config, sizeof(config));
  file.close();
}

void save_config(Configuration *config){
  File file = LittleFS.open("/configuration.dat", "w");
    if (!file) {
    Serial.println("File open failed for write!");
    return;
  }

  file.write((uint8_t*)config, sizeof(config));
  file.close();
}


void setupMenu() {

  toolset.display = &display;
  toolset.status = &status;
  toolset.config = &config;


  Node* root = new Node("Root", false, draw_init_page);
  Node* home = new Node("Home", false, draw_home_page);
  Node* control = new Node("Dispenser control", false);
  Node* control_act = new Node("Dispenser control", false, draw_movement_action_page, action_control_left, nullptr, action_control_right, nullptr, action_control_set, nullptr);
  Node* config = new Node("Configuration", false);
  Node* config_buzzer = new Node("Coonf. - 1st page", false, draw_config_1st, draw_config_left, nullptr, draw_config_right, nullptr, draw_config_set, nullptr, draw_config_back);

  menu.setRoot(root);
  menu.insertNode(root, home);
  menu.insertNode(root, control);
      menu.insertNode(control, control_act);
  menu.insertNode(root, config);
      menu.insertNode(config, config_buzzer);

  menu.setCurrent(home);


}

/////////////////////////////
/////////////////////////////
///
///  END DRAWERS
///
////////////////////////////




void motor_init(){
  digitalWrite(MOTOR_EN, LOW);
  driver.begin();

  driver.ihold(248);
  driver.irun(100);
  driver.iholddelay(0b011);

  driver.GCONF(0b010000000000000000);

  driver.XTARGET( 127<<0 | 127<<16);
  driver.toff(5);

  driver.push();

}


static void buttonLeft(){
  if(!digitalRead(BTN_L)){
    menu.navigateLeft();
  } else {
    menu.navigateLeftUp();
  }
}
static void buttonRight(){
  if(!digitalRead(BTN_R)){
    menu.navigateRight();
  } else {
    menu.navigateRightUp();
  }
}
static void buttonSet(){
  if(!digitalRead(BTN_SET)){
    menu.navigateSet();
  } else {
    menu.navigateSetUp();
  }
}
static void buttonBack(){
  if(!digitalRead(BTN_BACK)){
    menu.navigateBack();
  } else {
    menu.navigateBackUp();
  }
}

bool btn_set_last = 1;
bool btn_back_last = 1;
bool btn_left_last = 1;
bool btn_right_last = 1;


void setup() {

  TinyUSBDevice.setID(0x1209, 0xAE01);  // AstroMeters AMFOC01 VID/PID

  LittleFS.begin();
  load_config(&config);


  // Vypni motor!
  pinMode(MOTOR_EN, OUTPUT);
  digitalWrite(MOTOR_EN, HIGH);

  pinMode(LED1_T, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  analogWriteResolution(12);
  digitalWrite(LED1_B, HIGH);


  SPI.setRX(4);
  //SPI.setCS(OLED_CS);
  SPI.setSCK(OLED_CLK);
  SPI.setTX(OLED_MOSI);

  SPI.begin();

  display.begin(0,1);
  display.clearDisplay();
  display.display();
  draw_init_page(&display);

  // CHECK powering methods
  pinMode(PWR_PIN_USB, INPUT);
  pinMode(PWR_PIN_EXT, INPUT);
  pwr_state_usb = digitalRead(PWR_PIN_USB);
  pwr_state_usb_last = digitalRead(PWR_PIN_USB);
  pwr_state_ext = digitalRead(PWR_PIN_EXT);
  pwr_state_ext_last = digitalRead(PWR_PIN_EXT);


  pinMode(BTN_EXT, INPUT);
  btn_ext = digitalRead(BTN_EXT);
  btn_ext_last = digitalRead(BTN_EXT);

  Serial.begin(9600);
  Serial.setTimeout(10); // ms
  
  if(pwr_state_ext){
    motor_init();
  }
  

  pinMode(BUZZER, OUTPUT);
  PWM_Instance = new RP2040_PWM(BUZZER, 400000, 50.0f);
  PWM_Instance->setPWM();
  beep(1000.0f, 100);
  beep(1200.0f, 100);
  beep(1400.0f, 200);
    
  //sensor1.requestTemp();
  delay(1000);
  setupMenu();

  stop_dispensing();
}


void update_led(){
  int duration = 3;
  analogWrite(LED1_B, int(4095*sin(millis()/1000.0*4*3.1415/duration)) );
}


void pool_buttons(){
  if(btn_set_last != digitalRead(BTN_SET)){
    btn_set_last = digitalRead(BTN_SET);
    buttonSet();
  }

  if(btn_back_last != digitalRead(BTN_BACK)){
    btn_back_last = digitalRead(BTN_BACK);
    buttonBack();
  }

  if(btn_left_last != digitalRead(BTN_L)){
    btn_left_last = digitalRead(BTN_L);
    buttonLeft();
  }

  if(btn_right_last != digitalRead(BTN_R)){
    btn_right_last = digitalRead(BTN_R);
    buttonRight();
  }
}



void loop() {
  int ch;
  
  if(!status.dispensing){
    update_led();
  }


  if(pwr_state_usb != digitalRead(PWR_PIN_USB)){
    pwr_state_usb = digitalRead(PWR_PIN_USB);

    if(pwr_state_usb){
      motor_init();
      beep(900.0f, 150);
      beep(1100.0f, 100);
    }else{
      beep(1100.0f, 100);
      beep(900.0f, 150);
    }
  }

  if(pwr_state_ext != digitalRead(PWR_PIN_EXT)){
    pwr_state_ext = digitalRead(PWR_PIN_EXT);

    if(pwr_state_ext){
      motor_init();
      beep(800.0f, 150);
      beep(1000.0f, 100);
    }else{
      beep(1000.0f, 100);
      beep(800.0f, 150);
    } 
  }


  if(btn_ext != digitalRead(BTN_EXT)){
    btn_ext = digitalRead(BTN_EXT);
    status.dispensing = !btn_ext;
    toolset.display->invertDisplay(!btn_ext);

    if(!btn_ext){
       dispensing_start = millis();
       start_dispensing();
       //beep(800.0f, 150);
    //   beep(1000.0f, 100);
    }else{
        stop_dispensing();
    //   beep(1000.0f, 100);
    //   beep(800.0f, 150);
    } 
  }


  if(status.dispensing){
    Serial.print("Dispensing..");
    Serial.println( millis()-dispensing_start );
    if( (millis()-dispensing_start) > toolset.config->duration && status.valve){
      Serial.println("Konec extruze");
      stop_dispensing();
    }


    if( (millis()-dispensing_start) > toolset.config->interval && !status.valve){
      Serial.println("Konec intervalu");
      dispensing_start = millis();
      start_dispensing();
    }
  }


  pool_buttons();

  status.pwr_ext = pwr_state_ext;
  status.pwr_usb = pwr_state_usb;
  status.temp = -999.0;
  status.text = menu.current->name;
  menu.Run();


}


void start_dispensing(){
  status.valve = 1;
  analogWrite(LED1_B, 4000 );

  driver.XTARGET( 255<<0 | 255<<16);
  Serial.println("Start dispensing");
}


void stop_dispensing(){
  status.valve = 0;
  analogWrite(LED1_B, 200 );


  driver.XTARGET( -255<<0 | 127<<16);
  Serial.println("Stop dispensing");
}