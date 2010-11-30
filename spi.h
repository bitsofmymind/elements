
#include <utils>

class SPIBus: File
{
	private:
		SPIBus* instance;
		typedef transaction
		{
			string<uint16_t> data;
			SPIDevice* device;
			bool is_read;
		};
		//Queue<transaction> transactions;
		transaction current_transaction;
		SPIBus(void);

	public:
		SPIBus* get_instance(void);
		
		//only implement synchronous read and write for now
		virtual int8_t read( string<uint16_t>*, SPIDevice*, bool async );
		virtual int8_t write( string<uint16_t>, SPIDevice*, bool async );
		/*transaction and resume are called when an ISR routine calls
		read of write while an operation is currently ongoing on a 
		different device.*/
		current_transaction suspend(void);
		void resume(transaction);
};

class SPIDevice: File
{
	private:
		SPIBus* bus;		
		PORT device_pin
	public:
		SPIDevice( PORT device_pin, PORT interrupt_pin  );
		virtual int8_t read( string<uint16_t>*, bool async);
		virtual int8_t read( string<uint16_t>*, uint16_t timeout);
		virtual int8_t write( string<uint16_t>, bool async );
};