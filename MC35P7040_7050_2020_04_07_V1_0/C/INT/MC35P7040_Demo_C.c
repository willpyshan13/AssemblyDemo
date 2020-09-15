/******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2019.12.29
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	   	  : 814641858      
;  *    @晟矽微官方论坛   	   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description--------------------------------- 
       	P00,P01上拉，外部中断下降沿触发，P02、P03翻转（未消抖处理）

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
   	OEP0=0xfC; 	   	   	//io口方向 1:out  0:in  
   	PUP0=0x03; 	   	   	//io口上拉电阻   1:enable  0:disable
   	
   	IOP4=0x00; 	   	   	//io口数据位
   	OEP4=0xf0; 	   	   	//io口方向 1:out  0:in   
   	PUP4=0x00; 	   	   	//io口上拉电阻   1:enable  0:disable
   	
   	IOP5=0x00; 	   	   	//io口数据位
   	OEP5=0xff; 	   	   	//io口方向 1:out  0:in  
   	PUP5=0x00; 	   	   	//io口上拉电阻   1:enable  0:disable

   	P4CON=0xff;
}
/************************************************
;  *    @Function Name       : INT_Init
;  *    @Description         : 
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/  
void INT_Init(void)    	   	
{
    INT0IE=1;
   	INT1IE=1;       //使能外部中断
    EINTCR=0x10;    //下降沿触发
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
   	INT_Init();
   	GIE=1;        
}
void main(void)
{
   	Sys_Init();
   	while(1)
   	{
   	}
}
void int_isr(void) __interrupt  
{
   	__asm
   	   	push
   	__endasm;
   	if(INT0IF&&INT0IE)
   	{
   	   	INT0IF=0;
   	   	P02D=!P02D;
   	}
   	if(INT1IF&&INT1IE)
   	{
   	   	INT1IF=0;
   	   	P03D=!P03D;
   	}
   	__asm
   	   	pop
   	__endasm;
}




















