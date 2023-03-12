#pragma once

#include <driver/ledc.h>
#include <Arduino.h>

// 设置 LEDC 频率、分辨率和通道
const int LEDC_FREQ = 8000;     // LEDC 频率
const int LEDC_RES = 8;         // LEDC 分辨率
const int LEDC_CHANNEL = 0;     // LEDC 通道
const int LEDC_TIMER = LEDC_TIMER_0;  // LEDC 计时器

// 设置蜂鸣器引脚
const int BUZZER_PIN = 3;

void setup2() {
  // 初始化 LEDC 模块
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RES);

  // 绑定 LEDC 通道和引脚
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);

  // 设置 LEDC 计时器
  // ledcSetupTimer(LEDC_TIMER, LEDC_FREQ, LEDC_RES);
}

void loop2() {
  // 控制蜂鸣器发声
  ledcWriteTone(LEDC_CHANNEL, 4000);  // 发射 4000Hz 的声音
  delay(15);                          // 持续 ms
  ledcWriteTone(LEDC_CHANNEL, 0);     // 停止发声
}
