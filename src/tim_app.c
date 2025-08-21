#include "bsp_system.h"
extern system_parameter sp;
uint16_t adc_value_indictor = 0;

//定时器初始化
void gpt0_init()
{
	 /*初始化GPT0模块*/
	R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
	/*启动GPT0定时器*/
	R_GPT_Start(&g_timer0_ctrl);

}

/*GPT中断回调函数*/
void gpt0_callback(timer_callback_args_t* p_args)
 {
	/*定时器溢出事件*/
	if(TIMER_EVENT_CYCLE_END== p_args->event)
	{
		//---ADC采样与处理
		sp.adc_value0[adc_value_indictor] = Read_ADC_Voltage_Value0();
		sp.adc_value1[adc_value_indictor] = Read_ADC_Voltage_Value1();
		if(!sp.processing_flag)sp.y_front[adc_value_indictor] = (uint16_t)(sp.adc_value0[adc_value_indictor] * 0.2f + 60);//电压图像生成辅助
		if(!sp.processing_flag)sp.y_behind[adc_value_indictor] = (uint16_t)(sp.adc_value1[adc_value_indictor] * 50.0f + 20);//电流生成辅助
		if(++adc_value_indictor==FFT_LEN)
		{
			sp.processing_flag = 1;
			adc_value_indictor = 0;
			fft_volt_proc();
			fft_elec_proc();
		}
	}
 }