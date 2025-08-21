#include "mqtt.h"
#include "bsp_system.h"
#include <string.h>

void Mqtt_Init()
{
	char test_pub[50];
	char test_sub[50];
	
	uart4_write_string("AT+RST\r\n");  
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("ATE0\r\n");  
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("AT+CWDHCP=1,1\r\n"); 
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("AT+CWMODE=1\r\n");
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("AT+CWJAP=\"zyh\",\"zz971964\"\r\n");
	R_BSP_SoftwareDelay(5000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("AT+MQTTUSERCFG=0,1,\"ESP\",\"89Mif\",\"version=2018-10-31&res=proD\",0,0,\"\"\r\n");
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
	uart4_write_string("AT+MQTTCONN=0,\"broker.mqttdashboard.com\",1883,1\r\n");
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
	
/*	sprintf(test_pub,"AT+MQTTPUB=0,\"nitamade/jiushe/ge/shabi\",\"TT=567\",1,0\r\n");
	uart4_write_string(test_pub);
	R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MILLISECONDS);
	
	sprintf(test_sub,"AT+MQTTSUB=0,\"yuan/shen/qi/dong\",0\r\n");
	uart4_write_string(test_sub);
	R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MILLISECONDS);*/
	
	uart4_write_string("AT+CIPMODE=1\r\n");
	R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MILLISECONDS);
}
