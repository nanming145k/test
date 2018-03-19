#include "idcfg.h"

#if ECH
ECH_STATE_E	ech_state = ECH_STATE_INIT;
#endif


void TX_Phase_ProcessCfg(TX_DETECT_INFO_S   			*p_tx_info)
{
    uint_8 optional_packet_cnt = 0;
    u8 order=0;
	  u8	id_ext = 0;
	  u8 t_ce_cnt_1ms_timeout = 23;
    RX_PACKET_STRUCTURE_S   		*p_tx_receive_packet = NULL;
//	  u16 ipa;

    p_tx_info->t_delay = TIMING_POWER_CONTROL_HOLD_OFF_TIME_MIN;
    t_ce_cnt_1ms = 0;
    
	  Reset_Receive_Packet();

    do
    {
      
        if(( t_ce_cnt_1ms >= t_ce_cnt_1ms_timeout)&&
					(tx.rcv_packet_info_v.packet_part<=1)&&
					(tx.rcv_packet_info_c.packet_part<=1))
        {
                tx_phase = TX_PHASE_SELECT;
								Delay_ms(10);
                break; 
        }
        
				if(Receive_Packet(&p_tx_receive_packet))
					continue;

		t_ce_cnt_1ms = 0;
		switch ( p_tx_receive_packet->header)
		{
			/* receive ID Packet */
			case RX_PACKET_TYPE_INDENTIFICATION:			//鉴别包，判断是三星，苹果还是普通机
			{
				t_ce_cnt_1ms=0;
				if(order==0)
					order=1;
				t_ce_cnt_1ms_timeout = 30;
				id_ext = p_tx_receive_packet->msg[3]&0x80;
//				id1=p_tx_receive_packet->msg[1];
//				id2=p_tx_receive_packet->msg[2];
				vqi=p_tx_receive_packet->msg[0];
//				printf("vqi=%bx\n",vqi);
				ech_state = ECH_STATE_INIT;
				if( (p_tx_receive_packet->msg[2] == 0x42)&&(p_tx_receive_packet->msg[3]== 0x64))
				{
					Xamsung = 1;
					if((p_tx_receive_packet->msg[4] == 0x40)&&(p_tx_receive_packet->msg[5]== 0x16))
					{
						ech_state = ECH_STATE_ID_OK;
					}
				}
				else
					Xamsung = 0;
				#if ECH
					if ((p_tx_receive_packet->msg[1] == 0x00)
						&&(p_tx_receive_packet->msg[2] == 0x59)
						&&(p_tx_receive_packet->msg[3] == 0x64))
					{
					 if (QC == 2)
							ech_state = ECH_STATE_SEND_HANDSHAKE; 
						else
							ech_state = ECH_STATE_ID_OK;
						printf("ECH_ID\r\n");
					}
				#endif

				if( (p_tx_receive_packet->msg[1] == 0x00)&&(p_tx_receive_packet->msg[2] == 0x5A))
					Apple=1;
				else
					Apple=0;

				if((p_tx_receive_packet->msg[3]==0x30)&&(p_tx_receive_packet->msg[4]==0x31)&&(p_tx_receive_packet->msg[5]==0x32)&&(p_tx_receive_packet->msg[6]==0x33))
				{
				  BPP=1;
				}
			  else
				{
				  BPP=0;
					sfod=0;					
					if(Xamsung)
					{
					if(((sipa>105)&&(QC==0))||((sipa>70)&&(QC==2)))
							{
								sfod=1;						
							}
					}
					else if(Apple)
					{ 
							if(((sipa>70)&&(QC==0))||((sipa>50)&&(QC==2)))
							{
								sfod=1;
							}
					}
					else
					{
						 if(((sipa>150)&&(QC==0))||((sipa>90)&&(QC==2)))
						 {
							sfod=1;
						 }
					}
					if(sfod)
						printf("sipa large!\n");
			}
//					printf(" %bu\n",p_tx_receive_packet->msg[0]);

			}
			break;
			
			/* receive EXT ID Packet*/ 
			case RX_PACKET_TYPE_EXTENDED_IDENTIFICATION:
			{
				t_ce_cnt_1ms=0;
				if(!id_ext)
				{
					tx_phase = TX_PHASE_SELECT;
				}
			}
			break;

			/* receive CFG Packet*/
			case RX_PACKET_TYPE_CONFIGURATION:
			{
				t_ce_cnt_1ms=0;
				if(order<1)		//判断是否鉴别
				{
					tx_phase = TX_PHASE_SELECT;  
					break;
				}
				order=2;

				p_tx_info->max_power_value = p_tx_receive_packet->msg[0]&0x3F;
//				printf("mpv=%bd \n",p_tx_info->max_power_value);
				if( (p_tx_receive_packet->msg[2]&0x07) != optional_packet_cnt)
				{       
					tx_phase = TX_PHASE_SELECT;
					Delay_ms(10);
					break;
				}
				tx_phase = TX_PHASE_PT;
			}
			break;

			case RX_PACKET_TYPE_END_POWER_TRANSFER://EPT
			{
				tx_phase = COMPLETE;
//#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
//				exit_pt_info.reason_cur = EXIT_PT_REASON_EPT;
//#endif
			}
			break;
			               
			//下都是count要+1的
			case RX_PACKET_TYPE_POWER_CONTROL_HOLD_OFF:
			{
				t_ce_cnt_1ms=0;
				if(order!=1)
				{
					tx_phase = TX_PHASE_SELECT;  
					break;
				}
				p_tx_info->t_delay = p_tx_receive_packet->msg[0];
				if( (p_tx_info->t_delay<TIMING_POWER_CONTROL_HOLD_OFF_TIME_MIN)
				          ||(p_tx_info->t_delay>TIMING_POWER_CONTROL_HOLD_OFF_TIME_MAX))
				{
				     tx_phase = TX_PHASE_SELECT;
				     Delay_ms(50);
				     break;
				}
			}                   
			case 0x18:
			case 0x19:
			case 0x28:
			case 0x29:
			case 0x38:
			case 0x48:
			case 0x58:
			case 0x68:
			case 0x78:
			case 0x84:
			case 0xa4:
			case 0xc4:
			case 0xe2:     
			{
				t_ce_cnt_1ms=0;
				if(order==0)
				{
//					printf("Wrong order\n");
					tx_phase = TX_PHASE_SELECT;    
					break;
				} 
				optional_packet_cnt += 1;
				break;
			}
			default:
			break;
		}
		
    } while(tx_phase == TX_PHASE_IDCFG );	// cfg packet

  	if (tx_phase != TX_PHASE_PT)
  	{
//		Delay_ms(5);
		Disable_coil_power();
		TIMER_1ms_ENABLE(DISABLE);
#if NEW_TEMPDET_SLOW_CHARGING			
		temp_slow_state = 0;
#endif			
  	}
    return;  
}
