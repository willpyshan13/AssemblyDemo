/******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7050
;  *   	@Create Date         : 2019.12.29
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	   	  : 814641858      
;  *    @晟矽微官方论坛   	   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description--------------------------------- 
       	    	P44、P53 PWM输出   	   	
   	    	7050仿真使用7040仿真板
            输出PWM需要开启映射功能
******************************************************************************/

#include "user.h"

/************************************************
;  *    @Function Name       : CLR_RAM
;  *    @Description         : 
;  *    @IN_Parameter        :
;  *    @Return parameter    :
;  ***********************************************/
void CLR_RAM(void)
{
__asm 
    movai 0x3F 
    movra FSR0
    clrr FSR1
    clrr INDF
    DJZR FSR0 
    goto $ -2
    clrr INDF
__endasm;
}
/************************************************
;  *    @Function Name       : IO_Config
;  *    @Description         : io口初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/
void IO_Init(void)
{
    IOP0 = 0x00; //io口数据位
    OEP0 = 0xff; //io口方向 1:out  0:in
    PUP0 = 0x00; //io口上拉电阻   1:enable  0:disable

    IOP4 = 0x00; //io口数据位
    OEP4 = 0xff; //io口方向 1:out  0:in
    PUP4 = 0x00; //io口上拉电阻   1:enable  0:disable

    IOP5 = 0x00; //io口数据位
    OEP5 = 0xff; //io口方向 1:out  0:in
    PUP5 = 0x00; //io口上拉电阻   1:enable  0:disable

    P4CON = 0x00;
}
/************************************************
;  *    @Function Name       : TIMER0_PWM_Init
;  *    @Description         : 定时器初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/
void TIMER0_PWM_Init(void)
{
    TMRCR = 0;   //T1CLK=FCPU/2      T0CLK=FCPU/2    关闭T0溢出唤醒
    T0CR = 0xF1; //开启T0 1分频  T0CLK内部时钟  自动重载  开启PWM  256时钟
    //T0CNT = 256 - 125;
    T0LDR = 0x7f; 
}
/************************************************
;  *    @Function Name       : TIMER1_PWM_Init
;  *    @Description         : 
;  *    @IN_Parameter        :
;  *    @Return parameter    :
;  ***********************************************/
void TIMER1_PWM_Init(void)
{
    T1CR = 0xf1;
    T1LDR = 0x80; //开启T1    1分频  T1CLK内部时钟  开启PWM  256个周期
}
/************************************************
;  *    @Function Name       : Sys_Init
;  *    @Description         : 系统初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/
void Sys_Init(void)
{
    GIE = 0;
    CLR_RAM();
    IO_Init();
    TIMER0_PWM_Init();
    TIMER1_PWM_Init();
    GIE = 1;
}
void main(void)
{
    Sys_Init();
    while (1)
    {

    }
}

void int_isr(void) __interrupt
{
    __asm 
    push
    __endasm;

    __asm 
    pop
    __endasm;
}
