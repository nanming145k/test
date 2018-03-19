
#include <stdio.h>
#include "N76E003.h"
#include "Define.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "hal.h"
#include "rcvpkt.h"
bit BIT_TMP;
unsigned char bridge=0;
/******************flash**************************/
#if CALIB
#define  PAGE_ERASE_AP 0x22
#define  BYTE_PROGRAM_AP  0x21
#define  CALIADDRESSH	0x47
#define  CALIADDRESSL 0x80
volatile unsigned char code Data_Flash[4] _at_ 0x4780;
void WriteCC(u8 addr, u8 dat)
{
	IAPCN = BYTE_PROGRAM_AP;  // Program 201h with 55h
	IAPAH = CALIADDRESSH;
	IAPAL = CALIADDRESSL+addr;
	IAPFD = dat;	
	set_IAPGO;
}
void Flash(u8 rpH,u8 rpL,u8 paH,u8 paL)
{
	TA = 0xAA;  //CHPCON is TA protected
	TA = 0x55;
	CHPCON |= 0x01; //IAPEN = 1, enable IAP mode
	TA = 0xAA;  //IAPUEN is TA protected
	TA = 0x55;
	IAPUEN |= 0x01; //APUEN = 1, enable APROM update
	IAPCN = PAGE_ERASE_AP;  //Erase page 200h~27Fh
	IAPAH = CALIADDRESSH;
	IAPAL = CALIADDRESSL;
	IAPFD = 0xFF;
	TA = 0xAA;  //IAPTRG is TA protected
	TA = 0x55;
	IAPTRG |= 0x01;  //write to IAPGO to trigger IAP process
	
	WriteCC(0,rpH);
	WriteCC(1,rpL);
	WriteCC(2,paH);
	WriteCC(3,paL);
	
	TA = 0xAA;  //IAPUEN is TA protected
	TA = 0x55;
	IAPUEN &= ~0x01; //APUEN = 0, disable APROM update
	TA = 0xAA;  //CHPCON is TA protected
	TA = 0x55;
	CHPCON &= ~0x01; //IAPEN = 0, disable IAP mode	
}
#endif
/*************************PWM***************************/
void InitPWM(void)
{
	P00_Quasi_Mode;	   //PWM3
	P10_Quasi_Mode;	   //PWM2
	P1SR|=0x01;
	P0SR|=0x01;

	set_SFRPAGE;
	PWMINTC=0x02;
	clr_SFRPAGE;

	PWM2_P10_OUTPUT_ENABLE;
	PWM3_P00_OUTPUT_ENABLE;
  	PWM_IMDEPENDENT_MODE;
	PWM3_OUTPUT_INVERSE;
	PWMPH=0x03;
	PWMPL=0xfe;
	PWM2H=0;
	PWM2L=0;
	PWM3H=0;
	PWM3L=0;

	set_LOAD;
}

void SetPWM(uint32_t fre,uint16_t duty)
{
	uint32_t clk = 16000000;
	uint16_t temp;
	uint16_t temp1;
	temp=clk/fre;
  	temp1=(uint32_t)temp*duty/1000;
	PWMPH=temp>>8;
	PWMPL=temp;
	PWM2H =temp1>>8;
	PWM2L =temp1;
	temp1=temp-temp1;
	PWM3H =temp1>>8;
	PWM3L =temp1;
  	set_LOAD;
}

void PWMEn(u8 type)
{
	if(type==0)
	{
		clr_PWMRUN;
		PWM3_P00_OUTPUT_DISABLE;
		PWM2_P10_OUTPUT_DISABLE;
		set_LOAD;
		P00=0;
		P10=0;
		bridge=0;
	}
	else if(type==1)			   //P00是电容端
	{
		PWM3_P00_OUTPUT_DISABLE;
		PWM2_P10_OUTPUT_ENABLE;
		set_LOAD;
		set_PWMRUN;
		P00=0;
		bridge=1;
	}
	else if(type==2)
	{  	
	    PWM3_P00_OUTPUT_ENABLE;
		PWM2_P10_OUTPUT_ENABLE;
		set_LOAD;
		set_PWMRUN;
		bridge=2;
	}
}

void PWMSet(void)
{
//  u32 clk = 16000000;
  u16 duty = power_ctrl_pram.duty_cycle;
  u32 fre = power_ctrl_pram.fre;

  if(duty>OPREATE_RANGE_DUTY_CYCLE_MAX)
  {
	//				fre-=(duty-OPREATE_RANGE_DUTY_CYCLE_MAX)*100;
          duty=OPREATE_RANGE_DUTY_CYCLE_MAX;
#if APPLEQC
		if((Apple)&&(QC==2))
		{
			power_ctrl_pram.ctrl_type=PID_TYPE_DUTY;
			if(bridge==1)
			{
//				printf("fb\n");
				PWMEn(2);
				duty=250;
			}				
		}
		else
#endif			
       power_ctrl_pram.ctrl_type=PID_TYPE_FRE;
  }

  if(duty<OPREATE_RANGE_DUTY_CYCLE_MIN)
  {
          duty=OPREATE_RANGE_DUTY_CYCLE_MIN;
              if((bridge==2)&&(QC==0))
              {
                fre=120000;
								duty=OPREATE_RANGE_DUTY_CYCLE_MAX;
                PWMEn(1); 
       //         printf("HB\n");
								power_ctrl_pram.ctrl_type=PID_TYPE_FRE;
              }	
#if APPLEQC							
					if((Apple)&&(QC==2)&&(bridge==2))
					{
					//		fre = 126900;
							PWMEn(1); 
		//				printf("hb\n");
							duty=400;
							power_ctrl_pram.ctrl_type=PID_TYPE_DUTY;
					}
#endif
  }		   
  if(fre>OPREATE_RANGE_FRE_MAX)
  {
		//				duty-=(fre-OPREATE_RANGE_FRE_MAX)/100;
          fre=OPREATE_RANGE_FRE_MAX;

              {
                  power_ctrl_pram.ctrl_type = PID_TYPE_DUTY;    
     //             printf("duty!\n");
              }
  }
  if(fre<Fremin)
  {
          fre=Fremin;	 
					if(power_ctrl_pram.duty_cycle<OPREATE_RANGE_DUTY_CYCLE_MAX)
						power_ctrl_pram.ctrl_type = PID_TYPE_DUTY; 
							if((bridge==1)&&(QC==0))
              {
                fre=110000;
								duty=300;
								power_ctrl_pram.ctrl_type = PID_TYPE_DUTY; 
                PWMEn(2); 
        //        printf("FB\r\n");
              }
  }
  power_ctrl_pram.fre = fre;
  power_ctrl_pram.duty_cycle = duty;
  
  SetPWM(fre,duty);
  if(tx_phase==TX_PHASE_PT)
  {
	  printf("fre=%lu \n",fre);
	  printf("duty=%hu \n",duty);
  }
}

void Disable_coil_power(void)
{
  PWMEn(0);
  Delay10us(2);
  DRIVEREN=0;
  Delay10us(1); 
}

void Enable_coil_power(void)
{
  power_ctrl_pram.fre = INITIALFRE;
//	if((tx_phase==TX_PHASE_PING)&&(QC==2))
//		power_ctrl_pram.fre+=5000;
  power_ctrl_pram.duty_cycle = INITIALDUTY;
  power_ctrl_pram.ctrl_type = PID_TYPE_FRE;
  PWMSet(); 
  Delay10us(1); 
  DRIVEREN=1;
  Delay10us(4);	   
  if(QC==2)
  PWMEn(1);
  else
  PWMEn(2);
}


/****************************ADC*******************************/
u16 GetTemp(void)
{
	u16 adc=0;
	u32 temp=0;
	u8 i;
	P07_Quasi_Mode;
	Enable_ADC_AIN2;

	for(i=0;i<10;i++)
	{	
		clr_ADCF;
		set_ADCS;
		while(ADCF == 0);
		adc=ADCRL;
		adc+=(u16)ADCRH<<4;
		temp+=adc;
	}
	clr_ADCF;
//	return temp/10;
#if VCC5V
	adc=(u16)(0.122*temp);
#else
	adc=(u16)(0.08*temp);
#endif
	return adc;
}

u16 GetVpeak(void)
{
	u16 adc=0;
	u32 temp=0;
	u8 i;
	P11_Quasi_Mode;
	Enable_ADC_AIN7;
	for(i=0;i<10;i++)
	{	
		clr_ADCF;
		set_ADCS;
		while(ADCF == 0);
		adc=ADCRL;
		adc+=(u16)ADCRH<<4;
		temp+=adc;
	}
	clr_ADCF;
//	return temp/10;
#if VCC5V
	adc=(u16)(0.122*temp);
#else
	adc=(u16)(0.08*temp);
#endif
	return adc;
}

u16 GetVpa(void)
{
	u16 adc=0;
	u32 temp=0;
	u8 i;
	P03_Quasi_Mode;
	Enable_ADC_AIN6;
	for(i=0;i<10;i++)
	{	
		clr_ADCF;
		set_ADCS;
		while(ADCF == 0);
	}
	for(i=0;i<10;i++)
	{	
		clr_ADCF;
		set_ADCS;
		while(ADCF == 0);
		adc=ADCRL;
		adc+=(u16)ADCRH<<4;		
		temp+=adc;
	}
	clr_ADCF;
//	return temp/10;
#if VCC5V
	adc=(u16)(0.382*temp);
#else	
	adc=(u16)(0.3175*temp);
#endif	
	return adc;
}

 u16 GetIpa(void)
{
	u16 adc=0;
	u32 temp=0;
	u8 i;
	P04_Quasi_Mode;
	Enable_ADC_AIN5;
//		for(i=0;i<10;i++)
//	{	
//		clr_ADCF;
//		set_ADCS;
//		while(ADCF == 0);
//	}
	for(i=0;i<10;i++)
	{	
		clr_ADCF;
		set_ADCS;
		while(ADCF == 0);
		adc=ADCRL;
		adc+=(u16)ADCRH<<4;
		temp+=adc;
	}
	clr_ADCF;
//	return temp/10;
#if VCC5V
	adc=(u16)(0.081*temp);
#else
	adc=(u16)(0.081*temp);
#endif
	return adc;
}

/***************************1ms int***********************************************/

void InitTimer3(void)
{	 
	T3CON=0x04;    //16M/128=125K
	RH3=0xff;
	RL3=0x05;
	set_ET3;                                    //enable Timer3 interrupt
    set_EA;                                     //enable interrupts
    set_TR3;  
}

void TIMER_1ms_ENABLE(unsigned char on)
{
	if(on)
	set_TR3;
	else
	clr_TR3;	
}

void Timer3_ISR (void) interrupt 16 
{
#if S50B
	static u16 i=0;
  static u16 j=0;  
  static u16 k=0;
  static u8 m=0;
#endif	
	static u8 common=0;
	common++;
	if(common==4)
	{
		t_rp_cnt_1ms++;
		t_ce_cnt_1ms++; 
		t_pr_cnt_1ms++;
#if FORT20
		if(hftofb)
			hftofb++;
#endif			
		common=0;
	}
#if S50B	
#define CYCLE 11
#define STEP  450
	if(ledon)
	{
    k++;
    i++;
    if(i>CYCLE)
      i=0;
    if(i<j)
      LED1=0;
    else 
      LED1=1;
    
    if(k%STEP==1)
    {
      if((j<CYCLE-2)&&(m==0))
        j++;
      else
        m=1;
      
      if((j>0)&&(m==1))
        j--;
      else
        m=0;
    }
		ledon++;
		if(tx_phase==TX_PHASE_PT)
		{
			if(ledon>41000)
			{
				ledon=0;
				i=0;
				j=0;  
				k=0;
				m=0;				
				LED1=1;
			}
		}
		else if(ledon>16000)
		{
			ledon=0;
			i=0;
			j=0;  
			k=0;
			m=0;
			LED1=1;
		}			
	}		
#endif	
	clr_TF3;
}

/******************************capture**********************************/
void InitCapture(void)
{
	P01_Input_Mode;
	P15_Input_Mode;	
	TIMER2_DIV_16;
	TIMER2_Auto_Reload_Capture_Mode;

	IC4_P01_CAP0_RisingEdge_Capture;
	IC7_P15_CAP1_RisingEdge_Capture;

    set_ECAP;                                   //Enable Capture interrupt
    set_TR2;
}

u16 TIM2_GetCounter(u8 i)
{	
	u16 cap=0;
	if(i)
	{
		cap=C1L;
		cap+=C1H<<8;	
	}	
	else
	{
		cap=C0L;
		cap+=C0H<<8;
	}	
	return cap;
}

void Capture_ISR (void) interrupt 12
{	  		

		if(0!=(CAPCON0&0x02))			   //CAP1
		{
			rcv_packet_int_handler(1);
			CAPCON1^=0x04;						 //触发翻转
			clr_CAPF1;
		}
		if(0!=(CAPCON0&0x01))	   //CAP0
		{

			rcv_packet_int_handler(0);
			CAPCON1^=0x01;						  //触发翻转
			clr_CAPF0;
		}
}

/******************************GPIO**********************************/
void InitGPIO(void)
{
	P05_PushPull_Mode;//DRIVER EN
	P14_OpenDrain_Mode;//D+
	P17_PushPull_Mode;//D-	
	P02_Quasi_Mode;//FAN
	P16_Quasi_Mode;//LED2
	P12_Quasi_Mode;//LED1
		
	DRIVEREN=0;
	Dp=0;
	Dn=0;
	FAN=0;
	LED2=1;
	LED1=1;
}

/***************************QC2.0**********************************/
void QC20SHAKE(void)//2线
{
//		uint8_t i=0;
    Dp=0;//0.6V
    Dn=1;//0.6V
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=1;
#endif		
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=0;
#endif			
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=1;
#endif				
		
		
		
    Dn=0;//0V
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=0;
#endif	
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=1;
#endif	
		Delay_ms(500); 
#if NTCOPENSHORT
		if(ntc)
		LED1=0;
#endif			
}

void QC20OUT9V(void)//2线
{
    Dp=1;//3.3V
    Dn=1;//0.6
}
void QC20OUT5V(void)//2线
{     
    Dp=0;//0.6
    Dn=0;//0
}
void QC20OUT12V(void)//2线
{     
    Dp=0;//0.6
    Dn=1;//0.6
}

/**********************************FSK*****************************/
static void Delay_full_fsk(u8 full)
{
	u16 cnt = 0;
	u16 cnt_target = 256;
	
	set_LOAD;
	if (full)
            cnt_target=512;
	while(1)
	{
		 PWMF=0;
		 while(PWMF!=1);
		 cnt += 1;
		 if(cnt_target<=cnt)
		 {
			 break;
		 } 
	};/*wait to full cycle send OK*/
}

#define DEEP_SAMSUNG    1
void Fsksend_sa(void)
{
  u8 full = 1;
  uint8_t i=0;
  clr_EA;
  Delay_ms(5);
  while(i<30)
  {
	full = 1;
	if (((i>=2)&&(i<=3))
		|| ((i>=10)&&(i<=13))
		|| ((i>=15)&&(i<=16))
		|| ((i>=24)&&(i<=29)))
		full = 0;
	if (i%2)
		PWMPL+=DEEP_SAMSUNG;
	else
		PWMPL-=DEEP_SAMSUNG;
	Delay_full_fsk(full);
	i++;
  }
  set_EA;
  return;
}
#if ECH
//#define DEEP_ECH    (2)
void Fsksend_ech_power(int8_t deep_ech,int8_t power)
{
  unsigned char sig=0;
  // 5'b + 3'b(power)
  
  clr_EA;
  if(power & 0x01){
    PWMPL-=deep_ech;
    Delay_full_fsk(0);
    PWMPL+=deep_ech;
    Delay_full_fsk(0);//1 b0
  }else{
    PWMPL-=deep_ech;
    Delay_full_fsk(1);//0 b0
    sig =1;
  }
  
   if(power & 0x02){
     if(sig){
      PWMPL+=deep_ech;
      Delay_full_fsk(0);
      PWMPL-=deep_ech;
      Delay_full_fsk(0);//1 b1
     }else{
      PWMPL-=deep_ech;
      Delay_full_fsk(0);
      PWMPL+=deep_ech;
      Delay_full_fsk(0);//1 b1
     }
  }else{
    if(sig){
      PWMPL+=deep_ech;
      Delay_full_fsk(1);//0 b1
      sig =0;
    }else{
      PWMPL-=deep_ech;
      Delay_full_fsk(1);//0 b1
      sig =1;
    }
  }
  
   if(power & 0x04){
     if(sig){
      PWMPL+=deep_ech;
      Delay_full_fsk(0);
      PWMPL-=deep_ech;
      Delay_full_fsk(0);//1 b2
     }else{
      PWMPL-=deep_ech;
      Delay_full_fsk(0);
      PWMPL+=deep_ech;
      Delay_full_fsk(0);//1 b2
     }
  }else{
    if(sig){
      PWMPL+=deep_ech;
      Delay_full_fsk(1);//0 b2
      sig =0;
    }else{
      PWMPL-=deep_ech;
      Delay_full_fsk(1);//0 b2
      sig =1;
    }
  }
  if(sig){
    PWMPL+=deep_ech;
    Delay_full_fsk(0);
    PWMPL-=deep_ech;
    Delay_full_fsk(0);//1 b3
    
    PWMPL+=deep_ech;
    Delay_full_fsk(1);//0 b4
    
    PWMPL-=deep_ech;
    Delay_full_fsk(0);
    PWMPL+=deep_ech;
    Delay_full_fsk(0);//1 b5
    
    PWMPL-=deep_ech;
    Delay_full_fsk(0);
    PWMPL+=deep_ech;
    Delay_full_fsk(0);//1 b6
    
    PWMPL-=deep_ech;
    Delay_full_fsk(0);
    PWMPL+=deep_ech;
    Delay_full_fsk(0);//1 b7
  
  }else{
    PWMPL-=deep_ech;
    Delay_full_fsk(0);
    PWMPL+=deep_ech;
    Delay_full_fsk(0);//1 b3
    
    PWMPL-=deep_ech;
    Delay_full_fsk(1);//0 b4
    
    PWMPL+=deep_ech;
    Delay_full_fsk(0);
    PWMPL-=deep_ech;
    Delay_full_fsk(0);//1 b5
    
    PWMPL+=deep_ech;
    Delay_full_fsk(0);
    PWMPL-=deep_ech;
    Delay_full_fsk(0);//1 b6
    
    PWMPL+=deep_ech;
    Delay_full_fsk(0);
    PWMPL-=deep_ech;
    Delay_full_fsk(0);//1 b7
  }
  
  set_EA;
  return;
}
#endif
