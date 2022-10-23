/*
 * ds18b20.cpp
 *
 *  Created on: 11 сент. 2022 г.
 *      Author: akcel
 */

#include "ds18b20.h"

__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
micros *= (SystemCoreClock / 1000000) / 9;
/* Wait till done */
while (micros--) ;
}
uint8_t ds18b20::Reset(void)
{
	uint16_t status;
	RESET_PIN; //������ �������
	DelayMicro(485);//�������� ��� ������� �� 480 �����������
	SET_PIN; //������� �������
	DelayMicro(65);//�������� ��� ������� �� 60 �����������
	status = READ_BIT_DS;//��������� �������
	DelayMicro(500);//�������� ��� ������� �� 480 �����������
	return (status ? 1 : 0);//������ ���������
}

uint8_t ds18b20::ReadBit(void)
{
	uint8_t bit = 0;
	RESET_PIN; //������ �������
	DelayMicro(2);
	SET_PIN; //������� �������
	DelayMicro(13);
	bit = (READ_BIT_DS ? 1 : 0);//��������� �������
	DelayMicro(45);
	return bit;
}

uint8_t ds18b20::ReadByte(void)
{
	uint8_t data = 0;
	for (uint8_t i = 0; i <= 7; i++)
	data += this->ReadBit() << i;
	return data;
}

void ds18b20::WriteBit(uint8_t bit)
{
	RESET_PIN;
	DelayMicro(bit ? 3 : 65);
	SET_PIN;
	DelayMicro(bit ? 65 : 3);
}

void ds18b20::WriteByte(uint8_t dt)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		this->WriteBit(dt >> i & 1);
		//Delay Protection
		DelayMicro(5);
	}
}

uint8_t ds18b20::init(skipRom mode)
{
	if(this->Reset()) return 1;
	if(mode==skipRom::kSkip)
	{
		//SKIP ROM
		this->WriteByte(0xCC);
		//WRITE SCRATCHPAD
		this->WriteByte(0x4E);
		//TH REGISTER 100 ��������
		this->WriteByte(0x64);
		//TL REGISTER - 30 ��������
		this->WriteByte(0x9E);
		//Resolution 12 bit
		this->WriteByte(resolution::k12Bit);
	}
	return 0;
}

void ds18b20::MeasureTemperCmd(skipRom mode, uint8_t DevNum)
{
	this->Reset();
	if(mode==skipRom::kSkip)
	{
		//SKIP ROM
		this->WriteByte(0xCC);
	}
	//CONVERT T
	this->WriteByte(0x44);
}

void ds18b20::ReadStratcpad(skipRom mode, uint8_t *Data, uint8_t DevNum)
{
  uint8_t i;
  this->Reset();
  if(mode==skipRom::kSkip)
  {
    //SKIP ROM
	  this->WriteByte(0xCC);
  }
  //READ SCRATCHPAD
  this->WriteByte(0xBE);
  for(i=0;i<8;i++)
  {
    Data[i] = this->ReadByte();
  }
}

uint8_t ds18b20::GetSign(uint16_t dt)
{
  //�������� 11-� ���
  if (dt&(1<<11)) return 1;
  else return 0;
}
//----------------------------------------------------------
float ds18b20::Convert(uint16_t dt)
{
  float t;
  t = (float) ((dt&0x07FF)>>4); //��������� �������� � ������� ����
  //�������� ������� �����
  t += (float)(dt&0x000F) / 16.0f;
  return t;
}


ds18b20::ds18b20() {
	// TODO Auto-generated constructor stub

}

void ds18b20::update()
{

}
