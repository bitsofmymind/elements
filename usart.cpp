
Uart::Uart(void):
	owner(NULL),
	ISC()
{

}

~Uart::Uart(void)
{

}

int8_t Uart::read(string<uint16_t>* buffer, bool async)
{
	flags &= !NEW;	

	//deactivate uart interrupt

	//reactivate uart interrupt
}

int8_t Uart::write(Buffer<uint16_t>* buffer, bool async)
{
	if(!flags|DONE)
	{
		return 1;
	}
	transmit_buffer = buffer;
	transmit_buffer->read(UART_send_reg);
	return 0;
}

int8_t Uart::open(Resource* owner)
{
	if(owner){ return 1; }
	receive_buffer = new Buffer(RECEIVE_BUFFER_SIZE);
	flags |= DONE;
	owner = owner;
	return 0;
}

int8_t Uart::close(void)
{
	//Deactivate interrupt
	delete receive_buffer;
	return 0;
}

void Uart::isr(void)
{
	uint8_t byte;	
	
	if(!owner)
	{
		return;
	}
	
	//deactivate uart interrupts
	if(//byte received)
	{
		receive_buffer->write(byte_received);
		flags |= NEW;
		owner->wake();		
	}
	if(//byte sent)
	{
		flags &= !DONE;
		if(transmit_buffer->read(UART_send_reg))
		{
			busy_transmitting = false;
			if(transmit_buffer->free_if_consumed)
			{
				delete transmit_buffer;
			}
			flags |= DONE;
			owner->wake();
		}
	}
	if(//error)
	{
		flags |= ERROR;
	}
	//reactivate uart interrupts
}