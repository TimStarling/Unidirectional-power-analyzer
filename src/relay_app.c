#include "relay_app.h"

extern system_parameter sp;
uint8_t ucRelay[4] = {0,0,0,0};

void relay_disp(uint8_t *ucRelay)
{
	uint8_t temp=0x00;
	static uint8_t temp_old = 0xff;
	for(int i=0;i<4;i++){
		temp |= ucRelay[i]<<i;
	}
	if(temp != temp_old){
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_04_PIN_15,ucRelay[0]);
		//R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_07_PIN_12,ucRelay[1]);
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_07_PIN_13,ucRelay[2]);
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_04_PIN_14,ucRelay[3]);
	}
}

//继电器控制函数
void relay_proc()
{
	relay_disp(ucRelay);
	if(sp.apparent_power > sp.pow_max || sp.cut_flag == 1 || sp.THD_data > 5 || (sp.classify_flag == 1 && sp.power_factor >= 0.70f))R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_07_PIN_12,0);
	if(sp.auto_flag){//自动匹配
		if(sp.elect_rms > 6 * 0.6f){
			sp.relay_mode = 3;
		}
		else if(sp.elect_rms > 2 * 0.6f)
		{
			sp.relay_mode = 2;
		}
		else if(sp.elect_rms > 0.5 * 0.6f)
		{
			sp.relay_mode = 1; 
		}
		else{
			sp.relay_mode = 0;
		}
	}
	if(sp.relay_mode == 0){
			ucRelay[3] = 0;
			ucRelay[2] = 0;
			ucRelay[1] = 0;
			ucRelay[0] = 1;
	}
	else if(sp.relay_mode == 1){
			ucRelay[3] = 0;
			ucRelay[2] = 0;
			ucRelay[1] = 1;
			ucRelay[0] = 0;
	}
	else if(sp.relay_mode == 2){
			ucRelay[3] = 0;
			ucRelay[2] = 1;
			ucRelay[1] = 0;
			ucRelay[0] = 0;
	}
	else if(sp.relay_mode == 3){
			ucRelay[3] = 1;
			ucRelay[2] = 0;
			ucRelay[1] = 0;
			ucRelay[0] = 0;
	}
	
}
