#include "selection.h"
#include "common.h"
#include "rcvpkt.h"
#include "hal.h"
#include "Delay.h"
#include "stdio.h"

void TX_Phase_ProcessSelect(void)
{
//      uint_16 ipa_alg = 0;
//      static u8 i;
      t_ce_cnt_1ms = 0;
      Enable_coil_power();
      Delay_ms(2);
      sipa=GetIpa(); 
			if(sipa>IPA0)
				sipa-=IPA0;
			else
				sipa=0;

      Disable_coil_power();
			printf("Ipa=%hu \n",sipa); 


#if FODDET        
        if(((sipa>300)&&(QC==0))
			||((sipa>300)&&(QC==2)))
        {
					printf("sipa large!\n");
          tx_phase = TX_PHASE_SELECT; 
          return;
        }
#endif  
     if ((sipa<IPA_THRESHOLD_OBJ_DETECTION)||(sipa>IPA_THRESHOLD_OBJ_DETECTION0)) 
     {
            Delay_ms(5);
            tx_phase = TX_PHASE_PING;
            return;
     }

}