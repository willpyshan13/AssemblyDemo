;定时器的使用，
#include "MC35P7040.INC"
;
cblock  0x10
acctemp	   	   	;表示0x10地址，等同于 acctemp     equ  	   	0x10
statustemp 	   	;表示0x11地址，等同于 statustemp  equ  	0x11
endc

 org  0x3ff
 goto start        
 org  00h
 goto start        
 org  08h      	
timer0int: movra  acctemp
       	   swapar STATUS
       	   movra  statustemp
       	   bclr   T0IF
           movai  6
           movra  T0CNT
           xorra	  P1
        
timer0exit: swapar statustemp
            movra  STATUS
       	    swapr  acctemp
       	    swapar acctemp
            retie
            
start:
;========清除0x10到0x3F（通用寄存器）里的数据=======
          movai       0x3f
          movra       FSR
          movai       47
          movra       GPR
          djzr        FSR
          clrr        INDF
          djzr        GPR
          goto        $-3
          clrr        INDF
          clrr        GPR
;=============I/O====================== 
   	   	  clrr 	   	P1
   	   	movai  	   	  0x00
   	   	  movra	   	DDR1
;==============timer===================
   	   	   movai   	   	  6
   	   	  movra	   	  T0CNT
   	   	  movai	   	0x07
   	   	  movra	   	  T0CR
;==============开中断=====================
   	   	  bset 	   	  GIE  	   	;开总中断
   	   	  bset 	   	T0IE   	;开定时器中断
main:
   	   	  goto 	   	$  	   	   	;一直在此循环，相当于初始化完成之后，就停在这里
   end