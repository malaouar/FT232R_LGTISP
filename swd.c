
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include <ftd2xx.h>

#include "swd.h"


uint8_t data;		
uint8_t dir;		// Pins direction status (0=IN or 1=OUT)
DWORD bytes;

/*
//===================================================================
// microsecond delay
void //uSleep(int waitTime) {
    __int64 time1 = 0, time2 = 0;

    QueryPerformanceCounter((LARGE_INTEGER *) &time1);

    do {
        QueryPerformanceCounter((LARGE_INTEGER *) &time2);
    } while((time2-time1) < waitTime);
}
*/
//===================================================================
void SWC_CLR(){
	data &= ~SWD_CLK;
	FT_Write(handle, &data, 1, &bytes);	
}

//===================================================================
void SWC_SET(){
	data |= SWD_CLK;
	FT_Write(handle, &data, 1, &bytes);	
}
//===================================================================
void SWD_CLR(){
	data &= ~SWD_DAT;
	FT_Write(handle, &data, 1, &bytes);	
}
//===================================================================
void SWD_SET(){
	data |= SWD_DAT;
	FT_Write(handle, &data, 1, &bytes);	
}

//===================================================================
void RST_CLR(){
	data &= ~SWD_RST;
	FT_Write(handle, &data, 1, &bytes);	
}
//===================================================================
void RST_SET(){
	data |= SWD_RST;
	FT_Write(handle, &data, 1, &bytes);	
}

//===================================================================
void SWD_IND(){
	dir &= ~SWD_DAT;
	FT_SetBitMode(handle, dir, 1);		// change DATA (DTR) to Input
}
//===================================================================
void SWD_OUD(){
	dir |= SWD_DAT;
	FT_SetBitMode(handle, dir, 1);		// change DATA (DTR) to Output	
}


//===================================================================
void SWD_WriteByte(unsigned char start, unsigned char data, unsigned char stop){
	unsigned char cnt;
	
	if ( start ) {
		SWC_CLR();
		SWD_CLR();
		SWC_SET();
	}
	
	// send data
	for( cnt = 0; cnt < 8; cnt++ ){
		SWC_CLR();
		if ( data & 0x1 )	SWD_SET(); 
		else	SWD_CLR();
		data >>= 1;
		SWC_SET();
	}
	
	SWC_CLR();
	if ( stop ) 	SWD_SET();
	else 	SWD_CLR();
	
	SWC_SET();
}

//===================================================================
unsigned char SWD_ReadByte(unsigned char start, unsigned char stop){
	unsigned char rdata, cnt;
	unsigned char bRes = 0;
	
	if ( start ){
		SWC_CLR();
		SWD_CLR();
		SWC_SET();
	}
	
	SWD_IND();		// DATA line Input
	
	for( cnt = 0; cnt < 8; cnt++ ){
		bRes >>= 1;
		SWC_CLR();
		FT_GetBitMode(handle, &rdata); 
		if (rdata & SWD_DAT )	bRes |= 0x80;
		SWC_SET();
	}
	
	SWD_OUD();	// DATA line Output
	
	SWC_CLR();
	if( stop )  SWD_SET();
	else 	SWD_CLR();
	SWC_SET();
	return bRes;
}

//===================================================================
void SWD_init(){
	// set SWD pins to output
	dir = (SWD_CLK | SWD_DAT |  SWD_RST);
	FT_SetBitMode(handle, dir, 1);		// CLK (TX), DATA (DTR) and RESET (RX) Outputs
	// set DATA , CLK and RESET high
	data = 0;
	RST_SET();
	SWD_SET();
	SWC_SET();
}

//===================================================================
void SWD_Idle( unsigned char cnt ){
	volatile unsigned char i;
	
	SWD_SET();
	
	for( i = 0; i < cnt; i++ ){
		SWC_CLR();
		SWC_SET();
	}
}

//===================================================================
void SWD_exit(){
	// Disable: halt CPU and lock flash after reset
	SWD_WriteByte(1, 0xb1, 0);
	SWD_WriteByte(0, 0x0d, 1);
	SWD_Idle(2);
	
	//uSleep(200); // wait 200 uS
	
	// software reset
	SWD_WriteByte(1, 0xb1, 0);
	SWD_WriteByte(0, 0x0c, 1);
	SWD_Idle(2);
	SWD_Idle(40);
}

