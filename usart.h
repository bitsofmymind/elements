
class Uart: public File, public ISC
{
	protected:
	#define RECEIVE_BUFFER_SIZE 16	

		Buffer* transmit_buffer;
		Buffer* receive_buffer;	

	public:
		Uart();
		~Uart(void);
		
		virtual int8_t read(string<uint16_t>*, bool async);
		virtual int8_t write(string<uint16_t>*, bool async);
		virtual int8_t open(Resource* owner);
		virtual int8_t close(void);		

		
		virtual void isr(void);


}