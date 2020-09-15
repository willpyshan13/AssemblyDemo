/******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2019.12.29
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	   	  : 814641858      
;  *    @晟矽微官方论坛   	   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description--------------------------------- 
   	   	   	   	     	烧录时选择LVD3.6V 
					低于3.6V时LVD36标志置1 P01输出高；反之输出低，
					LVD说明：
					LVD检测值在烧录时烧写固定；
					使用注意LVD误差
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
   	GIE=1;        
}
void main(void)
{
   	Sys_Init();
   	while(1)
   	{
   	   	if(LVD36)  	   	//LVD烧录选择3.6V
   	   	{
   	   	   	P01D=1;
   	   	}else P01D=0;   
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




















