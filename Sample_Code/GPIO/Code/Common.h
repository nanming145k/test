#ifndef __COMMON_H
#define __COMMON_H
typedef bit                   BIT;
typedef unsigned char         UINT8;
typedef unsigned int          UINT16;
typedef unsigned long         UINT32;

typedef unsigned char         uint8_t;
typedef unsigned int          uint16_t;
typedef unsigned long         uint32_t;

typedef   signed char     int8_t;
typedef   signed short    int16_t;
typedef   signed long     int32_t;

typedef int32_t  int_32;
typedef int16_t int_16;
typedef int8_t  int_8;

typedef	uint32_t	u32;
typedef	uint16_t	u16;
typedef	uint8_t	u8;

typedef uint32_t  uint_32;
typedef uint16_t uint_16;
typedef uint8_t  uint_8;

typedef volatile int32_t  vint_32;
typedef volatile int16_t vint_16;
typedef volatile int8_t  vint_8;

typedef volatile uint32_t  vuint_32;
typedef volatile uint16_t vuint_16;
typedef volatile uint8_t  vuint_8;

#define     CID_READ				0x0B
#define     DID_READ				0x0C

#define     ERASE_APROM				0x22
#define     READ_APROM				0x00
#define     PROGRAM_APROM			0x21
#define     ERASE_LDROM				
#define     READ_LDROM				
#define     PROGRAM_LDROM			
#define     READ_CFG					0xC0
#define     PROGRAM_CFG				0xE1
#define			READ_UID					0x04


void  InitialUART0_Timer1(UINT32 u32Baudrate); //T1M = 1, SMOD = 1
//void  InitialUART0_Timer3(UINT32 u32Baudrate); //Timer3 as Baudrate, SMOD=1, Prescale=0
//void  InitialUART1_Timer3(UINT32 u32Baudrate);
//void  Send_Data_To_UART0(UINT8 c);
//UINT8 Receive_Data_From_UART0(void);
//void  Send_Data_To_UART1(UINT8 c);
//UINT8 Receive_Data_From_UART1(void);
//void  InitialUART1(UINT32 u32Baudrate);


#define VCC5V							0
#define CALIB						1   //电流校准功能
#define FODDET          1
#define DPLL            1
#define VPEAKDET        1
#define TEMPDET         1
#define SAMSUNG         1
#define NEW_TEMPDET_SLOW_CHARGING	1
#define FPDDET_EXITPT3TIMES_SUPPORT   1
#define ECH 1
#define FULLNESS 1
#define S50B	0
#define ZENS    0
#define APPLEQC 1
#define NTCOPENSHORT 1
#define FORT20	1


#define DISABLE            0
#define ENABLE             1
#define TX_ON   1
#define TX_OFF  0
#define TX_TRUE         1   
#define TX_FALSE        0  

#define IPA_THRESHOLD_OBJ_DETECTION          18     // mA
#define IPA_THRESHOLD_OBJ_DETECTION0         18

#define INITIALFRE                          146000
#define INITIALDUTY                         300
#define OPREATE_RANGE_DUTY_CYCLE_MIN        200  // %
#define OPREATE_RANGE_DUTY_CYCLE_MAX        500  // %
#define OPREATE_RANGE_FRE_MIN               110000  // Hz
#define OPERATE_SANSUNG_FRE_MIN             110000
#define OPREATE_RANGE_FRE_MAX               146000  // Hz


#define TIMING_POWER_CONTROL_SETTLING_TIME          5       //t_settle, 3-7ms, Target:5ms
#define TIMING_POWER_CONTROL_HOLD_OFF_TIME_MIN      5       //t_delay_min, 5ms
#define TIMING_POWER_CONTROL_HOLD_OFF_TIME_MAX      205     //t_delay_max, 205ms
#define TIMING_RX_WAKE_UP_TIME                      10     //t_wake,19-64ms,target:40ms
#define TIMING_TX_SELECT_SCAN_INTERVAL_TIME         3      //t_interval_selection, scan
#define TIMING_EXECUTE_A_SINGLE_ITERATION           3       //t_inner, ms

#define TIMING_CONTROL_ERROR_PACKET_TIMEOUT         1700	   //t_timeout, 700-1800ms, Target:1500ms    tt
#define TIMING_CONTROL_ERROR_PACKET_TIMEOUT_ECH     500
#define TIMING_RECEIVED_POWER_PACKET_TIMEOUT        23000   //t_power, 8000-24000ms, Target:23000ms  tpower


