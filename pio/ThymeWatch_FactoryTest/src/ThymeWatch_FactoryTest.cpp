#include "display.h"

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

#include "frame_00.h"
#include "frame_01.h"
#include "frame_02.h"
#include "frame_03.h"
#include "frame_04.h"
#include "frame_05.h"
#include "frame_06.h"
#include "frame_07.h"
#include "frame_08.h"
#include "frame_09.h"
#include "frame_10.h"
#include "frame_11.h"

#include "macos_face.h"
#include "rtc.h"
// #include "flappy_bird.h"
#include "buzzer.h"
#include "t_rex.h"

#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#define ENABLE_DEEP_SLEEP 0

#define uS_TO_S_FACTOR                                                         \
  1000000                /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60 /* Time ESP32 will go to sleep (in seconds) */

// Set the size of the display here, e.g. 144x168!
// Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 144, 168);
// The currently-available SHARP Memory Display (144x168 pixels)
// requires > 4K of microcontroller RAM; it WILL NOT WORK on Arduino Uno
// or other <4K "classic" devices!  The original display (96x96 pixels)
// does work there, but is no longer produced.

#define BLACK 0
#define WHITE 1

int minorHalfSize; // 1/2 of lesser of display width or height

RTC_DATA_ATTR char stringToKeep[20];

long boot_time;

RTC_DATA_ATTR uint32_t last_cost;

const uint8_t *nyan_frames[] = {
    nyan_128x93_frame_00, nyan_128x93_frame_01, nyan_128x93_frame_02,
    nyan_128x93_frame_03, nyan_128x93_frame_04, nyan_128x93_frame_05,
    nyan_128x93_frame_06, nyan_128x93_frame_07, nyan_128x93_frame_08,
    nyan_128x93_frame_09, nyan_128x93_frame_10, nyan_128x93_frame_11,
};

void show_colors() {
  uint8_t color = BLACK;
  for (int i = 0; i < minorHalfSize; i += 8) {
    // alternate colors
    canvas.fillRect(i, i, canvas.width() - i * 2, canvas.height() - i * 2,
                    color & 0b111);
    canvas.refresh();
    color++;
  }
  delay(10000);
}

void show_anim() {
  canvas.clearDisplay();
  while (1) {
    for (size_t i = 0; i < sizeof(nyan_frames) / sizeof(nyan_frames[0]); i++) {
      canvas.drawRGBBitmap(0, 0, (uint16_t *)nyan_frames[i], 128, 93);
      canvas.refresh();
      // delay(100);
    }
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  float result;
  result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return result;
}

void show_macos_time() {
  // canvas1.fillScreen(BLACK);

  // canvas1.writeFastHLine(1, 1, 9, WHITE);
  // canvas1.writeFastHLine(2, 2, 9, BLACK);

  // canvas.clearDisplay();
  canvas.drawRGBBitmap(0, 0, (uint16_t *)macos_128x128, 128, 128);
  canvas.fillRect(3, 37, 107, 33, 0b100);
  canvas.setCursor(3, 37 + 25);
  canvas.setTextSize(1);
  // canvas.setFont(&FreeMono24pt7b);
  canvas.setFont(&FreeMonoBold12pt7b);
  canvas.setTextColor(0b111);
  char buf[16];
  DateTime now = rtc.now();
  snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%02d:%02d:%02d", now.hour(),
           now.minute(), now.second());
  canvas.println(buf);
  auto sensorValue = analogRead(2);
  // Serial.println(sensorValue);
  float vbat = mapf(sensorValue, 0, 4095, 0, 3.3) * 2;
  canvas.setTextColor(0b000);
  canvas.print(vbat);
  canvas.println("V");
  canvas.print(last_cost);
  canvas.println("ms");
  // canvas.drawRGBBitmap(0, 0, canvas1.getBuffer(), 32, 32);
  canvas.refresh();
  last_cost = millis() - boot_time;
}

void show_time() {
  DateTime now = rtc.now();

  canvas.setRotation(0);
  canvas.clearDisplay();

  canvas.setTextSize(1);
  canvas.setTextColor(BLACK);
  canvas.setCursor(0, 0);
  canvas.println("Hello, world!");
  canvas.setTextColor(WHITE, BLACK); // inverted text
  canvas.println(3.141592);
  canvas.setTextSize(2);
  canvas.setTextColor(BLACK);
  canvas.print("0x");
  canvas.println(0xDEADBEEF, HEX);
  canvas.print("cost: ");
  canvas.println(last_cost, HEX);

  canvas.print(now.year(), DEC);
  canvas.print('/');
  canvas.print(now.month(), DEC);
  canvas.print('/');
  canvas.println(now.day(), DEC);

  canvas.print(now.hour(), DEC);
  canvas.print(':');
  canvas.print(now.minute(), DEC);
  canvas.print(':');
  canvas.print(now.second(), DEC);
  canvas.println();

  // sprintf(stringToKeep, "string");
  canvas.refresh();
}

void setup(void) {
  boot_time = millis();
  if (!ENABLE_DEEP_SLEEP) {
    Serial.begin(115200);
  }
  pinMode(SHARP_DISP, OUTPUT);
  digitalWrite(SHARP_DISP, HIGH);
  SPI.begin(SHARP_SCK, 11, SHARP_MOSI, SHARP_SS);

  // start & clear the display
  canvas.begin();

  // Several shapes are drawn centered on the screen.  Calculate 1/2 of
  // lesser of display width or height, this is used repeatedly later.
  minorHalfSize = min(canvas.width(), canvas.height()) / 2;
  init_rtc();
  if (!ENABLE_DEEP_SLEEP) {
    show_datetime();
  }
  // setup2();
  // show_anim();
  // show_time();
  // canvas.setRotation(0);
  // t_rex_setup();
}

void loop(void) {
  //  while (1) {
  //    t_rex_loop();
  //    delay(1);
  //  }
  // flappy_bird_loop();
  // loop2();

  show_macos_time();
  if (ENABLE_DEEP_SLEEP) {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  } else {
    Serial.println(last_cost);
    delay(950);
  }
}
