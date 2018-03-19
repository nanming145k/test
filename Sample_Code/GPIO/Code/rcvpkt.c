#include <stdio.h>
#include <string.h>
#include "Common.h"
#include "N76E003.h"
#include "Define.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "hal.h"
#include "rcvpkt.h"


unsigned char Cul_MsgLen( unsigned char i)
{
	unsigned char len;

if((i>0)&&(i<0x20))
	len=1;
else if((i>=0x20)&&(i<0x30))
	len=2;
else if((i>=0x30)&&(i<0x40))
	len=3;
else if(i==0x48)
	len=4;
else if((i>0x50)&&(i<0x60))
	len=5;
else if(i==0x68)
	len=6;
else if((i>0x70)&&(i<0x80))
	len=7;
else if((i>0x80)&&(i<0x90))
	len=8;
else if(i==0xA4)
	len=12;
else if(i==0xC4)
	len=16;
else if(i==0xE2)
	len=20;

 return len;
}

ECH_ERR_TYPE_E  Pakcet_Check_Checksum(	RX_PACKET_STRUCTURE_S	*p_tx_receive_packet )
{
	unsigned char i = 0 ;
	unsigned char sum = p_tx_receive_packet->header;
	
	for( i=0; i<p_tx_receive_packet->msg_len; i++)
	{
		sum = sum^p_tx_receive_packet->msg[i];
	}
	
	if( sum != p_tx_receive_packet->checksum)
	{
		return ERR_DECODE_ERROR_CHECK_PACKET;
	}
	return ERR_NO_ERROR;
}

void Reset_Receive_Packet(void)
{
	memset(&tx, 0 , sizeof(TX_S));
	return;
}

u8  Receive_Packet(RX_PACKET_STRUCTURE_S   	**p_tx_receive_packet)
{
		u8 status = 0;
    TX_RECEIVE_PACKET_INFO_S 	*p_rcv_packet_info_v = &tx.rcv_packet_info_v;		//电压解调信息
		RX_PACKET_STRUCTURE_S   	*p_rcv_packet_struct_v = &tx.rcv_packet_v;			//电压解调包内容
    TX_RECEIVE_PACKET_INFO_S 	*p_rcv_packet_info_c = &tx.rcv_packet_info_c;		//电流解调信息
		RX_PACKET_STRUCTURE_S   	*p_rcv_packet_struct_c = &tx.rcv_packet_c;			//电流解调包的内容
	
	if (*p_tx_receive_packet != NULL) 
	{
		*p_tx_receive_packet = NULL;
		Reset_Receive_Packet();
	}
	
	if((*p_tx_receive_packet == NULL)
		&& (TX_ON == p_rcv_packet_info_c->packet_set))
	{
		if(ERR_NO_ERROR == Pakcet_Check_Checksum( p_rcv_packet_struct_c))
		{
			*p_tx_receive_packet = p_rcv_packet_struct_c;
			ASKch=0;
//			Delay10us(8);//wait another path compelte
//			printf("Ask0 \n");
		}
		else
		{
//			memset( &tx.rcv_packet_c,0,sizeof(RX_PACKET_STRUCTURE_S));
//			memset( &tx.rcv_packet_info_c,0,sizeof(TX_RECEIVE_PACKET_INFO_S));
//			printf("c check error!!! \n");
			if(TX_PHASE_PING==tx_phase)
				Disable_coil_power();
		}
		p_rcv_packet_info_c->packet_set = TX_OFF;	
	}
	
	if((*p_tx_receive_packet == NULL)
		&& (TX_ON == p_rcv_packet_info_v->packet_set))
	{
		if(ERR_NO_ERROR == Pakcet_Check_Checksum( p_rcv_packet_struct_v))
		{
			*p_tx_receive_packet = p_rcv_packet_struct_v;	
			ASKch=1;			
//			Delay10us(8);//wait another path compelte	
//			printf("Ask1 \n");			
		}
		else
		{
//			memset( &tx.rcv_packet_v,0,sizeof(RX_PACKET_STRUCTURE_S));
//			memset( &tx.rcv_packet_info_v,0,sizeof(TX_RECEIVE_PACKET_INFO_S));
//			printf("v check error!!! \n");
			if(TX_PHASE_PING==tx_phase)
				Disable_coil_power();
		}
		p_rcv_packet_info_v->packet_set = TX_OFF;
	}
	
	if (*p_tx_receive_packet == NULL)
		status = 1;
	
	return status;
}

