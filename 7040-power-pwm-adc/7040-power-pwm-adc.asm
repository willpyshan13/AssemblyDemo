;******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2020.09.25
;  *   	@Author              : pengyushan
;  *----------------------Abstract Description---------------------------------        	
;  	   	   	P54 1ms翻转,P53输出PWM,P40.P41.VDDAD采集  
;          	 
;           选零点校准
;          	 
;******************************************************************************

#include "MC35P7040.INC"

cblock 0x0000
r0x0001
timer0_count1:2
flag1
endc

#define	FLAG_TIMER0_5000ms 	flag1,0
        org    	0x0000
        goto   	MAIN   	   	
        org    	0x8

MAIN:
       	CALL   	Sys_Init

MAIN_LOOP:
;*****************AD采集**********************
   	   	MOVAR  	ADM
   	   	ANDAI  	0xf8   	   	   	
   	   	ORAI   	0x00   	;通道0   	   	P40
   	   	MOVRA  	ADM
   	   	CALL   	ADC_Get_Value  	
   	   	CALL   	ADC_Get_Value  	;切换通道舍去前面两次采集
   	   	CALL   	ADC_Get_Value  	;demo演示，实际使用客户自行滤波     	
   	   	MOVAR  	ADM
   	   	ANDAI  	0xf8   	   	   	
   	   	ORAI   	0x01   	;通道1   	   	P41
   	   	MOVRA  	ADM
   	   	CALL   	ADC_Get_Value  	
   	   	CALL   	ADC_Get_Value  	;切换通道舍去前面两次采集
   	   	CALL   	ADC_Get_Value  	;demo演示，实际使用客户自行滤波 
   	   	MOVAR  	ADM
   	   	ANDAI  	0xf8   	   	   	
   	   	ORAI   	0x05   	;通道VDD     	
   	   	MOVRA  	ADM
   	   	CALL   	ADC_Get_Value  	
   	   	CALL   	ADC_Get_Value  	;切换通道舍去前面两次采集
   	   	CALL   	ADC_Get_Value  	;demo演示，实际使用客户自行滤波 
;*****************休眠************************
   	   	;5000ms进入休眠
       	;JBSET  	FLAG_TIMER0_5000ms
       	GOTO   	MAIN_LOOP
       	;BCLR   	FLAG_TIMER0_5000ms
   	  ;    	休眠关闭外设
       	BCLR   	GIE
       	BCLR   	ADEN
       	BCLR   	PWM1OE
       	BCLR   	T1EN
       	CLRR   	IOP5
       	nop
       	MOVAI  	0xe7
       	ANDRA  	OSCM
       	BSET   	OSCM,3
       	nop   
       	nop
       	nop	
       	BSET   	GIE
       	BSET   	PWM1OE
       	BSET   	T1EN
   	   	 ; 唤醒后开启外设
       	BSET   	ADEN
       	GOTO   	MAIN_LOOP


ADC_Get_Value:
       	BCLR   	ADEOC
       	BSET   	ADSTR
       	JBSET  	ADEOC
       	GOTO   	$-1
       	RETURN 	
    ; exit point of _ADC_Get_Value
    
Sys_Init:
       	BCLR   	GIE
       	CALL   	CLR_RAM
       	CALL   	IO_Init
ADC_ADJ_INIT:
   	   	;        两个校准实际应用是二选一
   	   	;        偏0   选零点校准
   	   	;        偏vdd 选顶点校准 
       	CALL   	ADC_Zero_ADJ
       	MOVRA  	r0x0001
       	MOVAR   r0x0001
       	JBSET  	Z
       	GOTO   	ADC_ADJ_INIT   	   	;demo演示 ，此处校准失败一直校准，处于循环
       	CALL   	TIMER1_PWM_Init   
       	CALL   	ADC_Init
       	BSET   	GIE
       	RETURN 	

ADC_Zero_ADJ:
       	MOVAI  	0x80
       	MOVRA  	ADT	   	;零点校准   ADTR[4:0]=0
       	CLRR   	VREFCR 	    ;内部2V
       	CLRR   	ADR	   ;64分频
       	BSET   	GCHS
       	BSET   	ADEN
       	BCLR   	ADEOC
       	BSET   	ADSTR

       	JBSET  	ADEOC
       	GOTO   	$-1
    ;  	结果是否为0
       	MOVAR   ADB
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_ADD
       	MOVAR  	ADR
       	ANDAI  	0x0f
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_ADD
       	MOVAI  	0x1f
       	ORRA   	ADT
       	BCLR   	ADEOC
       	BSET   	ADSTR

       	JBSET  	ADEOC
       	GOTO   	$-1
    ;  	结果是否为0
       	MOVAR   ADB
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_DEC
       	MOVAR  	ADR
       	ANDAI  	0x0f
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_DEC
    ;  正常模式
       	MOVAI  	0x3f
       	ANDRA  	ADT  
   	   	BCLR   	ADEN
       	MOVAI  	0x00   	   	;PASS
       	GOTO   	ADC_ZERO_ADJ_END
