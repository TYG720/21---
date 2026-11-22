#ifndef __Init_H_
#define __INIT_H_
#define INIT_WIFI_COMMAND_DEBUG_ENABLE 0
// Set this locally before enabling WiFi SPI debugging. Do not commit a real
// network password to a public repository.
#ifndef WIFI_SPI_PASSWORD
#define WIFI_SPI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif

#include "zf_common_headfile.h"

#if INIT_WIFI_COMMAND_DEBUG_ENABLE
extern uint8 motor_pit;
extern uint32 _t;
extern volatile uint8 wifi_spi_get_data_buffer[256];
extern uint8 data_len;
extern int16 just_i,just_p,just_tar,just_angle;
#endif
extern uint8 k4;
void System_Init(void);

// 蜂鸣器相关接口

// 调试接口
#if INIT_WIFI_COMMAND_DEBUG_ENABLE
void test_ss(void);
// 按键启动接口，WiFi 初始化后开始
void Begin(void);
#endif
// 蜂鸣器控制
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_RequestPulse(void);
void Buzzer_Task5ms(void);
void Buzzero_O(void);
void Buzzer_test(uint8 count);
// 调试输出
void o_O(const char dat[]);
void x_X(void); // 调试输出
#endif