ECH_ERR_TYPE_E   Decode_OneByte( TX_RECEIVE_PACKET_INFO_S	*p_rcv_packet_info)
{
	register uint8_t  data parity_cnt = p_rcv_packet_info->parity_cnt;
	register uint8_t  data bit_cnt = p_rcv_packet_info->bit_cnt;
	register uint8_t  data byte = p_rcv_packet_info->byte_val;
	ECH_ERR_TYPE_E	error_type = ERR_NO_ERROR;
	
	if( bit_cnt==0)//start bit
	{
		p_rcv_packet_info->byte_val = 0;
		byte = p_rcv_packet_info->byte_val;
		
		if( p_rcv_packet_info->bit_val!=0)
		{
			error_type= ERR_DECODE_ERROR_CHECK_BYTE_START;
		}
		parity_cnt = 0;
		bit_cnt ++;
	}
	else if( bit_cnt<9) // data byte
	{
		byte=byte>>1;
		if( p_rcv_packet_info->bit_val==1)
		{
			byte|=0x80;
			parity_cnt+=1;
		}
		bit_cnt ++;
	}
	else if( bit_cnt==9)//check bit
	{
		if( (parity_cnt%2)== p_rcv_packet_info->bit_val)
		{
			error_type = ERR_DECODE_ERROR_CHECK_BYTE_PARITY;
		}
		bit_cnt ++;
	}
	else if( bit_cnt==10)//stop bit
	{
		if( p_rcv_packet_info->bit_val!=1)
		{
			error_type = ERR_DECODE_ERROR_CHECK_BYTE_STOP;
		}
		bit_cnt ++;
	}

	p_rcv_packet_info->byte_val = byte;
	
	if( bit_cnt==11)
	{
		p_rcv_packet_info->byte_set = TX_ON;
		parity_cnt = 0;
		bit_cnt = 0;
		byte = 0;
	}
	
	p_rcv_packet_info->bit_cnt = bit_cnt;
	
	p_rcv_packet_info->parity_cnt = parity_cnt;
	p_rcv_packet_info->error_type = error_type;
	
	return error_type;
} 


