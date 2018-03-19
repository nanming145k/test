#include "pt.h"

#if DPLL
u16 firstvpa=0;
#endif
u8 Save=0;
u8 Savep=0;
#if ECH
extern ECH_STATE_E	ech_state;
#endif
u8 fod_effi[11] = {20,30,40,45,50,55,60,65,65,65,70};
u8 pid_Kp[2][9] = {
				{2, 2, 2, 6, 8, 10, 10, 10, 10},/*noraml*/
				{2, 2, 3, 3, 6, 6,  10,  10, 10},/*samsung*/
			};/*from 100K~190K*/

#if FORT20
void HBtoFB(void)
{
	if(bridge==1)
	{
//		Delay_ms(200);
		power_ctrl_pram.fre +=15000;
//		if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
//		{
//			power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
//			power_ctrl_pram.duty_cycle=400;
//			power_ctrl_pram.ctrl_type = PID_TYPE_DUTY;
//		}
		PWMSet();
		PWMEn(2); 
		printf("FB\n");
		power_ctrl_pram.cecnt_after_toFB = 1;
	}
}	
#endif

#if NEW_TEMPDET_SLOW_CHARGING 
static uint8_t	pt_temp_control_slow(
					uint8_t rpcount,
					uint16_t power_received,
					uint16_t papower,
					uint8_t s8_bl
	)
{	
	static uint8_t	rp_counter;
	
	static uint16_t rp_stable_cnt;
	static uint8_t	rp_notstable_cnt;
	static uint16_t stable_rp_value;
	
	static uint16_t pa_stable_cnt; 
	static uint8_t	pa_notstable_cnt;
	static uint16_t stable_pa_value;
	
	uint8_t 	   overtempreason = 0;
	uint16_t		rp_check_thr = 400;
	uint16_t        stable_rp_thr = 1500;
	uint16_t        stable_pa_thr = 2000;
	
	if (rpcount == 0)
	{
		rp_stable_cnt = 0;
		rp_notstable_cnt = 0;
		stable_rp_value = 0;
		
		pa_stable_cnt = 0;
		pa_notstable_cnt = 0;
		stable_pa_value = 0;

		rp_counter = 0;
	}

	 if(rp_counter < 0xff)
		rp_counter++;
	 if ((rp_counter > 30)&&(ech_state == ECH_STATE_INIT)&&(s8_bl<100))
	 {
			if (Samsung)
			{     
					rp_check_thr = 250;
					stable_rp_thr = 2500;
					stable_pa_thr = 4000;
			}
			
		 if (Xamsung)
		 {			 
			 if ((stable_rp_value < power_received + 250)
				 &&(stable_rp_value > power_received - 250))
			 {
				 rp_stable_cnt ++;
				 rp_notstable_cnt = 0;
				 printf("rp_stable_cnt =%hu\n", rp_stable_cnt);
			 }
			 else
			 {
				 rp_notstable_cnt ++;
				 if (rp_notstable_cnt > 5)
				 {
					 stable_rp_value = power_received;
					 rp_stable_cnt = 0; 				 
				 }
			 }
			 if (rp_stable_cnt > rp_check_thr)
			 {
				 printf("check stable_rp_value=%hu\n", stable_rp_value);
				if (stable_rp_value < stable_rp_thr)
					overtempreason = 1; 
				else
				{
					rp_stable_cnt = 0;
					stable_rp_value = 0;
					rp_notstable_cnt = 0;
				 }	
			 }			 

			 if ((stable_pa_value < papower + 250)
				 &&(stable_pa_value > papower - 250))
			 {
				 pa_stable_cnt ++;
				 pa_notstable_cnt = 0;
				 printf("pa_stable_cnt =%hu\n", pa_stable_cnt);
			 }
			 else
			 {
				 pa_notstable_cnt++;
				 if (pa_notstable_cnt > 5)
				 {
					 stable_pa_value = papower;
					 pa_stable_cnt = 0;
				 }
			 }
			 if (pa_stable_cnt > rp_check_thr)
			 {
				 printf("stable_pa_value=%hu!!!!\n", stable_pa_value);
				 if (stable_pa_value < stable_pa_thr)
					overtempreason = 2;
				 else
				 {
					pa_stable_cnt = 0;
					pa_notstable_cnt = 0;
					stable_pa_value = 0;
				 }
			 }			 
		 }
	 }

	 if (overtempreason) {
		printf("lp%bu\n", overtempreason);
	 }
	 return (overtempreason);
}

