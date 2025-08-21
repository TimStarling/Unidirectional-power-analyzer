#include "bsp_system.h"
//240*320
extern system_parameter sp;
extern float freamp[FFT_LEN];//用于存放各次谐波幅值的数组
uint16_t Draw_Volt_Old[512] = {0};
uint16_t Draw_Elec_Old[512] = {0};
int show_volt[200] = {0};
float Draw_FFT_Old[3*FFT_LEN] = {0};
void lcd_static_init()
{
	LCD_Init();
    LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
}


//串口屏字符串
void HMI_send_string(char *name,char *showdata){
	printf("%s=\"%s\"\xff\xff\xff",name,showdata);
}

//串口屏发送整数
void HMI_send_number(char *name,int num){
	printf("%s=%d\xff\xff\xff",name,num);
}

//串口屏发送浮点数
void HMI_send_float(char *name,float num){
	printf("%s=%d\xff\xff\xff",name,(int)(num * 100));
}

//串口屏发送波形
void HMI_wave(char *name,int ch,int val)
{
	printf("add %s,%d,%d\xff\xff\xff",name,ch,val);
}

void uart_draw_line(int x1,int y1,int x2,int y2,char *color)
{
	printf("line %d,%d,%d,%d,%s\xff\xff\xff", x1, y1, x2, y2, color);
}

void HMI_wave_fast(char *name,int ch,int count,int* show_data)
{
	int i;
	printf("addt %s,%d,%d\xff\xff\xff",name,ch,count);
	R_BSP_SoftwareDelay(100,BSP_DELAY_UNITS_MILLISECONDS);
	for(i = 0;i<count;i++)
	{
		 printf("%c",(int)((show_data[i]) % 256));
	}
	printf("\x01\xff\xff\xff");
}


void lcd_proc()
{
	char LCD_Dis_Buff[60] = {0};
	switch(sp.system_mode){//------------------LCD动态显示部分
		case 0:
			sprintf(LCD_Dis_Buff,"%-.2fV  ",(float)sp.volt_rms);
			HMI_send_string("t2.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fA  ",(float)sp.elect_rms);
			HMI_send_string("t6.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.1fHz  ",(float)sp.input_freq);
			HMI_send_string("t7.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.1f%%  ",(float)sp.THD_data);
			HMI_send_string("t8.txt",LCD_Dis_Buff);
			break;	         
		case 1:
			if(sp.processing_flag==1){
				for (int p = 0; p <1024; p++) { 
					HMI_wave("s0.id",0, sp.y_front[p]); //sp.y_front
					HMI_wave("s1.id",0, sp.y_behind[p]); //sp.y_behind
					if(sp.system_mode != 1)break;
				}
				sp.processing_flag = 0;

			}
			break;
		case 2:
			sprintf(LCD_Dis_Buff,"%-.2fW  ",(float)sp.apparent_power);
			HMI_send_string("t2.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fW  ",(float)sp.active_power);
			HMI_send_string("t6.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fW  ",(float)sp.reactive_power);
			HMI_send_string("t7.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2f  ",(float)sp.power_factor);
			HMI_send_string("t8.txt",LCD_Dis_Buff);
			if(sp.power_factor >= 0.70f){
				sprintf(LCD_Dis_Buff,"1");
				HMI_send_string("t50.txt",LCD_Dis_Buff);
			}
			else {
				sprintf(LCD_Dis_Buff,"0");
				HMI_send_string("t50.txt",LCD_Dis_Buff);
			}
			
			break;
		case 3:
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[10]);
			HMI_send_string("t31.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[30]);
			HMI_send_string("t32.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[50]);
			HMI_send_string("t33.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[70]);
			HMI_send_string("t34.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[90]);
			HMI_send_string("t35.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[110]);
			HMI_send_string("t36.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[130]);
			HMI_send_string("t37.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2fmV   ",freamp[150]);
			HMI_send_string("t38.txt",LCD_Dis_Buff);
			break;
		case 4:
				for(uint16_t d = 0; d < 480; d++)
				{	
					// 计算旧数据和新数据的幅值（限制在42-168范围内）
					int old_value = (int)Draw_FFT_Old[d+3];
					int new_value = (int)freamp[d+3];
					
					// 将幅值映射到42-190
					int old_y = 190 - (old_value * (190 - 42) / 15);  // 假设原最大值为15
					int new_y = 190 - (new_value * (190 - 42) / 15);
					
					// 确保值在42-190
					old_y = (old_y < 42) ? 42 : (old_y > 190 ? 190 : old_y);
					new_y = (new_y < 42) ? 42 : (new_y > 190 ? 190 : new_y);
					
					// 绘制旧数据（白色）
					uart_draw_line(d, 190, d, old_y, "WHITE");
					
					// 绘制新数据（红色）
					uart_draw_line(d, 190, d, new_y, "RED");
					
					Draw_FFT_Old[d+2] = freamp[d+2];
					if(sp.system_mode != 4)break;
				}
			break;
		case 5:
			break;
		case 6:
			sprintf(LCD_Dis_Buff,"%-.2fJ/S   ",sp.apparent_power);
			HMI_send_string("t2.txt",LCD_Dis_Buff);
			sprintf(LCD_Dis_Buff,"%-.2f KWH  ",sp.energy_comsumption*1.0/3600000);
			HMI_send_string("t7.txt",LCD_Dis_Buff);
		break;
	}
}
