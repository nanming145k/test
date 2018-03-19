#ifndef __HAL_H
#define __HAL_H



void Flash(u8 rpH,u8 rpL,u8 paH,u8 paL);
void InitPWM(void);
void SetPWM(uint32_t fre,uint16_t duty);
void PWMEn(u8 type);
void PWMSet(void);
void Disable_coil_power(void);
void Enable_coil_power(void);

u16 GetTemp(void);
u16 GetVpeak(void);
u16 GetVpa(void);
u16 GetIpa(void);

void InitTimer3(void);
void TIMER_1ms_ENABLE(unsigned char on);

void InitCapture(void);
u16 TIM2_GetCounter(u8 i);

void InitGPIO(void);

void Fsksend_sa(void);
#if ECH
void Fsksend_ech_power(int8_t deep_ech,int8_t power);
#endif

void QC20SHAKE(void);
void QC20OUT9V(void);
void QC20OUT5V(void);
void QC20OUT12V(void);
void run(void);

#endif