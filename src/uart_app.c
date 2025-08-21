#include "bsp_system.h"
extern system_parameter sp;
int uart_num = 0;
char uart_buffer[20] = "";
int uart4_num = 0;
char uart4_buffer[10] = "";
char poccessing_data[20] = "";
int start_flag = 0;
int start4_flag = 0;
/* 调试串口 UART4 初始化 */
void uart_init()
{
    g_uart4.p_api->open(g_uart4.p_ctrl, g_uart4.p_cfg);   
	g_uart3.p_api->open(g_uart3.p_ctrl, g_uart3.p_cfg);
}
 
static volatile bool uart_send_complete_flag3 = false;

//串口屏控制
void HMI_control()
{
    strncpy(poccessing_data, uart_buffer, sizeof(poccessing_data)-1);
    //串口屏控制部分
    if(poccessing_data[1] <= 0x07)
    {
        sp.system_mode = poccessing_data[1] - 1;
    }
	else if(poccessing_data[1] == 0x08)
	{
		sp.relay_mode = 0;
		sp.auto_flag = 0;
	}
	else if(poccessing_data[1] == 0x09)
	{
		sp.relay_mode = 1;
		sp.auto_flag = 0;
	}
	else if(poccessing_data[1] == 0x0A)
	{
		sp.relay_mode = 2;
		sp.auto_flag = 0;
	}
	else if(poccessing_data[1] == 0x0B)
	{
		sp.relay_mode = 3;
		sp.auto_flag = 0;
	}
	else if(poccessing_data[1] == 0x0C)
	{
		sp.auto_flag = !sp.auto_flag;
	}
    else if(poccessing_data[1] == 0xFC)
	{
		sp.pow_max = poccessing_data[2] + poccessing_data[3] * 256;
	}
	 else if(poccessing_data[1] == 0xAB)
	{
		sp.classify_flag = !sp.classify_flag;
	}
    // 重置缓冲区，使用memset更安全
    memset(uart_buffer, 0, sizeof(uart_buffer));
    uart_num = 0;
    memset(poccessing_data, 0, sizeof(poccessing_data));
}

void uart3_callback (uart_callback_args_t* p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
        {
            if(p_args->data == 0xFD && start_flag == 0)
            {
                start_flag = 1;
                uart_num = 0; // 重置缓冲区索引
                memset(uart_buffer, 0, sizeof(uart_buffer)); // 清空缓冲区
            }
            
            if(start_flag)
            {
                uart_buffer[uart_num++] = p_args->data;
                if(p_args->data == 0xFA)
                {
                    start_flag = 0;
                    // 不直接调用HMI_control，仅设置标志位
                    HMI_control();
                }
            }
            break;
        }
        case UART_EVENT_TX_COMPLETE:
        {
            uart_send_complete_flag3 = true;
            break;
        }
        default:
            break;
    }
}

void uart4_proc()
{
	if(strstr((char *)uart4_buffer,"!off&")==0)sp.cut_flag=1;
	memset(uart4_buffer, 0, sizeof(uart4_buffer));
    uart4_num = 0;
}
static volatile bool uart_send_complete_flag4 = false;
void uart4_callback (uart_callback_args_t* p_args1){
	switch(p_args1->event)
    {
		case UART_EVENT_RX_CHAR:
		{
			if(p_args1->data == '!' && start4_flag == 0)
            {
                start4_flag = 1;
                uart4_num = 0; // 重置缓冲区索引
                memset(uart4_buffer, 0, sizeof(uart4_buffer)); // 清空缓冲区
            }
            
            if(start4_flag)
            {
                uart4_buffer[uart4_num++] = p_args1->data;
                if(p_args1->data == '&')
                {
                    start4_flag = 0;
                    uart4_proc();
                }
            }
		}
		case UART_EVENT_TX_COMPLETE:
		{
			uart_send_complete_flag4 = true;
			break;
		}
		default:
			break;
		}
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     串口字节输出
// 参数说明     uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// 参数说明     dat             需要发送的字节
// 返回参数     void        
// 使用示例     uart_write_byte(0xA5);                                  // 串口1发送0xA5
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void uart4_write_byte (const char dat)
{
    uart_send_complete_flag4 = false;
    g_uart4.p_api->write(g_uart4.p_ctrl, (const char*)&dat, 1);     // 发送到串口
    while(!uart_send_complete_flag4);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     串口发送数组
// 参数说明     uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// 参数说明     *buff           要发送的数组地址
// 参数说明     len             发送长度
// 返回参数     void
// 使用示例     uart_write_buffer(&a[0], 5);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void uart3_write_buffer (const char *buff, uint32_t len)
{
    uart_send_complete_flag3 = false;
    g_uart3.p_api->write(g_uart3.p_ctrl, (const char*)buff, len);     // 发送到串口
    while(!uart_send_complete_flag3);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     串口发送字符串
// 参数说明     uart_n          串口模块号 参照 zf_driver_uart.h 内 uart_index_enum 枚举体定义
// 参数说明     *str            要发送的字符串地址
// 返回参数     void
// 使用示例     uart_write_string("seekfree"); 
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void uart4_write_string (const char *str)
{
    while(*str)                                                                 // 一直循环到结尾
    {
        uart4_write_byte(*str ++);                                       // 发送数据
    }
}

/*
	串口解析函数 格式：
	FD XXXX FA
	1-操作 01-后退 02-前进
*/


//ESP8266发送
void uart_proc()
{
	
	char ESP8266_out[100] = "    ";
	char ESP8266_in[100] = "    ";
	
	
	/*ESP8266数据传输部分*/
	
	/*ESP8266数据接收部分*/
	sprintf(ESP8266_in,"AT+MQTTSUB=0,\"yuan/shen/qi/dong\",0\r\n");
	uart4_write_string(ESP8266_in);
	R_BSP_SoftwareDelay(50,BSP_DELAY_UNITS_MILLISECONDS);
	
	
	sprintf(ESP8266_out,"AT+MQTTPUB=0,\"nitamade/jiushe/ge/shabi\",\"A%0.2f?B%0.2f?C%0.2f?D%0.2f?E%0.2f?\",1,0\r\n",sp.volt_rms,sp.elect_rms,sp.input_freq,sp.apparent_power,sp.energy_comsumption*1.0/3600000);
	uart4_write_string(ESP8266_out);
	
	
}

  

/*重定向printf输出*/
#if defined__GNUC__ &&!defined__clang__
 int _write(int fd,char *pBuffer,int size);//防止编译警告
int _write(int fd,char *pBuffer,int size)
 {
 (void)fd;
 R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)pBuffer,(uint32_t)size);
 while(uart_send_complete_flag== false);
 uart_send_complete_flag= false;
 return size;
 }
 #else
int fputc(int ch, FILE *f)
{
	(void)f;
	uart3_write_buffer((uint8_t *)&ch, 1);
	while(uart_send_complete_flag3 == false);
	uart_send_complete_flag3 = false;
	return ch;
}
#endif