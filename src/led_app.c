#include "bsp_system.h"
extern system_parameter sp;
uint8_t ucLed[4] = {0,0,0,0};

void led_disp(uint8_t *ucLed)
{
	uint8_t temp=0x00;
	static uint8_t temp_old = 0xff;
	for(int i=0;i<4;i++){
		temp |= ucLed[i]<<i;
	}
	if(temp != temp_old){
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_11_PIN_00,!ucLed[0]);
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_11_PIN_01,!ucLed[1]);
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_07_PIN_04,!ucLed[2]);
		R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_07_PIN_05,!ucLed[3]);
	}
}

void led_proc()
{
	led_disp(ucLed);
	ucLed[0] = sp.auto_flag;
	ucLed[1] = sp.classify_flag;
}