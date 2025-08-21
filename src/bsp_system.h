#include "hal_data.h"
#include "stdio.h"
#include <stdlib.h>
#include "lcd.h"
#include "key_app.h"
#include "led_app.h"
#include "math.h"
#include "arm_math.h"
#define FFT_LEN 1024 //FFT长度 4^n


typedef struct{
	int system_mode;//系统模式
	int processing_flag;
	int cut_flag;//断电标志位
	uint16_t y_front[FFT_LEN];//电压纵坐标存放
	uint16_t y_behind[FFT_LEN];//电流纵坐标存放
	uint16_t up_num;//暂升次数
	uint16_t down_num;//暂降次数
	uint16_t interrupt_num;//中断次数
	uint16_t relay_mode;//继电器挡位选择
	uint16_t auto_flag;//继电器挡位自动选择标志位
	uint16_t pow_max;
	uint16_t classify_flag;//恶性负载识别标志位
	
	double volt_rms;//电压有效值
	double elect_rms;//电流有效值
	double input_freq;//输入频率
	double phase_data;//相位数据
	double adc_value1[FFT_LEN];//电流数据存放
	double adc_value0[FFT_LEN];//电压数据存放
	
	
	float apparent_power;//视在功率参数
	float reactive_power;//无功功率参数
	float active_power;//有功功率参数
	float power_factor;//功率因数参数
	float energy_comsumption;//耗能
	float fft_volt_buf[FFT_LEN];
	float THD_data;
	
}system_parameter;

typedef enum//时间戳暂存变量
 {
	SYS_DELAY_UNITS_SECONDS = 240000000, ///< Requested delay amount?→is in seconds
	SYS_DELAY_UNITS_MILLISECONDS = 240000, //→is in microseconds
	
	SYS_DELAY_UNITS_MICROSECONDS = 240, //  →is in milliseconds
 } sys_delay_units_t;

void scheduler_init();
void scheduler_run();
void systick_init(uint32_t IT_freqency);//中断初始化
void lcd_static_init();
void lcd_proc();
void lcd_static_proc();
void relay_proc();

 
void gpt0_init();
 
void ADC_Init(void);
double Read_ADC_Voltage_Value0(void);
double Read_ADC_Voltage_Value1(void);
void volt_rms_calcu();//电压有效值计算
void elect_rms_calcu();//电流有效值计算
void get_power_data();//功率计算
void get_freq();//频率计算
 
void fft_init();
void fft_volt_proc();
void fft_elec_proc();
void THD_calcu();
void get_harmonic();
void phase_calu();
void qual_judgment();
void total_energy_comsumption();
void uart_init(void);
void uart_proc();
void uart4_proc();
void uart4_write_string (const char *str);
void HMI_wave(char *name,int ch,int val);

