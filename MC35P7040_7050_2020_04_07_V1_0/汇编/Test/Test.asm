#include "MC35P7040.INC"

#define	   	led	   	IOP0,0
#define	   	key	   	IOP0,1

ORG   	0x03ff
GOTO  	start  	
ORG   	0x00
GOTO  	start

start:
    CLRR   	IOP0 	   	   	;清P0和P1口
   	MOVAI  	   	0x00   	   	   	
    MOVRA  	IOP0   	   	;设置P0口的端口方向
    MOVAI  	0x01
    MOVRA  	IOP0    	   	;设置P1口的端口方向
    MOVAI  	0xfe
    MOVAI  	PUP0  	   	;设置上拉电阻
check:
    JBCLR  	key	   	   	;这里只是简单的判断按键有没有按下，未进行消抖


    GOTO   	close
    GOTO   	open
close:    
    BCLR   	led
    GOTO   	check
open:
   	BSET   	led
   	GOTO   	check
   	END	