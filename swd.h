#ifndef _SWD_LGT8FX8P_H_
#define	_SWD_LGT8FX8P_H_

#define SWD_CLK   0x01 		// TX   clk
#define SWD_RST   0x02 		// RX   reset 
#define SWD_DAT   0x10 		// DTR  data

void SWD_init();
void SWD_exit();
void SWD_Idle(uint8_t cnt);
void SWD_WriteByte(uint8_t start, uint8_t data, uint8_t stop);
uint8_t SWD_ReadByte(uint8_t start, uint8_t stop);
void SWD_EEE_CSEQ(uint8_t ctrl, uint16_t addr);
void SWD_EEE_DSEQ(uint32_t data);

uint8_t SWD_UnLock(uint8_t chip_erase);
void SWD_EEE_Write(uint32_t data, uint16_t addr);
uint32_t SWD_EEE_Read(uint16_t addr);

void ReadGUID(char *guid);
uint8_t read_lockbits();
void start_pmode();
void end_pmode();

// in main.c
extern uint8_t buf[];  
extern FT_HANDLE handle;


#endif
