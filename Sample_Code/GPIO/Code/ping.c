#include "ping.h"



void TX_Phase_ProcessPing(void )
{  
    RX_PACKET_STRUCTURE_S   		*p_tx_receive_packet = NULL;
    
    TIMER_1ms_ENABLE(ENABLE);				//������ʱ��
    if(sfod==2)											//������
		{
			LED1=1;
			Delay_ms(700);
			LED1=0;
			Delay_ms(1000);
		}
    Enable_coil_power();
    Delay_ms(TIMING_RX_WAKE_UP_TIME);
    Reset_Receive_Packet();
    t_ce_cnt_1ms = 0;
    do
    {
      
      if( t_ce_cnt_1ms >= 70)				//���ճ�ʱ
      {
                Delay_ms(10);
                tx_phase = TX_PHASE_SELECT;
#if FPDDET_EXITPT3TIMES_SUPPORT
								exit_pt_info.pt_cnt=0; 
								exit_pt_info.exit_pt_flag = 0;
#endif					
								sfod=0;
                break; 
      }
     
			if(Receive_Packet(&p_tx_receive_packet)==1)			//�ȴ��������
				continue;
      if( RX_PACKET_TYPE_SIGNAL_STRENGTH == p_tx_receive_packet->header)//SS  �ж�ͷ
      {
        if(sfod==2)
        {
          tx_phase = TX_PHASE_SELECT; 
          break;
        }				
				printf("SS=%bu \n",p_tx_receive_packet->msg[0]);
#if FPDDET_EXITPT3TIMES_SUPPORT				//����������
				if((exit_pt_info.ss_last >= (p_tx_receive_packet->msg[0]+3))||
					(exit_pt_info.ss_last <= (p_tx_receive_packet->msg[0]-3)))		//�жϳ������Ƿ���ͻ��
				{
					exit_pt_info.exit_pt_flag=0;
					exit_pt_info.pt_cnt=0; 
	//				printf("SS changed!! \n");
				}
				exit_pt_info.ss_last = p_tx_receive_packet->msg[0];

#endif
        if(p_tx_receive_packet->msg[0]>5)
        {				
          tx_phase = TX_PHASE_IDCFG;
#if FPDDET_EXITPT3TIMES_SUPPORT				
					if(exit_pt_info.exit_pt_flag!=0)		
					{
						tx_phase = TX_PHASE_SELECT;
						printf("to select\n");
					}
#endif						
        }
				else							
				{
					Disable_coil_power();
					tx_phase = TX_PHASE_SELECT;  
              break;
				}
      }
      else
      {
							
              tx_phase = TX_PHASE_SELECT;  
              break;
      }
    }
    while(tx_phase == TX_PHASE_PING );

	if (tx_phase != TX_PHASE_IDCFG )
	{
//		TIMER_1ms_ENABLE(DISABLE);
		Disable_coil_power();
#if NEW_TEMPDET_SLOW_CHARGING		
		temp_slow_state = 0;
#endif		
	}
}