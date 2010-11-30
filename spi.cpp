#include "spi.h"

SPIBus::SPIBus(void)
{

}

SPIBus::instance = NULL;

SPIBus* SPIBus::get_instance(void)
{
	if(!instance)
	{
		instance = new SPIBus();
	}
	return instance;
}

int9_t SPIBus::read(string<uin16_t>* buffers, SPIDevice* device, bool async)
{
	
}

int8_t SPIBus::write(string<uin16_t> data, SPIDevice* device, bool async)
{

}

void SPIBus::suspend(void)
{
	//let current SPI byte transfer finish
	//if the device that called the interrupt is the device being 
	transfered to, finish the current transaction.
	return current_transaction;
}

void SPIBus::resume(transaction)
{
	current_transaction = transaction;
}

int8_t SPIDevice::read(string<uint16_t>* buffer, bool async)
{
	return bus->read(buffer, this, async);
}

int8_t SPIDevice::write(string<uin16_t> data, bool async)
{
	return bus->write(data, this, async);
}