ADC_ADJ_ZERO_DEC:
       	MOVAR  	ADT
       	ANDAI  	0x0f
       	JBCLR  	Z
       	GOTO   	ADC_ADJ_ZERO_FAIL1
       	DJZR   	ADT
       	NOP	
       	BCLR   	ADEOC
       	BSET   	ADSTR

       	JBSET  	ADEOC
       	GOTO   	$-1
    ;  结果是否为0
       	MOVAR   ADB
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_DEC
       	MOVAR  	ADR
       	ANDAI  	0x0f
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_DEC
    ;  	正常模式
       	MOVAI  	0x3f
       	ANDRA  	ADT
   	   	BCLR   	ADEN
       	MOVAI  	0x00   	;PASS
       	GOTO   	ADC_ZERO_ADJ_END
ADC_ADJ_ZERO_FAIL1:
   	   	MOVAI  	0x3f
       	ANDRA  	ADT	   	   	;正常模式
   	   	BCLR   	ADEN
       	MOVAI  	0x01   	;FAIL
       	GOTO   	ADC_ZERO_ADJ_END
ADC_ADJ_ZERO_ADD:
   	   	MOVAI  	0x0f
       	ANDAR  	ADT
       	XORAI  	0x0f
       	JBSET  	Z
       	GOTO   	$+2
       	GOTO   	ADC_ADJ_ZERO_FAIL1

       	MOVAI  	0x01
       	ADDRA  	ADT
       	BCLR   	ADEOC
       	BSET   	ADSTR

       	JBSET  	ADEOC
       	GOTO   	$-1
    ;  	结果是否为0
       	MOVAR   ADB
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_ADD
       	MOVAR  	ADR
       	ANDAI  	0x0f
       	JBSET  	Z
       	GOTO   	ADC_ADJ_ZERO_ADD
    ;   正常模式
       	MOVAI  	0x3f
       	ANDRA  	ADT
   	   	BCLR   	ADEN
       	MOVAI  	0x00   	;PASS
ADC_ZERO_ADJ_END:
       	RETURN 	
    ; exit point of _ADC_Zero_ADJ
    
ADC_Init:
       	CLRR   	ADM
       	MOVAI  	0x90
       	ORRA   	ADM
       	CLRR   	VREFCR 	    ;  	内部2V
       	CLRR   	ADR	    ;   adc时钟  Fcpu/64
       	RETURN 	
    ; exit point of _ADC_Init
TIMER1_PWM_Init:
       	MOVAI  	0xf1
       	MOVRA  	T1CR
    ;  开启T1    1分频  T1CLK内部时钟  开启PWM  256个周期
       	MOVAI  	0x80
       	MOVRA  	T1LDR
       	RETURN 	
    ; exit point of _TIMER1_PWM_Init 
TIMER0_INT_Init:
    ; T1CLK=FCPU/2      T0CLK=FCPU/2    关闭T0溢出唤醒
       	CLRR   	TMRCR
    ; 开启T0    32分频  T0CLK内部时钟  自动重载  关闭PWM
       	MOVAI  	0xa4
       	MOVRA  	T0CR
       	MOVAI  	0x83
       	MOVRA  	T0CNT
       	MOVAI  	0x83
       	MOVRA  	T0LDR  	   	;  1ms
       	BSET   	T0IE        ;开启t0中断
       	RETURN 	
    ; exit point of _TIMER0_INT_Init
    
IO_Init:
       	CLRR   	IOP0    
       	MOVAI  	0xfe
       	MOVRA  	OEP0   	   	;  io口方向 1:out  0:in
       	MOVAI  	0x01
       	MOVRA  	PUP0   	   	; io口上拉电阻   1:enable  0:disable
       	CLRR   	IOP4
       	MOVAI  	0xfc
       	MOVRA  	OEP4   	   	;  io口方向 1:out  0:in
       	CLRR   	PUP4   	   	; io口上拉电阻   1:enable  0:disable
       	CLRR   	IOP5   
       	MOVAI  	0xff
       	MOVRA  	OEP5   	   	; io口方向 1:out  0:in
       	CLRR   	PUP5       	; io口上拉电阻   1:enable  0:disable
       	MOVAI  	0x03
       	MOVRA  	P4CON  	   	; P40 P41模拟
       	RETURN 	
    ; exit point of _IO_Init
CLR_RAM:
       	movai  	0x3F
       	movra  	FSR0
       	clrr   	FSR1
       	clrr   	INDF
       	DJZR   	FSR0
       	goto   	$ -2
       	clrr   	INDF
       	RETURN 	
    ; exit point of _CLR_RAM
       	end


