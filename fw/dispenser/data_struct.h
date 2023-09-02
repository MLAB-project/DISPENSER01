#pragma once
#ifndef DATASTRUCT_H_
#define DATASTRUCT_H_

#include <Adafruit_SH110X.h>
#include <vector>
#include "LittleFS.h"


struct Configuration{
  bool buzzer = 1;
  int step_range = 1000000;
  int default_speed_step = 5;
  int steps_per_mm = 1000;
  int duration = 500;
  int interval = 1000;
  int preset = 0;
  bool dispensing;
  bool valve;
};

struct Data{
   bool motor_movement;
   int duration;
   int interval;
   int preset = 0;
   bool dispensing;
   bool pwr_ext;
   bool pwr_usb;
   float temp;
   String text;
   bool valve;
};


struct ToolSet{
  Adafruit_SH1106G* display;
  void* buzzer;
  Data* status;
  OcularsData* oculars;
  Configuration* config;
};



void draw_top_row(Adafruit_SH1106G *display, String content){
  display->fillRoundRect(5, 0, 118, 11, 3, SH110X_WHITE);
  display->setTextColor(SH110X_BLACK, SH110X_WHITE);
  display->setTextSize(1);
  display->setCursor(9, 2);
  display->print( content );
  display->setTextColor(SH110X_WHITE);

}




#endif
