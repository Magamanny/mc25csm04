#include "mc25csm04.h"
// local define
void writeEnable(const mc25csm04_st *mc25csm04_s)
{
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(WREN); // write enable
  mc25csm04_s->chipDeselect();
}
void writeDisable(const mc25csm04_st *mc25csm04_s)
{
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(WRDI); // write disable instruction
  mc25csm04_s->chipDeselect();
}
void writeStatus(const mc25csm04_st *mc25csm04_s,uint8_t st)
{
  uint8_t rsp;
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(WRSR); // status register
  mc25csm04_s->SPI_transfer(st);   // read response
  mc25csm04_s->chipDeselect();
}
uint8_t readStatus(const mc25csm04_st *mc25csm04_s)
{
  uint8_t response;
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(RDSR);            // status register
  response = mc25csm04_s->SPI_transfer(0x00); // read response
  //Serial.print("Status Register=");
  //Serial.println(response, HEX);
  mc25csm04_s->chipDeselect();
  return response;
}
void writeByte(const mc25csm04_st *mc25csm04_s,uint32_t addr, uint8_t data)
{
  uint8_t addr8;
  writeEnable(mc25csm04_s);
  mc25csm04_s->chipSelect();
  //digitalWrite(CS0, LOW);
  mc25csm04_s->SPI_transfer(WRITE); // write instruction

  addr8 = addr >> 16;
  mc25csm04_s->SPI_transfer(addr8); // msb
  addr8 = addr >> 8;
  mc25csm04_s->SPI_transfer(addr8); // mid
  addr8 = addr;
  mc25csm04_s->SPI_transfer(addr8); // lsb

  mc25csm04_s->SPI_transfer(data); // data
  mc25csm04_s->chipDeselect();
  //digitalWrite(CS0, HIGH);
  mc25csm04_s->delay_ms(5); // max write time 5ms
}
uint8_t readByte(const mc25csm04_st *mc25csm04_s,uint32_t addr)
{
  uint8_t addr8;
  uint8_t response;
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(READ); // read instruction
  // address
  addr8 = addr >> 16;
  mc25csm04_s->SPI_transfer(addr8); // msb
  addr8 = addr >> 8;
  mc25csm04_s->SPI_transfer(addr8); // mid
  addr8 = addr;
  mc25csm04_s->SPI_transfer(addr8);           // lsb
  response = mc25csm04_s->SPI_transfer(0x00); // read
  mc25csm04_s->chipDeselect();
  return response;
}
// multi byte function, native supported by ic
// read string of data for specific length
void readString(const mc25csm04_st *mc25csm04_s,uint32_t addr, uint8_t *data, uint16_t len)
{
  uint8_t addr8;
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(READ); // read instruction
  // address
  addr8 = addr >> 16;
  mc25csm04_s->SPI_transfer(addr8); // msb
  addr8 = addr >> 8;
  mc25csm04_s->SPI_transfer(addr8); // mid
  addr8 = addr;
  mc25csm04_s->SPI_transfer(addr8); // lsb
  // read the sting
  for (uint16_t i = 0; i < len; i++)
  {
    data[i] = mc25csm04_s->SPI_transfer(0x00); // read
  }
  mc25csm04_s->chipDeselect();
}
// 8 byte for data 1 for the null terminator
void writePage(const mc25csm04_st *mc25csm04_s,uint32_t addr, uint8_t data[256], uint16_t len)
{
  uint8_t addr8;
  uint8_t cor;
  writeEnable(mc25csm04_s);
  mc25csm04_s->chipSelect();
  mc25csm04_s->SPI_transfer(WRITE); // write instruction
  // address
  addr8 = addr >> 16;
  mc25csm04_s->SPI_transfer(addr8); // msb
  addr8 = addr >> 8;
  mc25csm04_s->SPI_transfer(addr8); // mid
  addr8 = addr;
  mc25csm04_s->SPI_transfer(addr8); // lsb
  // the internal address is increased automatically, the lower 3 bit
  // this means that the address must be a multiple of 8, of all 8 bytes to be writen in 1 go
  cor = addr % 256;                     // handle if addr is not a multiple of 8
  cor = 256 - cor;
  if(len > cor)
  {
    len = cor;
  }
  for (int i = 0; i < len; i++) // handle if addr is not a multiple of 8
  {
    mc25csm04_s->SPI_transfer(data[i]); // data
  }
  mc25csm04_s->chipDeselect();
  mc25csm04_s->delay_ms(10);
}
