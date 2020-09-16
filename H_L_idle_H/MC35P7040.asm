;******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2020.01.09
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	  : 814641858      
;  *    @晟矽微官方论坛   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description---------------------------------    	
;  *            高频-低频-空闲模式-低频-高频-IO翻转 2S T0唤醒一次，  
;                P01翻转200次输出
;                P54D 2S翻转
;                模式切换可根据数据手册 模式切换示意图 修改；
;******************************************************************************
#include "MC35P7040.INC"

num	EQU	0x00

   	ORG     0x0000
   	goto   	MAIN
    ORG     0x08
INT_ISR:
   	push
   	JBSET  	T0IF
   	GOTO   	TIMER0_END
   	JBSET  	T0IE
   	GOTO   	TIMER0_END
   	BCLR   	T0IF
    MOVAI   00000100B
   	XORRA   IOP0   	   	    ;  P03D取反 
    MOVAI  	00010000B
   	XORRA  	IOP5   	   	    ;  P54D取反  
    MOVAI   00000001B
   	XORRA   IOP4   	   	    ;  P40D取反      	
TIMER0_END:
   	pop
   	RETIE  	

MAIN:
   	CALL   	Sys_Init
MAIN_LOOP:
   	BCLR   	ADEN    ;关闭外设  
   	BSET   	CLKS    ;切换到低频 4t
   	nop
   	BSET   	HFDE   	   	   	   	   	   
   	MOVAI  	256-125
   	MOVRA  	T0CNT
   	BSET   	T0EN
   	BSET   	T0IE
   	CLRR   	INTF
   	nop
   	nop
   	MOVAI  	0xe7
   	ANDRA  	OSCM
   	BSET   	OSCM,4 ;绿色模式
   	nop
   	nop    	   	  
   	nop
   	BCLR   	T0IE
   	BCLR   	T0EN
   	BCLR   	HFDE    ;打开高频  (用户自己选择)
   	nop
   	nop
   	BCLR   	CLKS    ;切换到高频(用户自己选择)
   	BSET   	ADEN    ;开启相应的外设  (用户自己选择)
    CLRR    num
IO_OUT_P01:
    JZR    num
    nop
    MOVAI  	19
    ASUBAR 	num
    JBSET  	C
    GOTO   	MAIN_LOOP 
    MOVAI  	00000010B
   	XORRA  	IOP0   	   	    ;  P01D取反
   	MOVAI  	00000010B
   	XORRA  	IOP4   	   	    ;  P44D取反
   	nop
   	nop
   	nop
   	nop
   	nop
   	nop
   	nop
   	nop
   	GOTO   	IO_OUT_P01

Sys_Init:
   	BCLR   	GIE
   	CALL   	CLR_RAM
   	CALL   	IO_Init
   	CALL   	TIMER0_INT_Init  
   	BSET   	GIE
   	RETURN 	
; exit point of _Sys_Init

TIMER0_INT_Init:
   	MOVAI  	0x06
   	MOVRA  	TMRCR   ;T1CLK=FCPU/2      T0CLK=Fosc    允许T0溢出唤醒
   	MOVAI  	0x14
   	MOVRA  	T0CR   ;开启T0    128分频  T0CLK内部时钟  自动重载  关闭PWM
   	MOVAI  	0x83
   	MOVRA  	T0CNT  ;T0CNT=256-125;
   	MOVAI  	0x83
   	MOVRA  	T0LDR  ;进入低频后   32k/4/128=2s    低频模式固定4t
   	RETURN 	
; exit point of _TIMER0_INT_Init

IO_Init:   	
   	CLRR   	IOP0
   	MOVAI  	0xff
   	MOVRA  	OEP0   ;io口方向 1:out  0:in  
   	MOVAI  	0x00
   	MOVRA  	PUP0   ;io口上拉电阻   1:enable  0:disable
   	CLRR   	IOP4   ;io口数据位
   	MOVAI  	0xff
   	MOVRA  	OEP4   ;io口方向 1:out  0:in   
   	CLRR   	PUP4   ;io口上拉电阻   1:enable  0:disable
   	CLRR   	IOP5   ;io口数据位
   	MOVAI  	0xff    ;io口方向 1:out  0:in  
   	MOVRA  	OEP5
   	CLRR   	PUP5   ;io口上拉电阻   1:enable  0:disable
   	;MOVAI  	0xff
   	;MOVRA  	P4CON
   	RETURN 	
; exit point of _IO_Init

CLR_RAM:   	
   	movai 0x01
   	movra FSR0
   	clrr FSR1
   	clrr INDF
   	DJZR FSR0
   	goto $-2
   	clrr INDF
   	RETURN 	
; exit point of _CLR_RAM


   	end
