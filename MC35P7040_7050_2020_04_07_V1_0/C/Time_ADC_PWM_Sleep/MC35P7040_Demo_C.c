/******************************************************************************
;  *   	@MCU   	   	   	   	 : MC35P7040
;  *   	@Create Date         : 2019.12.29
;  *   	@Author/Corporation  : Martin/SinoMCU
;  *   	@技术支持QQ群   	   	  : 814641858      
;  *    @晟矽微官方论坛   	   	  : http://bbs.sinomcu.com/	 
;  *   	@Copyright           : 2019 SINOMCU Corporation,All Rights Reserved.
;  *----------------------Abstract Description--------------------------------- 
                        T0定时1mS中断 P54翻转
                        T1（P53）输出PWM
                        T0定时5S进入休眠，P00唤醒
                        P40.P41.VDD AD采集  
                        两个校准实际应用是二选一
                        偏0   选零点校准
                        偏vdd 选顶点校准
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
    OEP0 = 0xfe; //io口方向 1:out  0:in
    PUP0 = 0x01; //io口上拉电阻   1:enable  0:disable

    IOP4 = 0x00; //io口数据位
    OEP4 = 0xfC; //io口方向 1:out  0:in
    PUP4 = 0x00; //io口上拉电阻   1:enable  0:disable

    IOP5 = 0x00; //io口数据位
    OEP5 = 0xff; //io口方向 1:out  0:in
    PUP5 = 0x00; //io口上拉电阻   1:enable  0:disable

    P4CON = 0x03;  	   	//P40 P41模拟
}
/************************************************
;  *    @Function Name       : TIMER0_INT_Init
;  *    @Description         : 定时器初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/
void TIMER0_INT_Init(void)
{
    TMRCR = 0;   //T1CLK=FCPU/2      T0CLK=FCPU/2    关闭T0溢出唤醒
    T0CR = 0xA4; //开启T0    32分频  T0CLK内部时钟  自动重载  关闭PWM
    T0CNT = 256 - 125;
    T0LDR = 256 - 125; // 1ms
    T0IE = 1;
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
;  *    @Function Name       : ADC_Config
;  *    @Description         : ADC初始化
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    :
;  ***********************************************/
void ADC_Init(void)
{
    ADM &= 0x00;
    ADM |= 0x90;
    VREFCR = 0; //内部2V
    ADR = 0;    //adc时钟  Fcpu/64
}
/************************************************
;  *    @Function Name       : ADC_Zero_ADJ
;  *    @Description         : ADC零点校准
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    : 0 校准成功  1校准失败
;  ***********************************************/
uint8_t ADC_Zero_ADJ(void)
{
    //零点校准
    ADT = 0x80; //零点校准   ADTR[4:0]=0
    VREFCR = 0; //内部2V
    ADR = 0;    //64分频
    GCHS = 1;
    ADEN = 1;

    ADEOC = 0;
    ADSTR = 1;
    while (!ADEOC);
    if ((ADB == 0) && ((ADR & 0x0F) == 0)) //结果是否为0
    {
        ADT |= 0x1F;
        ADEOC = 0;
        ADSTR = 1;
        while (!ADEOC);
        if ((ADB == 0) && ((ADR & 0x0F) == 0)) //结果是否为0
        {
            ADT &= 0x3f; //正常模式
            ADEN=0;
            return PASS;         
        }
        else
        {
            while (1)
            {
                if (ADT & 0x0f)
                {
                    ADT--;
                    ADEOC = 0;
                    ADSTR = 1;
                    while (!ADEOC);
                    if ((ADB == 0) && ((ADR & 0x0F) == 0)) //结果是否为0
                    {
                        ADT &= 0x3f; //正常模式
                        ADEN=0;
                        return PASS;
                    }
                }
                else 
                {
                    ADT &= 0x3f; //正常模式
                    ADEN=0;
                    return FAIL;
                }
            }
        }
    }
    else
    {
        while (1)
        {
            if ((ADT & 0x0f) == 0x0f) 
            {
                ADT &= 0x3f; //正常模式
                ADEN=0;
                return FAIL;
            }
            else
            {
                ADT++;
                ADEOC = 0;
                ADSTR = 1;
                while (!ADEOC);
                if ((ADB == 0) && ((ADR & 0x0F) == 0)) //结果是否为0
                {
                    ADT &= 0x3f; //正常模式
                    ADEN=0;
                    return PASS;
                }
            }
        }
    }
}
/************************************************
;  *    @Function Name       : ADC_Vertex_ADJ
;  *    @Description         : ADC顶点校准
;  *    @IN_Parameter      	 : 
;  *    @Return parameter    : 0 校准成功  1校准失败
;  ***********************************************/
uint8_t ADC_Vertex_ADJ(void)
{
    //顶点校准
    ADT = 0xD0; //顶点校准   ADTR[4:0]=0x10
    VREFCR = 0; //内部2V
    ADR = 0;    //64分频
    GCHS = 1;
    ADEN = 1;

    ADEOC = 0;
    ADSTR = 1;
    while (!ADEOC);
    if ((ADB == 0xff) && ((ADR & 0x0F) == 0x0f)) //结果是否为0x0fff
    {
        ADT &= 0xE0;
        ADT |= 0x0F;
        ADEOC = 0;
        ADSTR = 1;
        while (!ADEOC);
        if ((ADB == 0xff) && ((ADR & 0x0F) == 0x0f)) //结果是否为0x0fff
        {
            ADT &= 0x3f; //正常模式
            ADEN=0;
            return PASS;
        }
        else
        {
            while (1)
            {
                if (ADT & 0x0f)
                {
                    ADT--;
                    ADEOC = 0;
                    ADSTR = 1;
                    while (!ADEOC);
                    if ((ADB == 0xff) && ((ADR & 0x0F) == 0x0f)) //结果是否为0x0fff
                    {
                        ADT &= 0x3f; //正常模式
                        ADEN=0;
                        return PASS;
                    }
                }
                else 
                {
                    ADT &= 0x3f; //正常模式
                    ADEN=0;
                    return FAIL;
                }
            }
        }
    }
    else
    {
        while (1)
        {
            if ((ADT & 0x0f) == 0x0f) 
            {
                ADT &= 0x3f; //正常模式
                ADEN=0;
                return FAIL;
            }
            else
            {
                ADT++;
                ADEOC = 0;
                ADSTR = 1;
                while (!ADEOC);
                if ((ADB == 0xff) && ((ADR & 0x0F) == 0x0f)) //结果是否为0x0fff
                {
                    ADT &= 0x3f; //正常模式
                    ADEN=0;
                    return PASS;
                }
            }
        }
    }
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
/**************************
     两个校准实际应用是二选一
    偏0   选零点校准
    偏vdd 选顶点校准 
***************************/
    while(ADC_Zero_ADJ()); 	   	//demo演示,校准失败一直校准
  //  while(ADC_Vertex_ADJ());        //ADC校准
    TIMER0_INT_Init();
    TIMER1_PWM_Init();
    ADC_Init();
    GIE = 1;
}
/************************************************
;  *    @Function Name       : ADC_Get_Value_Average
;  *    @Description         : 连续转换
;  *    @IN_Parameter      	 : 通道
;  *    @Return parameter    : 返回ADC的值
   	   	  函数占用内存较大（180+字节），
;  ***********************************************/
uint ADC_Get_Value_Average(u8 CHX)
{
    u16 temp_value = 0;
    u8 channel = 0;
    unsigned long int tmpbuff = 0;
    u16 ADCMAX = 0;
    u16 ADCMIN = 0xffff;

    ADM = (ADM & 0xf8) | CHX; // ADC 使能  AD转换通道开启  通道  CHX
    for (channel = 0; channel < 20; channel++)
    {
        ADEOC = 0;
        ADSTR = 1; // 开始转换
        while (!ADEOC); //等待转换完成
        temp_value = ADB;
        temp_value = temp_value << 4 | (ADR & 0x0f);
        if (channel < 2)
            continue; //丢弃前两次采样的
        if (temp_value > ADCMAX)
            ADCMAX = temp_value; //最大
        if (temp_value < ADCMIN)
            ADCMIN = temp_value; //最小
        tmpbuff += temp_value;
    }
    tmpbuff -= ADCMAX;           //去掉一个最大
    tmpbuff -= ADCMIN;           //去掉一个最小
    temp_value = (tmpbuff >> 4); //除以16，取平均值
    return temp_value;
}
void main(void)
{
    Sys_Init();
    while (1)
    {
        ADC_num_value = ADC_Get_Value_Average(0);
        ADC_num_value = ADC_Get_Value_Average(1);
        ADC_num_value = ADC_Get_Value_Average(5); //VDD/4

        if (FLAG_TIMER0_5000ms)
        {
            //写法一
            FLAG_TIMER0_5000ms = 0;
            GIE = 0; //休眠关闭外设
            ADEN = 0;
   	   	   	PWM1OE=0;
   	   	   	T1EN=0;
            IOP5 = 0;
            Nop();
            OSCM &= 0xE7;
            OSCM |= 0x08;
            Nop();
            Nop();
            Nop();
            GIE = 1;
   	   	   	PWM1OE=1;
   	   	   	T1EN=1;
            ADEN = 1; //唤醒后开启外设

            /*写法二
            FLAG_TIMER0_5000ms=0; 
   	   	   	GIE = 0; //休眠关闭外设
            ADEN = 0;
   	   	   	PWM1OE=0;
   	   	   	T1EN=0;
            IOP5 = 0;
   	       	HFDE=1;
   	   	   	Nop();
   	   	   	Nop();
   	   	   	Nop();
   	        HFDE=0;
   	       	GIE = 1;
   	   	   	PWM1OE=1;
   	   	   	T1EN=1;
            ADEN = 1; //唤醒后开启外设  */
        }
    }
}

void int_isr(void) __interrupt
{
    __asm 
    push
    __endasm;
    if (T0IF && T0IE)
    {
        T0IF = 0;
        P54D = !P54D;
        timer0_count1++;
        if (timer0_count1 >= 5000)
        {
            timer0_count1 = 0;
            FLAG_TIMER0_5000ms = 1;
        }
    }

    __asm 
    pop
    __endasm;
}