#define RX_PACKET_TYPE_SIGNAL_STRENGTH              0x01
#define RX_PACKET_TYPE_END_POWER_TRANSFER           0x02
#define RX_PACKET_TYPE_CONTROL_ERROR                0x03
#define RX_PACKET_TYPE_8BIT_RECEIVED_POWER          0x04
#define RX_PACKET_TYPE_CHARGE_STATUS                0x05
#define RX_PACKET_TYPE_POWER_CONTROL_HOLD_OFF       0x06
#define RX_PACKET_TYPE_GENERAL_REQUEST              0x07
#define RX_PACKET_TYPE_RENEGOTIATE                  0x09
#define RX_PACKET_TYPE_SPECIFIC_REQUEST             0x20
#define RX_PACKET_TYPE_FOD_STATUS                   0x22
#define RX_PACKET_TYPE_24BIT_RECEIVED_POWER         0x31
#define RX_PACKET_TYPE_CONFIGURATION                0x51
#define RX_PACKET_TYPE_WPID_1                        0x54
#define RX_PACKET_TYPE_WPID_2                        0x55
#define RX_PACKET_TYPE_INDENTIFICATION              0x71
#define RX_PACKET_TYPE_EXTENDED_IDENTIFICATION      0x81

#define RX_PACKET_TYPE_ECH_HANDSHAKE					0x19

#define EPT_VALUE_UNKNOWN                   0X00
#define EPT_VALUE_CHARGE_COMPLETE           0X01
#define EPT_VALUE_INTERNAL_FAULT            0X02
#define EPT_VALUE_OVER_TEMPERATURE          0X03
#define EPT_VALUE_OVER_VOLTAGE              0X04
#define EPT_VALUE_OVER_CURRENT              0X05
#define EPT_VALUE_BATTERY_FAILURE           0X06
#define EPT_VALUE_RECONFIGURE               0X07
#define EPT_VALUE_NO_RESPONSE               0X08

#define DECODE_BIT_CNT_ONE_L			135
#define DECODE_BIT_CNT_ONE_H			390
#define DECODE_BIT_CNT_ZERO_L			390
#define DECODE_BIT_CNT_ZERO_H			750

#define TEMPTH1        1013			/*70du*/
#define TEMPTH2		   1232			/*60du*/
#define TEMPTH3		   1757			/*40du*/
#define TEMPTH4		   1900			/*35du*/
#define NEW_TEMP_TARGET_H 	1786				/*39du*/
#define NEW_TEMP_TARGET_L 	1843				/*37du*/

#define MAX_PA_VOLTAGE 12000


#if ECH
typedef enum ECH_STATE {
	ECH_STATE_INIT,
	ECH_STATE_ID_OK,
	ECH_STATE_SEND_HANDSHAKE,
	ECH_STATE_HANDSHAKE_OK,
	ECH_STATE_PT_SLOW_CHARGING = ECH_STATE_HANDSHAKE_OK,
	ECH_STATE_PT_FAST_CHARGING,
	ECH_STATE_PT_TUNING_VRECT,
	ECH_STATE_PT_TUNING_EFFI,
}ECH_STATE_E;
#endif
typedef enum t_TX_PHASE        
{       
    TX_PHASE_INVALID = 0,
    
    TX_PHASE_SELECT = 1,
    TX_PHASE_PING,
    TX_PHASE_IDCFG,
    TX_PHASE_PT,
    OVERTEMP,
    COMPLETE,
    OVP
} TX_PHASE_E;

typedef enum t_PID_CTRL_TYPE        
{       
	PID_TYPE_INVALID = 0,
    
    PID_TYPE_FRE = 1,
    PID_TYPE_DUTY,
    PID_TYPE_VOL,
} PID_CTRL_TYPE_E;

typedef struct t_TX_POWER_CONTROL_PARAMETER       
{    
    PID_CTRL_TYPE_E  ctrl_type;
    uint_16  		 duty_cycle; // %      
    uint_32 fre;    	// Hz       
		u8		pid_cnt;
		u8		cecnt_after_toFB;
}TX_POWER_CONTROL_PARAMETER_S;

typedef struct t_TX_DETECT_INFO           
{
    uint_8 				t_delay;
	uint_8 				max_power_value;
} TX_DETECT_INFO_S;


