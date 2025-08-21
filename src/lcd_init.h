#ifndef _LCD_INIT_H
#define _LCD_INIT_H

#include "hal_data.h"


#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif



//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_04, BSP_IO_LEVEL_LOW)//SCL=SCLK
#define LCD_SCLK_Set() R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_04, BSP_IO_LEVEL_HIGH)

#define LCD_MOSI_Clr() R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_03, BSP_IO_LEVEL_LOW)//SDA=MOSI
#define LCD_MOSI_Set() R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_03, BSP_IO_LEVEL_HIGH)

#define LCD_RES_Clr()  R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_06, BSP_IO_LEVEL_LOW)//RES
#define LCD_RES_Set()  R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_06, BSP_IO_LEVEL_HIGH)

#define LCD_DC_Clr()   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_08, BSP_IO_LEVEL_LOW)//DC
#define LCD_DC_Set()   R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_08, BSP_IO_LEVEL_HIGH)
 		     
 		     
#define TFT_BL_H() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_05, BSP_IO_LEVEL_HIGH)	//BL
#define TFT_BL_L() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_05, BSP_IO_LEVEL_LOW)	//BL

#define TFT_CS_H() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_02, BSP_IO_LEVEL_HIGH)	//CS
#define TFT_CS_L() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_02_PIN_02, BSP_IO_LEVEL_LOW)//CS

#define TFT_DC_D() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_08, BSP_IO_LEVEL_HIGH)	//DC
#define TFT_DC_C() 		R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_05_PIN_08, BSP_IO_LEVEL_LOW)//DC

#define TFT_RES_H() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_06, BSP_IO_LEVEL_HIGH)	//RST
#define TFT_RES_L() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_06, BSP_IO_LEVEL_LOW)//RST

#define TFT_SCL_H() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_04, BSP_IO_LEVEL_HIGH)	//CLK
#define TFT_SCL_L() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_04, BSP_IO_LEVEL_LOW)//CLK

#define TFT_MOSI_H() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_02, BSP_IO_LEVEL_HIGH)	//MOSI
#define TFT_MOSI_L() 	R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_02, BSP_IO_LEVEL_LOW)//MOSI




void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节
void LCD_WR_REG(uint8_t dat);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif



