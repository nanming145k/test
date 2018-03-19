
#include <stdio.h>
#include "N76E003.h"
#include "Define.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "hal.h"
#include "rcvpkt.h"
#include "ping.h"
#include "selection.h"
#include "idcfg.h"
#include "pt.h"
#include "version.h"

TX_POWER_CONTROL_PARAMETER_S	power_ctrl_pram;
TX_S  		     tx;
TX_PHASE_E	tx_phase = TX_PHASE_INVALID;
TX_DETECT_INFO_S        tx_info;
uint_16 	t_rp_cnt_1ms = 0;
uint_16 	t_ce_cnt_1ms = 0;
uint_16		t_pr_cnt_1ms = 0;
u8 Samsung=0;
u8 QC=0; 
u16 IPA0=0;
u32 Fremin=OPREATE_RANGE_FRE_MIN;
u8 echrx=0;/*0: not ech rx; other: ech rx.(1: samsung or epp or bpp ech rx,2 new personal ech protocol for "jingli" phone)*/
u8 Xamsung=0;
u8 ASKch=0;
//u8 id1=0;
//u8 id2=0;
u8 Apple=0;
u8 BPP=0;
u16 sipa=0;
u16 sfod=0;
#if S50B
u32 ledon=0;
#endif
#if NTCOPENSHORT
u8 ntc=0;
#endif
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
  EXIT_PT_INFO_S    exit_pt_info;
#endif
#if NEW_TEMPDET_SLOW_CHARGING
u8	temp_slow_state = 0;/*0:fast charging, 1: samsung enter low power during fast_charging, 2: samsung enter or at slow charging after 1*/
#endif
#if FORT20
u8 hftofb=0;
#endif
u8 vqi=0;


void main (void) 
{
	u16 i;	
	u16 temp;

	clr_EA;
	TA=0xAA;
	TA=0x55;
	CKEN|=0xc0;
	while(1)
	{
		if(CKSWT&0x08)
			break;
	}
	TA=0xAA;
	TA=0x55;
	CKSWT=0x02;
	TA=0xAA;
	TA=0x55;	
	CKEN&=0xc0;	
  
	InitPWM();
	SetPWM(146000,500);
	PWMEn(0);	
	InitialUART0_Timer1(115200);
	InitTimer3();
	InitCapture();
	InitGPIO();
	set_EA;
#if S50B
	TIMER_1ms_ENABLE(1);
  ledon=1;
#else	
	LED1=0;
#endif
	Reset_Receive_Packet();
	if(GetVpa()>MAX_PA_VOLTAGE)  //输入过压检测
	{
	  tx_phase=OVP;             
	}
	else
	{
	  tx_phase = TX_PHASE_SELECT;
	}
#if NTCOPENSHORT
	temp=GetTemp();
	if((temp >3200) || (temp < 600))
	{
		printf("Vtemp=%hu \n",temp);
		ntc=1;
	}
#endif	
	QC20SHAKE();
	QC20OUT12V();
	Delay_ms(1000);
#if NTCOPENSHORT
		ntc=0;
#endif
  printf("Vpa=%hu \n",GetVpa());
	if(GetVpa()>8000)
		QC=2;
	else
		QC=0;
	LED1=1;   //QC握手
	
	Delay_ms(100);
	IPA0=GetIpa();//ipa offset
	

  printf("%s\n", VERSION);
	printf("QC=%bd \n",QC);
	printf("IPAoffset=%hu \n",IPA0);
//	printf("CKEN=%bx\n",CKEN);
//	printf("CKSWT=%bx\n",CKSWT);

	
  while(1)
	{
		switch(tx_phase)
		{
			case TX_PHASE_SELECT:
			{
              #if S50B
								ledon=0;
              #endif
	            if(GetVpa()>MAX_PA_VOLTAGE)
	            { 
	              tx_phase=OVP;
	              Disable_coil_power();
	              break;                     
	            }          
	            if(GetVpa()>7000)
								QC=2;
	            else
								QC=0;   
				      temp=GetTemp();
							printf("Vtemp=%hu \n",temp);
	            Disable_coil_power();
	            LED1=1;                  
	            if(!Samsung) 
								Delay_ms(200);     
							if((exit_pt_info.reason_last == EXIT_PT_REASON_WRONG_PACKET)&&(Samsung))
								Delay_ms(300);					
							
	            Samsung=0;     
	            Delay_ms(300);	

							TX_Phase_ProcessSelect();			
			}
			break;

			case TX_PHASE_PING:
			{
				TX_Phase_ProcessPing();
			}
			break;

			case TX_PHASE_IDCFG:
			{
				TX_Phase_ProcessCfg(&tx_info); 
			}
			break;
			case TX_PHASE_PT:
			{
				TX_Phase_ProcessPT(&tx_info);
			}
			break;
			
	    case OVERTEMP:
	    {
				LED1=1;
				for(i=0;i<600;i++)
				{
					LED1=0;
					Delay_ms(500);
	        LED1=1;
	        Delay_ms(500);
	            
	        if(GetTemp()> TEMPTH4)//40度
	        {
						Delay_ms(300);
	          if(GetTemp() > TEMPTH4)
	          break;
	        }
	       }
	       tx_phase=TX_PHASE_SELECT;
	      }
	      break;        
 
	        case COMPLETE:
	        {
	           LED1=1;
	           while(1)
	          {                    
	              Enable_coil_power();
	              Delay_ms(5);
	              i=GetIpa()-IPA0;
	              if((i>30)&&(i<65)&&(QC==0))
	                break;
	              else if((i>15)&&(i<45)&&(QC==2))
	                break;
	              Disable_coil_power();
	              Delay_ms(300);
	           }
	
	          LED1=1;
	          LED2=1;
	          tx_phase=TX_PHASE_SELECT;
	        }
	        break;
	        case OVP:
	        {
	            QC20OUT12V();
	            while(GetVpa()>MAX_PA_VOLTAGE)
	            {
	              Delay_ms(1000);
	              LED1=0;
	              Delay_ms(1000);
	              LED1=1;
	            }   
	            tx_phase=TX_PHASE_SELECT;
	        }              
			default:
        	break; 	
		}
	}
}




