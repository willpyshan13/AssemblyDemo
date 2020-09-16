;******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2020.01.09
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	  : 814641858      
;  *    @晟矽微官方论坛   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description---------------------------------    	
;  *           P00,P01上拉，外部中断下降沿触发，P02、P03翻转（未消抖处理）
;******************************************************************************
#include "MC35P7040.INC"   

    ORG    	0x0000
   	goto   	MAIN
   	ORG	   	0x8

INT_ISR:
   	push
   	JBSET  	INT0IF
   	GOTO   	INT1_INTERR
   	JBSET  	INT0IE
   	GOTO   	INT1_INTERR
   	BCLR   	INT0IF
    MOVAI  	00010000B
   	XORRA  	IOP5   	   	    ;  P54D取反


INT1_INTERR:
   	JBSET  	INT1IF
   	GOTO   	INT_END
   	JBSET  	INT1IE
   	GOTO   	INT_END
   	BCLR   	INT1IF
    MOVAI  	00001000B
   	XORRA  	IOP0   	   	    ;  P03D取反

INT_END:
   	RETIE  	

MAIN:  	
   	CALL   	Sys_Init
MAIN_LOOP:
   	GOTO   	MAIN_LOOP
   	RETURN 	
; exit point of main

Sys_Init:
   	BCLR   	GIE
   	CALL   	CLR_RAM
   	CALL    IO_Init
   	CALL   	INT_Init  
   	BSET   	GIE
   	RETURN 	
; exit point of Sys_Init

INT_Init:  	
   	BSET   	INT0IE
   	BSET   	INT1IE  ;使能外部中断
   	MOVAI  	0x10    
   	MOVRA  	EINTCR  ;下降沿触发
   	RETURN 	
; exit point of INT_Init

IO_Init:
   	CLRR   	IOP0    ;io口数据位
   	MOVAI  	0xfc
   	MOVRA  	OEP0    ;io口方向 1:out  0:in  
   	MOVAI  	0x03
   	MOVRA  	PUP0    ;io口上拉电阻   1:enable  0:disable
   	CLRR   	IOP4    ;io口数据位
   	MOVAI  	0xf0
   	MOVRA  	OEP4    ;io口方向 1:out  0:in   
   	CLRR   	PUP4    ;io口上拉电阻   1:enable  0:disable
   	CLRR   	IOP5    ;io口数据位
   	MOVAI  	0xff
   	MOVRA  	OEP5    ;io口方向 1:out  0:in  
   	CLRR   	PUP5    ;io口上拉电阻   1:enable  0:disable
   	MOVAI  	0xff
   	MOVRA  	P4CON
   	RETURN 	
; exit point of IO_Init

CLR_RAM:
   	movai 0x3F
   	movra FSR0
   	clrr FSR1
   	clrr INDF
   	DJZR FSR0
   	goto $-2
   	clrr INDF
   	RETURN 	
; exit point of CLR_RAM


   	end
