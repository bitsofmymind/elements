//Interrupt Servicing Class

#define DONE 0x01
#define NEW 0x02
#define ERROR 0x04

class ISC
{
	public:
		uint8_t flags;
		
		ISC(void)
		{
			flags = 0;
		}

		virtual void isr(void) = 0;
};