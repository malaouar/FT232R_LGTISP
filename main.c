
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
//#include <windows.h>	// for Sleep() function if needed

#include <ftd2xx.h>
#include "swd.h"


void program_page(uint16_t address, uint8_t len);

uint8_t buf[128];

FT_HANDLE handle;


//===================================================================
int main(int argc, char *argv[]){
	FILE *fp;
	int i, j,  pages, count; 
	
	if(argc<2){
		printf("HEX file needed!\n");
		return 1;
	}
	
	//printf("File to open is %s\n", argv[1]);
	
	fp = fopen(argv[1], "r");
	
	if(fp == NULL){
		printf("OOOOPS Opening %s error!!\n", argv[1]);
		return 1;
	}
	
	// get bytes count
	count = 0;
	while(1){
		if(fgetc(fp) == EOF) break; 
		count++;
	}
	//printf(" Nr of bytes = %d\n", count);
	
	pages=0;
	pages = count/128;
	if(count % 128) pages++;
	//printf(" Pages = %d\n", pages);
	
	rewind(fp);			// lseek = 0

	//============================
    // Initialize, open device, set bitbang mode 
    if(FT_Open(0, &handle) != FT_OK) {
        puts("Can't open device");
        return 1;
    }
	printf("Programmer found and ready....\n");
	
    FT_SetBaudRate(handle, 9600);  		// freq = 9600 * 16
	
	SWD_init();
	
	start_pmode();			// Enter programming mode and erase flash
	
	printf("Programming .........");
	for (i = 0; i < pages; i++){
		memset(buf, 0xFF, sizeof(buf));
		if(i != (pages-1))	for (j = 0; j < 128; j++) buf[j] = fgetc(fp);
		else for (j = 0; j < (count % 128); j++) buf[j] = fgetc(fp);		
		program_page(i*64, 128);
		printf(".......");
	}
	printf(".........DONE!!\n");

	end_pmode();			// Exit programming mode
	
	FT_Close(handle);

	return 0;
}


//===================================================================
void program_page(uint16_t address, uint8_t len) {
	uint16_t addr = address / 2;
	int i;	

	// flash memory @address, (length) bytes
	SWD_EEE_CSEQ(0x00, addr);
	SWD_EEE_CSEQ(0x84, addr);
	SWD_EEE_CSEQ(0x86, addr);
	
	for (i = 0; i < len; i += 4){
		SWD_EEE_Write(*((uint32_t *)(&buf[i])), addr);
		++addr;
	}
	
	SWD_EEE_CSEQ(0x82, addr - 1);
	SWD_EEE_CSEQ(0x80, addr - 1);
	SWD_EEE_CSEQ(0x00, addr - 1);
}
