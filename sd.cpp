
#include "sd.h"

Fat32SDMMC::Fat32SDMMC(File* device):
device(device),
Resource()
{
	uint8_t response;
	send_command(SEND_OP_COND, 0, &response, NULL, true);
	/*There is a difference between SD and MMC cards in the 
	startup procedure, check http://www.maxim-ic.com/app-notes/index.mvp/id/3969 for more info.*/
	busy_wait(5); /*Wait to let the card, maybe this could be implemented as a normal resource sleep
	 and a mechanism to prevent any transaction during that time?*/
}

int8_t Fat32SDMMC::send_command(uint8_t cmd, uint32_t args, uint8_t* response, string<uint16_t>* buffer, bool is_read)
{
	int8_t result = 0;
	uint8_t response;
	SDMMCCommand command;

	command.code = cmd;
	command.argument = args;
	command.crc7 = crc_7({&command, 5});
	command.reserved = 1;
	result = device.write({&command, 6}, 100);
	
	result = device->read({&response, 1}, 100);
	if(!buffer)
	{
		//we are not waiting for data.
		return result;
	}
	
	string<uint16_t> crc16_buffer;
	uint16_t crc16;
	crc16_buffer.length = sizeof(uint16_t);
	crc16_buffer.data = &crc16;
	
	if(is_read)
	{	

		result = device->read(buffer, 100);
		result = device->read(&crc16_buffer, 0);
		if(crc16( *buffer ) != crc16)
		{
			//Transmission error
			return 1;
		}
		return result;
	}
	else
	{	
		crc16 = crc16( *buffer );
		result = device->write(buffer, 100);
		return device->write(&crc16_buffer, 0);
	}
}