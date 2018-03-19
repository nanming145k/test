#ifndef __PT_H
#define __PT_H

#include "N76E003.h"
#include "common.h"
#include "rcvpkt.h"
#include "hal.h"
#include "selection.h"
#include "ping.h"
#include "idcfg.h"
#include "Delay.h"
#include <stdio.h>

void TX_Phase_ProcessPT( TX_DETECT_INFO_S   			*p_tx_info);

#endif