typedef enum t_ECH_ERR_TYPE
{
    ERR_NO_ERROR                    = 0x00,      
        
    ERR_TIMEOUT_ERROR_HALF_PERIOD,          // Get half period timeout, 0x01
    ERR_TIMEOUT_ERROR_GET_BIT,              // Get one bit timeout, 0x02
    ERR_TIMEOUT_ERROR_PACKET,               // Receive Packet time out, 0x03
    ERR_TIMEOUT_ERROR_NOT_DETECTED ,        // not detected, 0x04
    ERR_DECODE_ERROR_CHECK_BYTE_PARITY,     // check parity bit, 0x05
    ERR_DECODE_ERROR_CHECK_BYTE_STOP,       // check stop bit, 0x06
    ERR_DECODE_ERROR_CHECK_BYTE_START,      // check start bit, 0x07
    ERR_DECODE_ERROR_CHECK_PACKET,          // one packet checksum, 0x08
    ERR_DECODE_ERROR_FEW_PERAMBLE_BIT,      // few peramble, 0x09
    
    ERR_PACKET_UNKNOWN_HEADER,              // unknown packet
    ERR_TIMEOUT_NEXT_PACKET,                // t_next
    ERR_TIMEOUT_CONTROL_ERROR_PACKET,       // t_timeout
    ERR_TIMEOUT_RECEIVED_POWER_PACKET,      // t_power
   
    ERROR_UNDEFINED                 = 0xFF
}ECH_ERR_TYPE_E;

#define  MSG_LEN_MAX   20

typedef struct t_RX_PACKET_STRUCTURE          
{           
    unsigned char  header;
    unsigned char msg_len;    
    unsigned char checksum; 
    unsigned char msg[MSG_LEN_MAX];    
} RX_PACKET_STRUCTURE_S;

typedef enum t_PKT_PART
{
    PREAMBLE = 1,
    HEADER, 
    MESSAGE,    
    CHECKSUM, 
    INVALID = 0xFF
}  PKT_PART_E;


typedef struct t_TX_RECEIVE_PACKET_INFO
{
    vuint_8   receive_trigger; 
    vuint_8   half_bit_cnt;
    vuint_8  cnt_on;

    uint8_t		parity_cnt;

    uint_8  bit_cnt;
    vuint_8  bit_val;
    vuint_8  bit_set;

    uint_8   byte_cnt;
    uint_8  byte_val;
    uint_8  byte_set;

    uint_8 packet_set;

    PKT_PART_E   	packet_part;      // indicate the packet part which is being received
    ECH_ERR_TYPE_E  error_type;      
}TX_RECEIVE_PACKET_INFO_S;

typedef struct _TX_STRUCT_PACKET 
{
    TX_RECEIVE_PACKET_INFO_S	rcv_packet_info_v;
    RX_PACKET_STRUCTURE_S		rcv_packet_v;
    
    TX_RECEIVE_PACKET_INFO_S	rcv_packet_info_c;
    RX_PACKET_STRUCTURE_S		rcv_packet_c;
} data TX_S;  //data *P_TX_S;

#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
  // reason
  typedef enum t_EXIT_PT_REASON		//退出电能传输状态的原因
  {
    EXIT_PT_REASON_UNKNOW   = 0,
    EXIT_PT_REASON_EPT,	//3
    EXIT_PT_REASON_CE_TO,	//4
    EXIT_PT_REASON_RP_TO,
    EXIT_PT_REASON_WRONG_RP,
    EXIT_PT_REASON_WRONG_PACKET,
    EXIT_PT_REASON_OVER_VPEAK,
    EXIT_PT_REASON_FOD,
    EXIT_PT_REASON_POWER_LIMIT,
    EXIT_PT_REASON_OVER_TEMP
  }EXIT_PT_REASON_E;
  
  // exit pt info
  typedef struct t_EXIT_PT_INFO
  {
    EXIT_PT_REASON_E reason_cur;
    EXIT_PT_REASON_E reason_last;
    uint8_t ss_last;
 //   uint8_t rx_removed_flag;
    uint8_t pt_cnt;
	  uint8_t exit_pt_flag;
    
  }EXIT_PT_INFO_S;		//退出电能传输状态信息
#endif
	
extern u8 bridge;
extern u32 Fremin;
extern u8 Samsung;
extern uint_16 t_rp_cnt_1ms;
extern uint_16 t_ce_cnt_1ms;
extern uint_16 t_pr_cnt_1ms;
extern TX_POWER_CONTROL_PARAMETER_S	power_ctrl_pram;
extern TX_PHASE_E	tx_phase;
extern TX_S        tx;
extern u8 QC;
extern TX_DETECT_INFO_S        tx_info;
extern u16 IPA0;
extern u8 Xamsung;
extern u8 ASKch;
//extern u8 id2;
//extern u8 id1;
extern u8 Apple;
extern u8 BPP;
extern 	u16 sipa;
extern u16 sfod;
extern volatile unsigned char code Data_Flash[4];
#if S50B
extern u32 ledon;
#endif
#if NEW_TEMPDET_SLOW_CHARGING
  extern u8  temp_slow_state;
#endif
#if FPDDET_EXITPT3TIMES_SUPPORT // exit pt 3 times
  extern EXIT_PT_INFO_S exit_pt_info;
#endif
#if NTCOPENSHORT
extern u8 ntc;
#endif
#if FORT20
extern u8 hftofb;
#endif
extern u8 vqi;

#endif


