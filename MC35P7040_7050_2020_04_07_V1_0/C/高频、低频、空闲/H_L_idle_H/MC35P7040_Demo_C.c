/******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2019.12.29
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	   	  : 814641858      
;  *    @晟矽微官方论坛   	   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description---------------------------------    	
;  *            高频-低频-空闲模式-低频-高频-IO翻转 2S T0唤醒一次，  
                P01翻转200次输出
                P54D 2S翻转
                模式切换可根据数据手册 模式切换示意图 修改；
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
   	movai  	0x3F
   	movra  	FSR0
   	clrr   	FSR1
   	clrr   	INDF
   	DJZR    FSR0
   	goto   	$-2
   	clrr   	INDF
__endasm;
}
/************************************************
;  *    @Function Name       : IO_Init
;  *    @Description         : 
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/  
void IO_Init(void)
{
   	IOP0=0x00; 	   	   	//io口数据位
   	OEP0=0xff; 	   	   	//io口方向 1:out  0:in  
   	PUP0=0x00; 	   	   	//io口上拉电阻   1:enable  0:disable
   	
   	IOP4=0x00; 	   	   	//io口数据位
   	OEP4=0xff; 	   	   	//io口方向 1:out  0:in   
   	PUP4=0x00; 	   	   	//io口上拉电阻   1:enable  0:disable
   	
   	IOP5=0x00; 	   	   	//io口数据位
   	OEP5=0xff; 	   	   	//io口方向 1:out  0:in  
   	PUP5=0x00; 	   	   	//io口上拉电阻   1:enable  0:disable

   	P4CON=0xff;
}
/************************************************
;  *    @Function Name       : TIMER0_INT_Init
;  *    @Description         : 定时器初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/  
void TIMER0_INT_Init(void)     	   	
{
    TMRCR=0x06;            //T1CLK=FCPU/2      T0CLK=Fosc    允许T0溢出唤醒
    T0CR=0x04;          //开启T0    128分频  T0CLK内部时钟  自动重载  关闭PWM
    T0CNT=256-125;
    T0LDR=256-125;     // 进入低频后   32k/4/128=2s      //低频模式固定4t
   //  	T0IE=1;
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
   	TIMER0_INT_Init();
   	GIE=1;        
}
void main(void)
{
   	u8 num;
   	Sys_Init();
   	while(1)
   	{
   	    ADEN=0;	   	   	 //关闭外设    	
   	   	CLKS=1;	   	//切换到低频 4t
   	   	Nop();
   	   	HFDE=1;	   	//关闭高频
   	   	//开启唤醒条件(部分在初始化已经配置)
   	   	T0CNT=256-125;     	   	   	   	   	   	   
   	   	T0EN=1;
   	   	T0IE=1;
   	   	INTF=0;
       	Nop();
   	   	Nop();
   	   	OSCM&=0xE7;   
   	   	OSCM|=0x10;	   	   	   	   	//绿色模式
   	   	Nop();
   	   	Nop();
   	   	Nop(); 	   	   	  
   	   	T0IE=0;
   	   	T0EN=0;
       	HFDE=0;	   	//打开高频  (用户自己选择)
   	   	Nop();
   	   	Nop();
       	CLKS=0;	   	//切换到高频	   	(用户自己选择)

   	   	ADEN=1;	   	//开启相应的外设  (用户自己选择)
   	   	
   	   	for(num=0;num<10;num++)
   	   	{
   	   	   	P01D=!P01D;
   	   	   	Nop();
   	   	   	Nop();
   	   	   	Nop();
            Nop();
   	   	   	Nop();
   	   	   	Nop();
            Nop();
   	   	   	Nop();
   	   	}
   	}
}
void int_isr(void) __interrupt  
{
   	__asm
   	   	push
   	__endasm;
    if(T0IF&&T0IE)
   	{
   	   	T0IF=0;
        P54D=!P54D;
   	}
   	__asm
   	   	pop
   	__endasm;
}




