#endif
#if APPLEQC
void tan(void)
{
						if(power_ctrl_pram.fre == 127000)
							power_ctrl_pram.fre = 126900;
						else if(power_ctrl_pram.fre == 126900)
							power_ctrl_pram.fre = 127000;
						
						PWMSet();
}
#endif
void Ctrl_PIDAlgorithm(int_8 ctrl_error_val)
{
    int_32 deltafre;

    //for( i=0; i<PID_LOOP_INTERATION_INDEX_MAX; i++)
    {
        Delay_ms(TIMING_EXECUTE_A_SINGLE_ITERATION);	// tinner, ms
        
				if(Samsung)
				{
					 Fremin=OPERATE_SANSUNG_FRE_MIN;
				}
				else
				{
					Fremin=OPREATE_RANGE_FRE_MIN;
				}      
						
				if ((power_ctrl_pram.cecnt_after_toFB)&&(power_ctrl_pram.cecnt_after_toFB < 10))
					Fremin += 20000;		
				
				if ((ctrl_error_val > 55)&&(Samsung))
						ctrl_error_val = 55;
if( PID_TYPE_FRE == power_ctrl_pram.ctrl_type)
{
						if(bridge==2)
							deltafre=((int_32)ctrl_error_val*100); 
						else
							deltafre=((int_32)ctrl_error_val*300); 	
						if((ctrl_error_val==60)||(ctrl_error_val==-60))
							deltafre*=1.5;

            if(((ctrl_error_val==-1)||(ctrl_error_val==1)))
              deltafre=0;
		
							if(((ctrl_error_val==-2)||(ctrl_error_val==-3)||(ctrl_error_val==-4))&&(BPP))
							{  
								power_ctrl_pram.duty_cycle+=(int_16)(ctrl_error_val*2);  
							}	
							else
          			power_ctrl_pram.fre-=deltafre;
}
else   //duty
{
	if(((ctrl_error_val==-1)||(ctrl_error_val==1)))
					ctrl_error_val=0;
	 power_ctrl_pram.duty_cycle+=(int_16)(ctrl_error_val*1);     
}
          PWMSet();
          return;          
    }
}