//===================================================================
void SWD_ReadSWDID( char *pdata ){
	SWD_WriteByte(1, 0xae, 1);
	SWD_Idle(4);
	pdata[0] = SWD_ReadByte(1, 0);
	pdata[1] = SWD_ReadByte(0, 0);
	pdata[2] = SWD_ReadByte(0, 0);
	pdata[3] = SWD_ReadByte(0, 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_SWDEN(){
	SWD_WriteByte(1, 0xd0, 0);
	SWD_WriteByte(0, 0xaa, 0);
	SWD_WriteByte(0, 0x55, 0);
	SWD_WriteByte(0, 0xaa, 0);
	SWD_WriteByte(0, 0x55, 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_UnLock0(){
	SWD_WriteByte(1, 0xf0, 0);
	SWD_WriteByte(0, 0x54, 0);
	SWD_WriteByte(0, 0x51, 0);
	SWD_WriteByte(0, 0x4a, 0);
	SWD_WriteByte(0, 0x4c, 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_UnLock1(){
	SWD_WriteByte(1, 0xf0, 0);
	SWD_WriteByte(0, 0x00, 0);
	SWD_WriteByte(0, 0x00, 0);
	SWD_WriteByte(0, 0x00, 0);
	SWD_WriteByte(0, 0x00, 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_UnLock2(){
	SWD_WriteByte(1, 0xf0, 0);
	SWD_WriteByte(0, 0x43, 0);
	SWD_WriteByte(0, 0x40, 0);
	SWD_WriteByte(0, 0x59, 0);
	SWD_WriteByte(0, 0x5d, 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_EEE_CSEQ(uint8_t ctrl, uint16_t addr){
	SWD_WriteByte(1, 0xb2, 0);
	SWD_WriteByte(0, (addr & 0xff), 0);
	SWD_WriteByte(0, ((ctrl & 0x3) << 6) | ((addr >> 8) & 0x3f), 0);
	SWD_WriteByte(0, (0xC0 | (ctrl >> 2)), 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_EEE_DSEQ(uint32_t data){
	SWD_WriteByte(1, 0xb2, 0);
	SWD_WriteByte(0, ((uint8_t *)&data)[0], 0);
	SWD_WriteByte(0, ((uint8_t *)&data)[1], 0);
	SWD_WriteByte(0, ((uint8_t *)&data)[2], 0);
	SWD_WriteByte(0, ((uint8_t *)&data)[3], 1);
	SWD_Idle(4);
}

//===================================================================
void SWD_EEE_Write(uint32_t data, uint16_t addr){
	SWD_EEE_DSEQ(data);
	SWD_EEE_CSEQ(0x86, addr);
	SWD_EEE_CSEQ(0xc6, addr);
	SWD_EEE_CSEQ(0x86, addr);
}

//===================================================================
uint8_t SWD_EEE_GetBusy(){
	uint8_t res = 0;
	
	SWD_WriteByte(1, 0xaa, 1);
	SWD_Idle(8);
	SWD_ReadByte(1, 0);
	SWD_ReadByte(0, 0);
	res = SWD_ReadByte(0, 1);
	SWD_Idle(8);
	return res & 0x1;
}

//===================================================================
void SWD_ChipErase(){
	/* reset flash to 0xff */
	// I guess it is to operate the FLASH access control register-EECR
	SWD_EEE_CSEQ(0x00, 1);
	SWD_EEE_CSEQ(0x98, 1);
	SWD_EEE_CSEQ(0x9a, 1);
	//uSleep(200);
	SWD_EEE_CSEQ(0x8a, 1);
	//uSleep(20);
	SWD_EEE_CSEQ(0x88, 1);
	SWD_EEE_CSEQ(0x00, 1);
}

//===================================================================
// Crack the read decryption protection (read the flash except the first 1k, the first 1k will be erased)
void crack(){
	SWD_EEE_CSEQ(0x00, 1);
	SWD_EEE_CSEQ(0x98, 1);
	SWD_EEE_CSEQ(0x92, 1); //  Will erase the first page of flash (1024 bytes)
	//uSleep(200);
	SWD_EEE_CSEQ(0x9e, 1); // unlock
	//uSleep(200);
	SWD_EEE_CSEQ(0x8a, 1);
	//uSleep(20);
	SWD_EEE_CSEQ(0x88, 1);
	SWD_EEE_CSEQ(0x00, 1);
}

//===================================================================
uint32_t SWD_EEE_Read(uint16_t addr){
	uint32_t data;
	
	SWD_EEE_CSEQ(0xc0, addr);
	SWD_EEE_CSEQ(0xe0, addr);
	
	SWD_WriteByte(1, 0xaa, 1);
	((uint8_t *)&data)[0] = SWD_ReadByte(1, 0);
	((uint8_t *)&data)[1] = SWD_ReadByte(0, 0);
	((uint8_t *)&data)[2] = SWD_ReadByte(0, 0);
	((uint8_t *)&data)[3] = SWD_ReadByte(0, 1);
	SWD_Idle(4);
	return data;
}


//===================================================================
uint8_t SWD_UnLock(uint8_t chip_erase){
	char swdid[4];
	char flag[2];
	
	SWD_ReadSWDID(swdid); 
	// {0x3e, 0xa2, 0x50, 0xe9} indicates that this is the first SWD operation, 
	// {0x3f, 0xa2, 0x50, 0xe9} indicates that the SWD unlock operation has been performed previously (before)
	SWD_SWDEN();
	
	if ( ! (swdid[0] == 0x3e || swdid[0] == 0x3f) ) return 0; // invalid device
	if ( swdid[0] == 0x3f && chip_erase == 0 ) return 1; // Has been unlocked, and not completely erased
	if ( swdid[0] == 0x3e ) SWD_UnLock0();	// unlock for the first time
	if ( chip_erase == 1 ) SWD_ChipErase();	// full erase
	else if ( chip_erase == 2 )  crack();	// partial erase
	
	if ( swdid[0] == 0x3e ){ // unlock for the first time
		SWD_UnLock1();
		// At this point swdid[0] == 0x3f
		
		SWD_WriteByte(1, 0xb1, 0);
		SWD_WriteByte(0, 0x3d, 0);
		SWD_WriteByte(0, 0x60, 0);
		SWD_WriteByte(0, 0x0c, 0);
		SWD_WriteByte(0, 0x00, 0);
		SWD_WriteByte(0, 0x0f, 1);
		SWD_Idle(40);
		
		SWD_UnLock2();
	}
	
	SWD_Idle(40);
	
	SWD_WriteByte(1, 0xb1, 0);
	SWD_WriteByte(0, 0x0c, 0);
	SWD_WriteByte(0, 0x00, 0);
	SWD_WriteByte(0, 0x17, 1);
	SWD_Idle(40);
	
	SWD_WriteByte(1, 0xa9, 1);
	SWD_Idle(4);
	flag[0] = SWD_ReadByte(1, 0);
	flag[1] = SWD_ReadByte(0, 1);
	SWD_Idle(4);
	
	if ( flag[1] == 0x20 ) {
		SWD_WriteByte(1, 0xb1, 0);
		SWD_WriteByte(0, 0x3d, 0);
		SWD_WriteByte(0, 0x20, 0);
		SWD_WriteByte(0, 0x0c, 0);
		SWD_WriteByte(0, 0x00, 0);
		SWD_WriteByte(0, 0x0f, 1);
		SWD_Idle(40);
	}
	else
	if ( flag[1] == 0x60 ) {}// 0x60 does not have this command
	else return 0; // Leaving the reset pin unconnected returns this
	
	SWD_WriteByte(1, 0xb1, 0);
	SWD_WriteByte(0, 0x0d, 1);
	SWD_Idle(2);
	
	return 1;
}

//===================================================================
void ReadGUID( char *guid ){
	SWD_Idle(10);
	SWD_WriteByte(1, 0xa8, 1);
	SWD_Idle(4);
	guid[0] = SWD_ReadByte(1, 0);
	guid[1] = SWD_ReadByte(0, 0);
	guid[2] = SWD_ReadByte(0, 0);
	guid[3] = SWD_ReadByte(0, 1);
	SWD_Idle(4);
}

//===================================================================
uint8_t read_lockbits(){
	char swdid[4];
	
	SWD_ReadSWDID(swdid); 
	return swdid[0];
}

//===================================================================
// Enter programming mode
void start_pmode(){
	uint8_t pmode=0;
	
	RST_CLR();
	SWD_Idle(10);
	pmode = SWD_UnLock(1);
	if ( ! pmode )	pmode = SWD_UnLock(1); // retry	
}

//===================================================================
// Leave programming mode
void end_pmode(){
	SWD_exit();
	RST_SET();
}
