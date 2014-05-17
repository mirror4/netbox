#pragma once

typedef struct CRC16TIstate_st
{
	unsigned short crc;
}CRC16TI_CTX;

int CRC16TI_Init(CRC16TI_CTX *c)
{
	c->crc = 0xFFFF;
	return 0;
}

int CRC16TI_Update(CRC16TI_CTX *c, const unsigned char *p, size_t len)
{
	unsigned short crc = c->crc;
	for (size_t i = 0; i < len; i++) {
		unsigned char data = *p++;
		for (int j = 0; j < 8; j++) {
			if (((crc & 0x8000) >> 8) ^ (data & 0x80))
				crc = (crc << 1) ^ 0x8005;
			else
				crc <<= 1;
			data <<= 1;
		}
	}
	c->crc = crc;
	return 0;
}

int CRC16TI_Final(unsigned char *md, CRC16TI_CTX *c)
{
	*(unsigned short *)md = c->crc;
	return 0;
}