void TX_Phase_ProcessPT( TX_DETECT_INFO_S   			*p_tx_info)
{
	  u8 bpprpnum=0;
	 u16 lastrppower=0;
	
    int_8    ctrl_error_val;
    uint_16  power_received;
    u16 ipa;	
		u16 ipac;	
		u8 ipa0;		
	  u8 ipa1;	
    u16 vpa;
    u16 papower=0;
    u8 firstce = 1;
    int_16 ploss = 0;
		u8 rpcount=0;
#if !FORT20
    u8 cecount=0;
#endif
    u8 readyfsk=1;
    u8 NotCharging_Cnt = 0;
    u8 Charging_Cnt = 0;
    u8 complete=0;
		u16 tempthr=0;
    u8 i = 0;
    u8 header,byte1,byte2,byte3;
		u32 cetimeout=TIMING_CONTROL_ERROR_PACKET_TIMEOUT;
		u16 vtemp=0;
#if ECH
    unsigned char ECH_Rx_Required_Power = 5;
    u8 first_ce_zero = 1;
#endif
#if FULLNESS
			 #define RP_FULLNESS_MORE_THAN_NUMBER  250 
			 int  rp_full_number_of_conditions = 0;
			 int  rp_unfull_number_of_conditions = 0;
			 #define RP_FULLNESS_NUM 1000
#endif
#if FODDET
    u8 fodcount = 0;
#endif
    int_8 last_ctrl_error_val=0;

    u8 cebutiao=0;
#if DPLL
    u32 cntdown=0;
		u16 deltavpa=0;
#endif
#if NEW_TEMPDET_SLOW_CHARGING
		u8	s8_bl = 0;
#endif  
#if CALIB
		u8 caliben=0;	
#endif
#if APPLEQC
u8 tantanle=1;
u8 lock=0;
#endif
    RX_PACKET_STRUCTURE_S   		*p_tx_receive_packet = NULL;
    t_ce_cnt_1ms=0;
    t_rp_cnt_1ms=0;			   
	  t_pr_cnt_1ms=0; 
#if FORT20				 
		hftofb=0;
#endif			

#if DPLL
    firstvpa=GetVpa();
//	if(firstvpa<4900)
//		deltavpa=300;
//	else
		deltavpa=500;
		printf("FirstVpa=%hu \n",firstvpa);
#endif
#if FPDDET_EXITPT3TIMES_SUPPORT
	exit_pt_info.reason_cur = EXIT_PT_REASON_UNKNOW;
#endif  
    Reset_Receive_Packet();
		

		
    while(1)
    {
      
#if VPEAKDET  
      if(GetVpeak()>1870)
      {
        Disable_coil_power();
        tx_phase = TX_PHASE_SELECT;
				printf("VPEAK!!!\n");
 #if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
				exit_pt_info.reason_cur = EXIT_PT_REASON_OVER_VPEAK;
 #endif				
        break;
      }
#endif

			if((t_rp_cnt_1ms > TIMING_RECEIVED_POWER_PACKET_TIMEOUT)
                &&(rpcount == 0))/*iphone rx timeout and Qi certi*/
			{
				tx_phase = TX_PHASE_SELECT;
				printf("RP timeout!!\n");
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
		  	exit_pt_info.reason_cur = EXIT_PT_REASON_RP_TO;
#endif				
				break;
			}
			if(( t_pr_cnt_1ms >= 2000)||(( t_ce_cnt_1ms >= cetimeout)))
			{
					if( t_pr_cnt_1ms >= 2000)
					{
						Savep++;
						if(Savep>=3)
						{
							Savep=0;
							tx_phase = TX_PHASE_SELECT;
							printf("CE timeout \n");
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
							exit_pt_info.reason_cur = EXIT_PT_REASON_CE_TO;
#endif							
							break;
						}	
						else
						{
							t_pr_cnt_1ms=0;
							if(power_ctrl_pram.ctrl_type!=PID_TYPE_DUTY)
								power_ctrl_pram.fre+=5000;
							else
								power_ctrl_pram.duty_cycle-=50;	
							if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
							{
								power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
								power_ctrl_pram.duty_cycle-=50;
								power_ctrl_pram.ctrl_type = PID_TYPE_DUTY; 
							}								
							printf("Savep \n");
						}	
					}
				else if( t_ce_cnt_1ms >= cetimeout)
				{
					  if(BPP)
							Save=2;
						else
							Save++;
						if(Save>=2)
						{
								Save=0;
								tx_phase = TX_PHASE_SELECT;
								printf("CE timeout  \n");
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
								exit_pt_info.reason_cur = EXIT_PT_REASON_CE_TO;
#endif								
								break;
						}
						else
						{
							t_ce_cnt_1ms = 0;
							t_rp_cnt_1ms = 0;
							if(power_ctrl_pram.ctrl_type!=PID_TYPE_DUTY)
								power_ctrl_pram.fre+=5000;
							else
								power_ctrl_pram.duty_cycle-=50;	
							if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
							{
								power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
								power_ctrl_pram.duty_cycle-=50;	
								power_ctrl_pram.ctrl_type = PID_TYPE_DUTY; 
							}
									
									printf("Save \n");
						 }                     
				}   
				  PWMSet(); 
			  }
             
#if DPLL
         if(QC==0)
         {
           cntdown=0;
           do
           {
             cntdown++;
             vpa=GetVpa()+deltavpa;
             Delay10us(1);
             if(cntdown>10)
             {
							 printf("DPL+\n");
               power_ctrl_pram.fre+=5000;
							 if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
							{
								power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
						   	power_ctrl_pram.duty_cycle-=50;	
								power_ctrl_pram.ctrl_type = PID_TYPE_DUTY; 
							}
               PWMSet();
               cebutiao=4;
							 if(BPP)
							 {
								 LED1=1;
								 Delay_ms(100);
								 LED1=0;
							 }
               break;
             } 
           } while(vpa<firstvpa);  
         }
#endif          
#if FORT20				 
				if(hftofb>200)
				{
					HBtoFB();
					cebutiao=3;
					hftofb=0;
				}	
#endif					 
        if(Receive_Packet(&p_tx_receive_packet))
					continue; 
        header = p_tx_receive_packet->header;
				byte1 = p_tx_receive_packet->msg[0];
				byte2 = p_tx_receive_packet->msg[1];
				byte3 = p_tx_receive_packet->msg[2];
				Save=0;
				Savep=0;
        if( header == RX_PACKET_TYPE_END_POWER_TRANSFER)		//收到停止传输能量信号
        {
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
						exit_pt_info.reason_cur = EXIT_PT_REASON_EPT;
#endif					
						printf("EPT=%bd \n",byte1);
            switch (byte1)			//判断停止电能传输的原因
            {
                case EPT_VALUE_UNKNOWN:             // unknown
										if (ech_state > ECH_STATE_INIT)
										{
											tx_phase = TX_PHASE_SELECT;
											Disable_coil_power();
										}
										else
										{
											tx_phase = COMPLETE;
											Disable_coil_power();
											Delay_ms(200);
										}
								break;
								
                case EPT_VALUE_OVER_TEMPERATURE:    // over temperature  
#if NEW_TEMPDET_SLOW_CHARGING
								if ((Samsung)&&(ech_state == ECH_STATE_INIT))
								{
									tx_phase = TX_PHASE_SELECT;
									temp_slow_state = 1;
									Disable_coil_power();
									break;
								}
#endif						
                case EPT_VALUE_RECONFIGURE:         // reconfigure

                case EPT_VALUE_CHARGE_COMPLETE:     // charge complete
                  

                case EPT_VALUE_INTERNAL_FAULT:      // internal fault
                //no break;
                case EPT_VALUE_OVER_VOLTAGE:        // over voltage
                //no break;
                case EPT_VALUE_OVER_CURRENT:        // over current
                //no break;
                case EPT_VALUE_BATTERY_FAILURE:     // battery failure
                //no break;
                
                default:
                  tx_phase = COMPLETE;
									Disable_coil_power();
									Delay_ms(200);
                break;
            }
            break;
        }
        else if( header == RX_PACKET_TYPE_8BIT_RECEIVED_POWER)		//
        {        

						 printf("RP=%bu \n",byte1);
                  
			       
             #if ECH
               if(ech_state == ECH_STATE_HANDSHAKE_OK)
               {
									power_received = (uint_16)(((uint_32)ECH_Rx_Required_Power + 5) * byte1*1000/255);
               }else
             #endif
                power_received = (uint_32)byte1*1000*p_tx_info->max_power_value>>8;
//#if APPLEQC						
//					if((Apple)&&(QC==2))
//					{
//						if(power_received>7000)
//						{
//							lock++;
//							if(lock>2)
//							tantanle=0;
//						}
//						else
//							lock=0;
//						if((tantanle)&&(power_received<5700))
//					{
//						tan();
//					}
//					if((power_received>5700)&&(power_received<6700)&&(tantanle))
//					{
//						tantanle++;
//						if(tantanle==12)
//						{
//							tantanle=1;
//							tan();
//						}
//					}
//					else if(tantanle)
//						tantanle=1;
//					}
//#endif									 
            #if FULLNESS
             if(Apple)
             {
              if(400 <= power_received && RP_FULLNESS_NUM + 250  >= power_received)
              {
                rp_full_number_of_conditions++;
								printf("n=%d\r\n",rp_full_number_of_conditions);
                if(rp_full_number_of_conditions >= RP_FULLNESS_MORE_THAN_NUMBER)
                {
                  rp_unfull_number_of_conditions = 0;
                  rp_full_number_of_conditions = RP_FULLNESS_MORE_THAN_NUMBER + 1;
                  printf("F!!\r\n");
                  complete=1;
									LED1 = 1;
                }
								if(rp_full_number_of_conditions > 30)
								{
									rp_unfull_number_of_conditions = 0;
								}
              }
              else
              {
                rp_unfull_number_of_conditions++;
                if(rp_unfull_number_of_conditions > 5)
                {
                  rp_unfull_number_of_conditions = 5;
                  rp_full_number_of_conditions = 0;
									LED1 =0;
                  complete=0;
                }
              }
             }
            #endif
						 
             ipa=GetIpa();
#if CALIB				
						if((Data_Flash[0]==0xff)&&(Data_Flash[1]==0xff))
							caliben=0;
						else
						{
							caliben=1;
	//						printf("cali %hd\n",(u16)Data_Flash[0]*100+Data_Flash[1]);
	//						printf("cali %hd\n",(u16)Data_Flash[2]*100+Data_Flash[3]);
						}
						 if(caliben)
						 {
							 ipac=(u16)((u32)ipa*((u16)Data_Flash[0]*100+Data_Flash[1])/((u16)Data_Flash[2]*100+Data_Flash[3]));
							 printf("Ipac=%hu \n",ipac);
						 }
#endif						 
             vpa=GetVpa();
						if(vpa>MAX_PA_VOLTAGE)
						{
							tx_phase=OVP;
							printf("OVP!\n");
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
							exit_pt_info.reason_cur = EXIT_PT_REASON_POWER_LIMIT;
#endif 							
							break;                     
						}
						if(ipa>2000)
						{
							tx_phase=TX_PHASE_SELECT;
							printf("OCP!\n");
							sfod=2;
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
							exit_pt_info.reason_cur = EXIT_PT_REASON_POWER_LIMIT;
#endif 
							break; 
						}
             papower=(ipa/10)*(vpa/100);
						    
             if(Samsung)
             {
							 FAN=1;
               power_received=power_received*2; 
               if(power_received>(papower*3/2))
               power_received=power_received/2;
             }
             printf("Power_received=%hu ", power_received); 
						 printf("Power_pa=%hu \n", papower);   
						 printf("Vpa=%hu \n",vpa);
						 printf("Ipa=%hu \n",ipa);
						 
						 
#if FODDET
if(!BPP)
{	

						if((Xamsung)||(Apple))/*samsung. iphone*/
					{
					    ploss = (int16_t)papower -(int16_t)power_received;
						#if S50B || ZENS
							if(((ploss >= 5200)&& (ech_state >= ECH_STATE_ID_OK)) //T20
								||((ploss >= 4700)&&(rpcount > 20) && (ech_state == ECH_STATE_INIT)) //samsung apple
								||((ploss >= 3900)&&(rpcount <= 20)&&(Xamsung && ech_state == ECH_STATE_INIT))/*samsung fast/slow charging*/
								||((ploss >= 3200)&&(rpcount <= 20)&&(Apple))/*iphone slow charging*/
#if APPLEQC
								||((papower>9200)&&(((Apple)&&(QC==0))||((!Samsung)&&(Xamsung))))  /*samsung/iphone slow charging*/    
#else
								||((papower>9200)&&((Apple)||(!Samsung)))  /*samsung/iphone slow charging*/ 
#endif                         
								||(papower>15200))
						#else
							if(((ploss >= 5000)&&(ech_state >= ECH_STATE_ID_OK)) //T20
								||((ploss >= 4500)&&(rpcount > 20) && (ech_state == ECH_STATE_INIT)) //samsung apple
								||((ploss >= 3700)&&(rpcount <= 20)&&(Xamsung && ech_state == ECH_STATE_INIT))/*samsung fast/slow charging*/
#if APPLEQC							
								||((ploss >= 3000)&&(rpcount <= 20)&&(Apple)&&(QC==0))/*iphone slow charging*/
								||((ploss >= 3700)&&(rpcount <= 20)&&(Apple)&&(QC==2))/*iphone fast charging*/
#else
								||((ploss >= 3000)&&(rpcount <= 20)&&(Apple))
#endif							
#if APPLEQC
								||((papower>9000)&&(((Apple)&&(QC==0))||((!Samsung)&&(Xamsung))))  /*samsung/iphone slow charging*/    
#else
								||((papower>9000)&&((Apple)||(!Samsung)))  /*samsung/iphone slow charging*/ 
#endif
								||(papower>15000))
						#endif 
							{	
								if(rpcount>20)
							fodcount++;
								else
								fodcount+=2;	
						  }
						else
						{
							if(!sfod)
							fodcount=0;
						}
					}
					else
					{	
           i = power_received / 1000;
           if(i > 10)
             i = 10;
           if((power_received < ((u32)papower * fod_effi[i]/100)) && (power_received>300))
             fodcount+=2;
					 else if((power_received==0)&&(papower>1700))
						 fodcount+=2;
           else 
					 {
						 if(!sfod)
             fodcount=0;
					 }
				 }
if(sfod)
fodcount+=4;
if(vqi<0x11)
	fodcount=0;
					if ((fodcount > 7) || (papower>20000))
					{
						tx_phase = TX_PHASE_SELECT;
						sfod=2;
						printf("FOD \n");
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
            exit_pt_info.reason_cur = EXIT_PT_REASON_FOD;
#endif						
						break;
					}
}
else   //BPP FOD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
						if((power_received-lastrppower>35)&&(power_received-lastrppower<250)&&(bpprpnum<5)&&(power_received<((u32)papower*65/100)))
							fodcount++;	
						else if(power_received<((u32)papower*20/100))
							fodcount+=2;
						else
							fodcount=0;
						
//printf("powerdiff=%hu \n", power_received-lastrppower); 
//printf("fodcount=%bu \n", fodcount);
						lastrppower=power_received;
						if(bpprpnum<10)
						bpprpnum++;
						if(fodcount>3)
						{
								tx_phase = TX_PHASE_SELECT;
								sfod=2;
								printf("FOD \n");
								break;
						}
            
}
#endif
#if TEMPDET
				vtemp=GetTemp();
				printf("Vtemp=%hu \n",vtemp);
				if(Apple||(Xamsung && (ech_state == ECH_STATE_INIT))) //xamsung apple use 60, T20 and others 70
								tempthr=TEMPTH2;
				else
								tempthr=TEMPTH1;
			 if(vtemp<tempthr)//
			  {
					Delay_ms(50);
					if(GetTemp()<tempthr)
					{
						tx_phase = OVERTEMP;
						Disable_coil_power();
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
						exit_pt_info.reason_cur = EXIT_PT_REASON_OVER_TEMP;
#endif						
						break;
					}
			  }
#endif				
#if NEW_TEMPDET_SLOW_CHARGING 
			 if(pt_temp_control_slow(rpcount, power_received, papower, s8_bl))
			 {	
					temp_slow_state = 1;
					tx_phase = TX_PHASE_SELECT;
					if ((!Samsung)&&(Xamsung))
					{
                    uint16_t ipa_target = 0;
                    uint_8	 cnt = 60; /*60*1s = 60s*/    
									
                    Delay_ms(10);
										Disable_coil_power();

                    while(cnt--)
                    {
                      Delay_ms(990);
											Enable_coil_power();
                      Delay_ms(10);
                      ipa=GetIpa();
                      ipa-=IPA0;
                      if (ipa_target == 0 )
                        ipa_target = ipa;
											Disable_coil_power();
                      printf("lp=%hu !\n", ipa);
                      if ((ipa > ipa_target + 7)
                         || (ipa_target > ipa + 7))
                      { 
                        temp_slow_state = 0;
                        break;
                      }
                    }							
					}
					break;
		 }
#endif				
             if (rpcount <0xff)
			    rpcount++;
		}
        // Control Error packet
        else if( header == RX_PACKET_TYPE_CONTROL_ERROR)
        {
					ctrl_error_val = (int_8)byte1;
#if APPLEQC
					if((Apple)&&(QC==2))
					{
					power_ctrl_pram.fre = 129000;
					power_ctrl_pram.ctrl_type=PID_TYPE_DUTY;
					}
#endif
					if(firstce==1)
					{
#if APPLEQC						
						if((Apple)&&(QC==2))
						{
//							power_ctrl_pram.fre = 129000;
							power_ctrl_pram.duty_cycle=350;
//							power_ctrl_pram.ctrl_type=PID_TYPE_DUTY;
						}
						else
#endif
						{
							power_ctrl_pram.fre = OPREATE_RANGE_FRE_MAX;
							power_ctrl_pram.duty_cycle = OPREATE_RANGE_DUTY_CYCLE_MAX;
						}
	//					PWMSet();
#if !S50B
						if(!Xamsung)
						LED1=0;
#else
	          ledon=1;					
#endif

					}				
					else if((firstce>3)&&(ctrl_error_val!=55)&&(Xamsung))
					{
#if !S50B						
						LED1=0;
#endif
						firstce=0;
					}					
					if(firstce!=0)
					firstce++;
//#if !S50B					
//					if((papower<2000)&&(power_ctrl_pram.fre<Fremin+10000)&&(ctrl_error_val > 30))
//					{
//						 if (NotCharging_Cnt < 0xff)
//								NotCharging_Cnt++;
//							Charging_Cnt = 0;
//							if (NotCharging_Cnt == 10)
//							LED1=1;
//					}
//	        else if((((papower > 2200)&&(ctrl_error_val<10))||((papower > 3000)&&(power_received > 3000)))&&(!complete))
//					{
//						 NotCharging_Cnt = 0;
//						if (Charging_Cnt < 0xff)
//							Charging_Cnt++;
//						if (Charging_Cnt==15)
//							LED1=0;
//					}
//					else
//					{
//						NotCharging_Cnt = 0;
//						Charging_Cnt = 0;
//					}
//#endif
#if !FORT20
          if(cecount!=0)
            cecount++;
          if((cecount==3)&&(bridge==1))
          {          
            cecount=0;
            power_ctrl_pram.fre +=15000;
//						if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
//						{
//							power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
//							power_ctrl_pram.duty_cycle-=50;
//						}
            PWMSet();
            PWMEn(2); 
						cebutiao=3;
						printf("FB\n");
						power_ctrl_pram.cecnt_after_toFB = 1;
            continue;
          }
#endif	
          if((readyfsk!=0)&&(QC==2)&&(Xamsung==1))
          {				
						if((readyfsk==1)/*&&(Xamsung==0)*/)
						{
#if NEW_TEMPDET_SLOW_CHARGING									
							if((temp_slow_state == 1))
#endif										
							{
#if NEW_TEMPDET_SLOW_CHARGING										
								temp_slow_state = 2;
#endif								
								readyfsk=0;
								continue;
							}
					  }			
            readyfsk=0;
            Fsksend_sa();
						//printf("FSK for samsung!!\n");
            continue;
          }
					
           if((ctrl_error_val==-10)&&(Apple==1)&&(last_ctrl_error_val==0)) 
           {
             ipa=GetIpa();
             vpa=GetVpa();
             papower=(ipa/10)*(vpa/100);
             if(papower>4000)
             {
                ctrl_error_val=0;
         //       printf("a-n\r\n");
             }
           }          
          
          last_ctrl_error_val = (int_8)byte1;
          Delay_ms(p_tx_info->t_delay);    
#if ECH
  
			if(ech_state >= ECH_STATE_SEND_HANDSHAKE && first_ce_zero == 1 && ctrl_error_val == 0)
			{
					Fsksend_ech_power(2,4);  //1- <5W 2- 5W 3- 7.5W 4 10W 5- 12.5W 6- 15W
					first_ce_zero = 0;
					//ech_state = ECH_STATE_HANDSHAKE_OK;
					continue;
					
			}
			if ((ctrl_error_val == -127) 
				&&(ech_state >= ECH_STATE_HANDSHAKE_OK))
			{
				power_ctrl_pram.fre += 35000;
				if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
				  power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
				//PWMTimer1Set( power_ctrl_pram.fre,  power_ctrl_pram.duty_cycle);
				PWMSet();
				ctrl_error_val = 0;
				printf("ech%ld\r\n", power_ctrl_pram.fre);
			}
#endif					
//#if DPLL
            if(cebutiao!=0) 
						{
              cebutiao--;
							ctrl_error_val = 0;
            }
//#endif
						printf("CE=%bd \n",ctrl_error_val);  
						if(ASKch)
							printf("ASK1 \n");
						else
							printf("ASK0 \n");
						
						if ((power_ctrl_pram.cecnt_after_toFB)&& (power_ctrl_pram.cecnt_after_toFB < 0xff))
							power_ctrl_pram.cecnt_after_toFB++;											
            if((ctrl_error_val!=0)&&((cebutiao==0)||(ctrl_error_val<0)))
            {
                 Ctrl_PIDAlgorithm( ctrl_error_val); //ta(j-1), c(j)
            }
            Delay_ms(TIMING_POWER_CONTROL_SETTLING_TIME);     
						
        }
        else if(header == RX_PACKET_TYPE_CHARGE_STATUS)
        {        
              if(byte1==100)
              {
                  complete=1;
#if !S50B								
                  LED1=1;
#endif								
									printf("Charging=%bd!!\r\n",byte1);
#if NEW_TEMPDET_SLOW_CHARGING
									s8_bl = 100;
#endif								
              }
              else
              {   
#if !S50B								
                LED1=0;
#endif								
                complete=0;
              }
        } 
#if SAMSUNG        
        else if((header == 0x18)&&(QC==2))
        {
              Samsung=1;
							printf("It's samsung\n");
        }

        else if (header == 0x28)
        {
					  printf("0x28,%bx,%bx \r\n",byte1,byte2);
            if((byte1==0x06)&&(byte2==0x2c))
            {
#if FORT20
							hftofb=1;
#else							
              cecount=1;
#endif	
			        Samsung=1;
			//				printf("Ready to full buridge 28 06 2C\n");
							continue;
            }
						else if((byte1==0x00)&&(byte2==0x01))
            {
#if FORT20
							hftofb=1;
#else							
              cecount=1;
#endif	
			        Samsung=1;
			//				printf("Ready to full buridge 28 00 01\n");
							continue;
            }
            else if((byte1==0x0c)&&(byte2==0x00))
            {
#if NEW_TEMPDET_SLOW_CHARGING
							if (temp_slow_state == 0)
#endif							
							readyfsk=2;
							continue;
            }
            else if((byte1==0x01)&&(byte2==0x00))
            {
#if NEW_TEMPDET_SLOW_CHARGING
							if (temp_slow_state == 0)
#endif							
              readyfsk=2;
							continue;
            }         
#if NEW_TEMPDET_SLOW_CHARGING
						else if(byte1==0x0f)/*sangsung S8 power level*/
						{
							s8_bl = byte2;
						}
#endif		
						
		}
#endif
#if ECH
       else if (header == RX_PACKET_TYPE_ECH_HANDSHAKE)
       {
          printf("0x19 %x\r\n", p_tx_receive_packet->msg[0]);
          if (ech_state == ECH_STATE_SEND_HANDSHAKE && first_ce_zero == 0 && p_tx_receive_packet->msg[0]> 5*2){
            
            ech_state = ECH_STATE_HANDSHAKE_OK;
            ECH_Rx_Required_Power = ((unsigned char)p_tx_receive_packet->msg[0])/2;
              
            Delay_ms(5);
            power_ctrl_pram.fre +=15000;
            if(power_ctrl_pram.fre>OPREATE_RANGE_FRE_MAX)
                power_ctrl_pram.fre=OPREATE_RANGE_FRE_MAX;
            PWMSet();
						PWMEn(2); 
            printf("FB\r\n");
            power_ctrl_pram.cecnt_after_toFB = 1;
          }else if(ech_state == ECH_STATE_SEND_HANDSHAKE && first_ce_zero == 0 && p_tx_receive_packet->msg[0] <= 5*2)
          {
            ech_state = ECH_STATE_HANDSHAKE_OK;
            ECH_Rx_Required_Power = ((unsigned char)p_tx_receive_packet->msg[0])/2;;
          }
       }
#endif
			else if((header == RX_PACKET_TYPE_SIGNAL_STRENGTH)
			||(header == RX_PACKET_TYPE_CONFIGURATION)
			||(header == RX_PACKET_TYPE_INDENTIFICATION)
			||(header == RX_PACKET_TYPE_EXTENDED_IDENTIFICATION))
        {  
					Disable_coil_power();
					Delay_ms(10);
					tx_phase = TX_PHASE_SELECT;
					printf("Wrong pkt(0x%bx)\n",header);
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
					exit_pt_info.reason_cur = EXIT_PT_REASON_WRONG_PACKET;
#endif		
					break;
        } 
			else if(header == RX_PACKET_TYPE_POWER_CONTROL_HOLD_OFF)
			{
//				if(BPP)
				{
					if(firstce==1)
					{
					Delay_ms(5);	
					Disable_coil_power();
					Delay_ms(10);
					tx_phase = TX_PHASE_SELECT;
//					printf("Wrong pkt(0x%bx)\n",header);
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
					exit_pt_info.reason_cur = EXIT_PT_REASON_WRONG_PACKET;
#endif		
					break;	
					}						
				}
			}
#if CALIB			
			else if(header==0x3f)
			{
				if(byte1==0xec)
				{
					ipa=GetIpa();
//					printf("Ipa=%hu \n",ipa);
					ipa0=(u8)(ipa/100);
					ipa1=(u8)(ipa-(u16)ipa0*100);
					Flash(byte2,byte3,ipa0,ipa1);
//					printf("%hd\n",(u16)Data_Flash[2]*100+Data_Flash[3]);
					printf("ccok %hd\n",(u16)Data_Flash[0]*100+Data_Flash[1]);
					
				}
			}
#endif						
    }

    if(tx_phase != TX_PHASE_PT)
    {
      TIMER_1ms_ENABLE(DISABLE);
			Disable_coil_power();
			FAN=0;
			
#if FPDDET_EXITPT3TIMES_SUPPORT	
			if((exit_pt_info.reason_last == exit_pt_info.reason_cur)&&(exit_pt_info.reason_cur!=EXIT_PT_REASON_UNKNOW))
			{
				exit_pt_info.pt_cnt += 1;
				if((ech_state > ECH_STATE_INIT)&&(exit_pt_info.reason_cur==EXIT_PT_REASON_EPT)&&(p_tx_receive_packet->msg[0]==EPT_VALUE_UNKNOWN))
				{
					exit_pt_info.pt_cnt-=1;
				}
				if(3<=exit_pt_info.pt_cnt)
				{
					printf("exit pt 3 time \n");
					exit_pt_info.exit_pt_flag = 1;
				}
			}
			else
			{
				exit_pt_info.pt_cnt = 1;
			}
			printf("[pt];reason_cur=%bu, reason_last=%bu, exit_pt_cnt=%bu\n",\
           exit_pt_info.reason_cur,exit_pt_info.reason_last,exit_pt_info.pt_cnt);
			exit_pt_info.reason_last = exit_pt_info.reason_cur;

#endif			
			
    }
#if NEW_TEMPDET_SLOW_CHARGING
		if (temp_slow_state == 2)
			temp_slow_state = 0;
#endif
    return;
}




