

uint8_t crc7( string<uint16_t> data )
{
	uint8_t reg = 0;
	
	for(uint16_t i = 0; i < data.length; i++)
	{
		for(uint8_t j = 0x80; j != 0; j >>= 1)
		{
			reg <<= 1;
			reg += data[i] & j;
			if(reg & 0x80)
			{
				reg ^= 0x09; //XORing with the CRC7 polynomial
			}
		}
	}
	//For augmentation
	for(uint8_t j = 0; j < 8; j++)
	{
		reg <<= 1;
		if(reg & 0x80)
		{
			reg ^= 0x09;
		}
	}
	return reg & 0x7F;
}

uint16_t crc16( string<uint16_t> data )
{
	uint16_t reg = 0;
	
	for(uint16_t i = 0; i < data.length; i++)
	{
		for(uint_t j = 0x80; j != 0; j >>= 1)
		{
			reg <<= 1;
			reg += data[i] & j;
			if(reg & 0x8000)
			{
				reg ^= 0x1021; //XORing with the CRC16 polynomial
			}
		}
	}
	//For augmentation
	for(uint8_t j = 0; j < 16; j++)
	{
		reg <<= 1;
		if(reg & 0x8000)
		{
			reg ^= 0x1021;
		}
	}
	return reg;
}