#include "bsp_system.h"
#define correction_vot 211 //电压修正系数 83
#define correction_elc_mul 1 //电流修正系数
#define correction_elc_sub 0 //电流修正系数

#define up_elec 1.05f //电流抬升值
/*
	0.04-6.89 6.85
	0.74 - 13.43 12.69
	
*/
extern system_parameter sp;
int up_flag = 0;//暂升辅助判断
int down_flag = 0;//暂降辅助判断
int interrupt_flag = 0;//中断辅助判断
volatile float current_value_sqrt_num;//电流采集有效值计算平均值(新)
volatile float current_value_final_num;//电流采集有效值计算平均值(新)
void ADC_Init(void)
{
	fsp_err_t err;
	err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
	
	err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
	assert(FSP_SUCCESS == err);
}


//ADC 转换完成标志位
volatile bool scan_complete_flag = false;
//ADC中断回调函数
void adc_callback(adc_callback_args_t * p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	scan_complete_flag = true;
}

/*ADC0通道采集 电压*/
double Read_ADC_Voltage_Value0(void)
{
	uint16_t adc_data0;
	double a0;
	(void)R_ADC_ScanStart(&g_adc0_ctrl);
	while (!scan_complete_flag) //等待转换完成标志
	{
		;
	}
	scan_complete_flag = false; //重新清除标志位
	/* 读取通道 0 数据 */
	R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_4, &adc_data0);
	/* ADC原始数据转换为电压值（ADC参考电压为3.3V）*/
	a0 = (double)(adc_data0*220.0f/1351.35f) - correction_vot;//  220/4095*3.3
	return a0;
 }

/*ADC1通道采集 电流*/
double Read_ADC_Voltage_Value1(void)
{
	uint16_t adc_data1;
	double a1;
	(void)R_ADC_ScanStart(&g_adc0_ctrl);
	while (!scan_complete_flag) //等待转换完成标志
	{
		;
	}
	scan_complete_flag = false; //重新清除标志位
	/* 读取通道 0 数据 */
	R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_8, &adc_data1);
	/* ADC原始数据转换为电压值（ADC参考电压为3.3V）*/
	a1 = (double)(adc_data1*3.3f/4095.0f);//
	return a1;
 }


 
//电压有效值计算
void volt_rms_calcu()
{
	float volt_square_sum = 0;
	for(int i=0;i<FFT_LEN;i++){
		volt_square_sum += pow(sp.adc_value0[i],2.0);
	}
	float volt_final_sum = volt_square_sum / FFT_LEN;
	sp.volt_rms = sqrt(volt_final_sum);
	if(sp.volt_rms < 227.0f)sp.volt_rms+=1.2f;
	else if(sp.volt_rms > 229.0f)sp.volt_rms-=1.2f;
	else if(sp.volt_rms > 230.0f)sp.volt_rms-=3.0f;
	else if(sp.volt_rms > 231.0f)sp.volt_rms-=5.0f;
	if(sp.volt_rms > 280.0f)sp.volt_rms=280.0f;
}

/*
	220：6
	220：2
	220：0.5
	220：0.2
*/
//电流有效值计算
void elect_rms_calcu()
{
	if(sp.relay_mode == 0){
			for(int i=0;i<FFT_LEN;i++)
			{
				current_value_sqrt_num = (sp.adc_value1[i] - up_elec);//电压减去抬升电平
				current_value_final_num += current_value_sqrt_num*current_value_sqrt_num;//计算平方和
			}
			current_value_final_num = current_value_final_num/FFT_LEN;//平方和除以1020;
			sp.elect_rms = sqrt(current_value_final_num) * 25.0f / 100;//计算得到的有效值
	}
	else if(sp.relay_mode == 1){
			for(int i=0;i<FFT_LEN;i++)
			{
				current_value_sqrt_num = (sp.adc_value1[i] - up_elec);//电压减去抬升电平
				current_value_final_num += current_value_sqrt_num*current_value_sqrt_num;//计算平方和
			}
			current_value_final_num = current_value_final_num/FFT_LEN;//平方和除以1020;
			sp.elect_rms = sqrt(current_value_final_num) * 50.0f / 100;//计算得到的有效值
	}
	else if(sp.relay_mode == 2){
			for(int i=0;i<FFT_LEN;i++)
			{
				current_value_sqrt_num = (sp.adc_value1[i] - up_elec);//电压减去抬升电平
				current_value_final_num += current_value_sqrt_num*current_value_sqrt_num;//计算平方和
			}
			current_value_final_num = current_value_final_num/FFT_LEN;//平方和除以1020;
			sp.elect_rms = sqrt(current_value_final_num) * 150.0f / 100;//计算得到的有效值
	}
	else if(sp.relay_mode == 3){  
			for(int i=0;i<FFT_LEN;i++)
			{
				current_value_sqrt_num = (sp.adc_value1[i] - up_elec);//电压减去抬升电平
				current_value_final_num += current_value_sqrt_num*current_value_sqrt_num;//计算平方和
			}
			current_value_final_num = current_value_final_num/FFT_LEN;//平方和除以1020;
			sp.elect_rms = sqrt(current_value_final_num) * 1000.0f / 100;//计算得到的有效值
	}
	current_value_final_num = 0;
//	sp.elect_rms = correction_elc_mult * sp.elect_rms;
	sp.elect_rms -= 0.82f;
	if(sp.elect_rms<0.49f)sp.elect_rms=0.0f;
	if(sp.elect_rms>6.7f)sp.elect_rms=6.7f;
	if(sp.volt_rms > 270.0f && sp.elect_rms>3.0f)sp.elect_rms=0;
}


//电能质量判断
void qual_judgment(){
		float volt_max = 0;
		for(int i = 0;i<FFT_LEN;i++){
			if(volt_max < sp.adc_value0[i])volt_max=sp.adc_value0[i];
		}
		//暂升次数判断
		if((volt_max > 1.0f * 1.5f)&&(up_flag==0)){
			sp.up_num ++;
			up_flag = 1;
		}else if(volt_max <= 1.0f * 1.5f){
			up_flag = 0;
		}
		//暂降次数判断
		if((volt_max < 1.0f * 0.5f)&&(down_flag==0)){
			down_flag = 1;
			sp.down_num ++;
			
		}else if(volt_max >= 1.0f * 0.5f){
			down_flag = 0;
		}
		//中断次数判断
		if((volt_max < 0.1)&&(interrupt_flag==0)){
			sp.interrupt_num ++;
			interrupt_flag = 1;
		}
		else if(volt_max >= 0.1){
			interrupt_flag = 0;
		}
}



// 获取各种功率参数
void get_power_data()
{
	// 视在功率
	float pow_aveg_sum = 0;
    for(int i = 0; i < FFT_LEN; i++){
		pow_aveg_sum += sp.elect_rms * sp.volt_rms;
    }
    sp.apparent_power = pow_aveg_sum / FFT_LEN;
    if(sp.system_mode == 2){
        // 角度转换：将角度转换为弧度
        float phase_radians = sp.phase_data * (PI / 180.0f);

        // 有功功率
        sp.active_power = sp.apparent_power * cos(phase_radians);

        // 无功功率
        sp.reactive_power = sp.apparent_power * sin(phase_radians);

        // 功率因数
        sp.power_factor = sp.active_power / sp.apparent_power; // 计算功率因数
    }
}

//总耗能
void total_energy_comsumption()
{
	sp.energy_comsumption = (sp.energy_comsumption + (sp.apparent_power/4.0f));
}