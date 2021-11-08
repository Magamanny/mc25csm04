// using Pins pack 2, SPI2
// CS=PB12
// 1ms
// AT25040 power V=3.3
/*
// IDE coocox ColIde
Pin connection
25CSM04s | STM32f405RG using SPI2 pin_pack_2
SI -> PB15(MOSI)
SO -> PB14(MISO)
SCK -> PB13(SCK)
CS -> PB12(GPIO)
Vcc -> +3.3V
Vss- > GND
WP -> +3.3V Pull Up
HOLD -> +3.3V Pull Up

25CSM04 SOIC pinout(same as AT25040)
CS = 1
SO = 2
WP = 3
Vss= 4(Ground)
SI = 5
SCK= 6
HOLD=7
Vcc=8(Suppy)
See the microchip datasheet
SCK max, tested on 4MHZ on 8Mhz no response(it may be due to external wiring)
if VCC>=2.5 = 5Mhz
if VCC>=3.0 = 8Mhz ,8Mhz ok on 5 volt
*/

#ifndef MC25CSM04_H
#define MC25CSM04_H

#define WRSR 0x01
#define WRITE 0x02
#define READ 0x03
#define WRDI 0x04
#define RDSR 0x05
#define WREN 0x06

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// I know it can be better, but it will do for now
// currently only support single mc25csm04.
typedef struct mc25csm04_st
{
  void (*chipDeselect)();
  void (*chipSelect)();
  uint8_t (*SPI_transfer)(uint8_t);
  void (*delay_ms)(uint32_t ms);
} mc25csm04_st;
// method to initilize the function structure, dont screw it up, keep the order.
void mc25csm04Init(void (*chipDeselect)(),
					void (*chipSelect)(),
					uint8_t (*SPI_transfer)(uint8_t),
					void (*delay_ms)(uint32_t ms));
// removed as this is casuign problem in c++
/*
uint8_t __attribute__((weak)) SPI_transfer(uint8_t reg);

void __attribute__((weak)) chipDeselect(void);
void __attribute__((weak)) chipSelect(void);
void __attribute__((weak)) delay_ms(uint32_t ms);
*/
/*
//example implementation in main.c
// hal functions used by the driver
// strog functions
// using tm spi lib and general gpio
uint8_t SPI_transfer(uint8_t reg)
{
	return TM_SPI_Send(SPI2, reg);
}
void chipSelect()
{
	// CS high time, 200ns
	for(int i=0;i<50;i++);
	GPIO_WriteBit(CS0_PORT, CS0_PIN, Bit_RESET); // low
	// CS setup time, 200ns(min for 3.3v), try to perform 1us delay
	for(int i=0;i<50;i++);
}
void chipDeselect()
{
	// CS hold time, 200ns(min for 3.3v), try to perform 1us delay
	for(int i=0;i<50;i++);
	GPIO_WriteBit(CS0_PORT, CS0_PIN, Bit_SET); // high
}
 */
// may need to change the name of these functions as they are too general
// May need to change to addr for large ic
void writeDisable();
void writeEnable();
uint8_t readStatus();
void writeStatus(uint8_t st);

void writeByte(uint32_t addr, uint8_t data);
uint8_t readByte(uint32_t addr);

void readString(uint32_t addr, uint8_t *data, uint16_t len);
void writePage(uint32_t addr, uint8_t data[8],uint16_t len);
// Write String implementtaion used in a running project
/*
void writeString(uint32_t addr,char data[])
{
	uint16_t len;
	uint16_t len_a,len_b;
	uint16_t dp=0;
	uint16_t cor=0;
	uint8_t tempData[256]; // to pass to the page
	len=strlen(data);
	data[len]=0; // add null
	len=len+1;
	cor = addr%256;
	len_a = (len+cor)/256;
	for(int i=0;i<len_a;i++)
	{
		// load to a temp data to pass it to the page wire function
		if(addr%256==0) // if multiple of 8
		{
			for(int j=0;j<256;j++,dp++)
			{
				tempData[j]=data[dp];
			}
			writePage(addr,tempData,256); // while write till end of a page
			addr=addr+256;
		}
		else // not multiple of 8
		{
			cor = addr%256;
			for(int j=0;j<(256-cor);j++,dp++)
			{
				tempData[j]=data[dp];
			}
			writePage(addr,tempData,256); // while write till end of a page
			addr=addr+(256-cor);
		}
	}
	len_b = (len+cor)%256; // check for any data remaining
	if(len_b!=0)
	{
		for(int i=0;i<len_b;i++,dp++)
		{
			tempData[i]=data[dp];
		}
		writePage(addr,tempData,len_b);
	}
}
 */
// Testning funciton for spi eeprom flash, example
/*
static void eepromTestCli()
{
	char *data_write;
	char *data_read;
	int len=0;
	int eFlage=0; // error flage used in Checking Data Corruption.
	const uint32_t addr=458752; // 64k*7=448KB
	// allocate memory
	data_read = (char*) calloc(1024, sizeof(char));
	data_write = (char*) calloc(1024, sizeof(char));
	// if memory not avaliable
	if(data_read == 0 || data_write==0)
	{
		sendStr("Error! Memory allocation Error, Stopping Test.\r\n");
		return;
	}
	strcpy(data_write,
			"Take this kiss upon the brow!\r\n"
			"And, in parting from you now,\r\n"
			"Thus much let me avow --\r\n"
			"You are not wrong, who deem\r\n"
			"That my days have been a dream\r\n"
			"Yet if hope has flown away\r\n"
			"In a night, or in a day,\r\n"
			"In a vision, or in none,\r\n"
			"Is it therefore the less gone?\r\n"
			"All that we see or seem\r\n"
			"Is but a dream within a dream.\r\n\r\n"
			"I stand amid the roar\r\n"
			"Of a surf-tormented shore,\r\n"
			"And I hold within my hand\r\n"
			"Grains of the golden sand--\r\n"
			"How few! yet how they creep\r\n"
			"Through my fingers to the deep,\r\n"
			"While I weep--while I weep!\r\n"
			"O God! can I not grasp\r\n"
			"Them with a tighter clasp?\r\n"
			"O God! can I not save\r\n"
			"One from the pitiless wave?\r\n"
			"Is all that we see or seem\r\n"
			"But a dream within a dream?\r\n");
	sendStr("Writing Data\r\n");
	writeString(addr,data_write);
	sendStr("Done\r\n");
	sendStr("Reading Data\r\n");
	readString(addr,(uint8_t*)data_read,700);
	sendStr("-----------------\r\n");
	sendStr(data_read);
	sendStr("-----------------\r\n");
	sendStr("Checking Data Corruption\r\n");
	len = strlen(data_write);
	for(int i=0;i<len;i++)
	{
		if(data_write[i]!=data_read[i]) // if any miss match
		{
			eFlage=1;
			break;
		}
	}
	if(eFlage)
	{
		sendStr("Data Corrupted\r\n");
		sendStr("Test Filed\r\n");
	}
	else
	{
		sendStr("No Data Corruption\r\n");
		sendStr("Test Success\r\n");
	}
	// free memory
	free(data_write);
	free(data_read);
}
 */
#ifdef __cplusplus
} // closing brace for extern "C"
#endif
#endif // end of include gaurd