void rcv_packet_int_handler( u8 vol)
{
	register uint16_t data  time_diff = 0;
        
   static u16 data time_diffv0 ;
   static u16 data time_diffv1 ;
        
   static u16 data  time_diffc0 ;
   static u16 data time_diffc1 ;
        
	TX_RECEIVE_PACKET_INFO_S	data *p_rcv_packet_info = NULL;
	RX_PACKET_STRUCTURE_S   	data *p_rcv_packet_struct = NULL;
	

	if(vol)
	{
		p_rcv_packet_struct = &tx.rcv_packet_v;
		p_rcv_packet_info = &tx.rcv_packet_info_v;
	}
	else
	{
		p_rcv_packet_struct = &tx.rcv_packet_c;
		p_rcv_packet_info = &tx.rcv_packet_info_c;
	}

	if( (TX_OFF == p_rcv_packet_info->receive_trigger)
		&&(TX_OFF == p_rcv_packet_info->packet_set))
	{

		p_rcv_packet_info->receive_trigger = TX_ON;
		p_rcv_packet_info->cnt_on = TX_TRUE;
		p_rcv_packet_info->packet_part = PREAMBLE;
	}


/*======================= Decode a bit val =========================*/	
	if(vol)
	{
         // time_diffv1 = TIM2_GetCounter(vol);
					time_diffv1=(C1H<<8)+C1L;
          if(time_diffv1>time_diffv0)
						time_diff=time_diffv1-time_diffv0;
          else
						time_diff=65534-time_diffv0+time_diffv1; 
          time_diffv0=time_diffv1;
	}
	else
	{
          //time_diffc1 = TIM2_GetCounter(vol);
					time_diffc1=(C0H<<8)+C0L;
          if(time_diffc1>time_diffc0)
						time_diff=time_diffc1-time_diffc0;
          else
						time_diff=65534-time_diffc0+time_diffc1; 
          time_diffc0=time_diffc1;
	}

if( TX_ON==p_rcv_packet_info->receive_trigger)	
{
	if( p_rcv_packet_info->cnt_on == TX_TRUE)
	{
		if( (time_diff>= DECODE_BIT_CNT_ZERO_L)       // 1 period
				&&(time_diff< DECODE_BIT_CNT_ZERO_H))
		{
			p_rcv_packet_info->bit_val = 0;
			p_rcv_packet_info->bit_set = TX_ON;         
		}		
		else
		{
			if( (time_diff> DECODE_BIT_CNT_ONE_L)   // 1/2 period
				&&(time_diff< DECODE_BIT_CNT_ONE_H))
			{
				p_rcv_packet_info->bit_val = 1;
				if( p_rcv_packet_info->packet_part != PREAMBLE)
				{
					p_rcv_packet_info->cnt_on = TX_FALSE;
				}             
				p_rcv_packet_info->bit_set = TX_ON;
			}       
			else
			{
				p_rcv_packet_info->receive_trigger = TX_OFF;
				if(p_rcv_packet_info->packet_set != TX_ON)
				{
                memset( p_rcv_packet_info,0,sizeof(TX_RECEIVE_PACKET_INFO_S));
   //             memset( p_rcv_packet_struct,0,sizeof(RX_PACKET_STRUCTURE_S));
				}
			}
		}
	}
	else
	{
		p_rcv_packet_info->cnt_on = TX_TRUE;         
	}
	
/*======================= Decode the packet =========================*/	
	if(p_rcv_packet_info->bit_set == TX_ON)
	{

		/********************* PREAMBLE *********************/
		if(p_rcv_packet_info->packet_part == PREAMBLE)
		{
			if(p_rcv_packet_info->bit_val==1)
			{
				p_rcv_packet_info->half_bit_cnt+=1;
			}
			else if(p_rcv_packet_info->bit_val==0)
			{
				if ((QC==2)&&(tx_phase == TX_PHASE_PT)&&(p_rcv_packet_info->half_bit_cnt>8)&&(p_rcv_packet_info->half_bit_cnt<50)&&(vol))
				{
					t_ce_cnt_1ms = 0;
					if(!BPP)
						t_rp_cnt_1ms = 0;
				}
				if(p_rcv_packet_info->half_bit_cnt>4)
				{
					p_rcv_packet_info->bit_cnt = 1;	// received header's start bit
					p_rcv_packet_info->packet_part = HEADER;
				}
				else
				{
					p_rcv_packet_info->half_bit_cnt = 0;
					p_rcv_packet_info->receive_trigger = TX_OFF;
					p_rcv_packet_info->error_type = ERR_DECODE_ERROR_FEW_PERAMBLE_BIT;
				}
			}
		}
		/********************* HEADER *********************/
		else if(p_rcv_packet_info->packet_part == HEADER)
		{
			if( ERR_NO_ERROR == Decode_OneByte( p_rcv_packet_info))
			{
				if( TX_ON == p_rcv_packet_info->byte_set) // finish decode a byte
				{
					p_rcv_packet_info->byte_set = TX_OFF;
					p_rcv_packet_struct->header = p_rcv_packet_info->byte_val;
					p_rcv_packet_struct->msg_len = Cul_MsgLen(p_rcv_packet_struct->header);

					p_rcv_packet_info->packet_part = MESSAGE;

                    if(p_rcv_packet_struct->header==0x03)
                    {
                      t_ce_cnt_1ms = 0;
					  t_pr_cnt_1ms=0;
                    }
                    else if(p_rcv_packet_struct->header==0x04)
                    {
                      t_rp_cnt_1ms=0;
					  t_pr_cnt_1ms=0;
                    }
				}
			}
			else if(ERR_NO_ERROR != p_rcv_packet_info->error_type)
			{
					memset( p_rcv_packet_info, 0, sizeof(TX_RECEIVE_PACKET_INFO_S));
			}
		}
		/********************* MESSAGE *********************/			
		else if(p_rcv_packet_info->packet_part == MESSAGE)
		{
			if( ERR_NO_ERROR == Decode_OneByte( p_rcv_packet_info))
			{
				if( TX_ON == p_rcv_packet_info->byte_set) // finish decode a byte
				{
					p_rcv_packet_info->byte_set = TX_OFF;
					p_rcv_packet_struct->msg[p_rcv_packet_info->byte_cnt] = p_rcv_packet_info->byte_val;
					p_rcv_packet_info->byte_cnt ++;
				}
				
				if(( p_rcv_packet_info->byte_cnt == p_rcv_packet_struct->msg_len)||(p_rcv_packet_info->byte_cnt>=MSG_LEN_MAX)) // finish decode all message
				{
					p_rcv_packet_info->byte_cnt = 0;
					p_rcv_packet_info->packet_part = CHECKSUM;
				}
			}
			else if(ERR_NO_ERROR != p_rcv_packet_info->error_type)
			{
					memset( p_rcv_packet_info, 0, sizeof(TX_RECEIVE_PACKET_INFO_S));
			}
		}
		/********************* CHECKSUM *********************/			
		else if(p_rcv_packet_info->packet_part == CHECKSUM)
		{
			if( ERR_NO_ERROR == Decode_OneByte( p_rcv_packet_info))
			{
				if( TX_ON == p_rcv_packet_info->byte_set) // finish decode a byte
				{
					p_rcv_packet_info->byte_set = TX_OFF;
					p_rcv_packet_struct->checksum = p_rcv_packet_info->byte_val;
					p_rcv_packet_info->packet_set = TX_ON;
				}
			}
			else if(ERR_NO_ERROR != p_rcv_packet_info->error_type)
			{
					memset( p_rcv_packet_info, 0, sizeof(TX_RECEIVE_PACKET_INFO_S));
			}
		}
		p_rcv_packet_info->bit_set = TX_OFF;//
	}
}
/*======================= Reset Decode Environment Variable =========================*/
//	if( (p_rcv_packet_info->receive_trigger == TX_OFF)
//		||(ERR_NO_ERROR != p_rcv_packet_info->error_type))//
//	{
//		p_rcv_packet_info->bit_cnt = 0;
//		p_rcv_packet_info->byte_val = 0;
//		p_rcv_packet_info->parity_cnt = 0;
//		p_rcv_packet_info->half_bit_cnt = 0;

//		p_rcv_packet_info->error_type = ERR_NO_ERROR;
//	}

	return;
}