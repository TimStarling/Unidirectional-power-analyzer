#include "bsp_system.h"
#define fft_sample_freq 5120
#define correction_phase 0.05f //相位差修正系数
#define fft_correction_vot 3.869f
extern system_parameter sp;
arm_cfft_radix4_instance_f32 scfft;//傅里叶变换存放结构体

float FFT_VOLT_INPUT[FFT_LEN * 2];//电压 实部和虚部
float FFT_VOLT_OUTPUT[FFT_LEN];//电压 FFT结果存放
float FFT_ELEC_INPUT[FFT_LEN * 2];//电流 实部和虚部
float FFT_ELEC_OUTPUT[FFT_LEN];//电流 FFT结果存放
float freamp[FFT_LEN];//用于存放各次谐波幅值的数组

float phase_volt;//电压相位
float phase_elec;//电流相位

int basic_num = 0;//基波分量位置

void fft_init()
{
	arm_cfft_radix4_init_f32(&scfft,FFT_LEN,0,1);
}

/*
频率=数组下标* 
采样率/fft计算的点数
5120Hz
*/
void fft_volt_proc()
{
	//1.将浮点数切换为复数类型
	for(int i=0;i<FFT_LEN;i++){
		FFT_VOLT_INPUT[i*2]=(float)sp.adc_value0[i];//实部
		FFT_VOLT_INPUT[i*2+1]=0;
	}
	//2.调用FFT进行计算
	arm_cfft_radix4_f32(&scfft,FFT_VOLT_INPUT);
	//3.将复数数据转换为实数,取模
	arm_cmplx_mag_f32(FFT_VOLT_INPUT,FFT_VOLT_OUTPUT,FFT_LEN);
	for(int k=0;k<FFT_LEN;k++){
		freamp[k] = FFT_VOLT_OUTPUT[k] * fft_correction_vot / 1000.0f;
	}
}

void fft_elec_proc()
{
	//1.将浮点数切换为复数类型
	for(int i=0;i<FFT_LEN;i++){
		FFT_ELEC_INPUT[i*2]=(float)sp.adc_value1[i];//实部
		FFT_ELEC_INPUT[i*2+1]=0;
	}
	//2.调用FFT进行计算
	arm_cfft_radix4_f32(&scfft,FFT_ELEC_INPUT);
	//3.将复数数据转换为实数,取模
	arm_cmplx_mag_f32(FFT_VOLT_INPUT,FFT_ELEC_OUTPUT,FFT_LEN);
}

void THD_calcu()
{
	float sum_data = 0;
	for(int i=1;i<=8;i++){
		sum_data = freamp[i*10] * freamp[i*10];
	}
	sum_data = sum_data /  freamp[0];
	sp.THD_data = sqrt(sum_data)*100;
}

void get_freq()//获取频率
{
	static double input_freq_old;
	if(sp.system_mode==0 || sp.system_mode==2){//界面0时有效
		float volt_max=0;
		for(int j = 1;j<FFT_LEN;j++)
		{
			if(FFT_VOLT_OUTPUT[j]>volt_max)
			{
				volt_max = FFT_VOLT_OUTPUT[j];
				basic_num=j;
			}
		}
		sp.input_freq = (float)basic_num*5120.0f/FFT_LEN;
		if(sp.input_freq>500)sp.input_freq=input_freq_old;
		else input_freq_old = sp.input_freq;
	}
}

// 获得电压相位
void get_phase_vot()
{
	phase_volt = atan2f(FFT_VOLT_INPUT[20+1], FFT_VOLT_INPUT[20]) * 180.0f / PI;
}

// 获得电流相位
void get_phase_elec()
{
	phase_elec= atan2f(FFT_ELEC_INPUT[20+1], FFT_ELEC_INPUT[20])* 180.0f / PI;
}

// 获得相位差
float get_phase_diff()
{
    if (sp.system_mode == 2) {
		float phase_diff;//相位差
        get_phase_vot();
        get_phase_elec();

        // 计算基波分量的相位差
        phase_diff = phase_volt - phase_elec;

        // 将相位差限制在 -180 到 180 度之间
        if (phase_diff > 180) {
            phase_diff -= 360;
        } else if (phase_diff < -180) {
            phase_diff += 360;
        }

        // 更新相位差到系统参数
        return phase_diff;
    }
}

void phase_calu()
{
	float phase_data = 0.00f;
	for(int i = 0;i<10;i++){
		phase_data += get_phase_diff();
	}
	sp.phase_data = (phase_data / 10.0f) - correction_phase;
}