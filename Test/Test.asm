#include "MC35P7040.INC"

#define	   	led	   	IOP0,0
#define	   	key	   	IOP0,1

ORG   	0x03ff
GOTO  	start  	
ORG   	0x00
GOTO  	start

start:
    CLRR   	IOP0 	   	   	;��P0��P1��
   	MOVAI  	   	0x00   	   	   	
    MOVRA  	IOP0   	   	;����P0�ڵĶ˿ڷ���
    MOVAI  	0x01
    MOVRA  	IOP0    	   	;����P1�ڵĶ˿ڷ���
    MOVAI  	0xfe
    MOVAI  	PUP0  	   	;������������
check:
    JBCLR  	key	   	   	;����ֻ�Ǽ򵥵��жϰ�����û�а��£�δ��������


    GOTO   	close
    GOTO   	open
close:    
    BCLR   	led
    GOTO   	check
open:
   	BSET   	led
   	GOTO   	check
   	END	