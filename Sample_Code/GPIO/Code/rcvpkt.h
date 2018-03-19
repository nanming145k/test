#ifndef __RCVPKT_H
#define __RCVPKT_H

void Reset_Receive_Packet(void);
void rcv_packet_int_handler( u8 vol);
u8 Receive_Packet(RX_PACKET_STRUCTURE_S   	**p_tx_receive_packet);

#endif