

http://www.maxim-ic.com/app-notes/index.mvp/id/3969

#include "crc.h"

class Fat32SDMMC: Resource
{
		
		//SD/MMC commands are 6 bit so they are systematically ORed with 0x40
		#define GO_IDLE_STATE 0x00|0x40 //Resets the card
		#define SEND_CSD 0x09|0x40 //Sends card-specific data
		#define SEND_CID 0x0a|0x40 //Sends card identification
		#define READ_SINGLE_BLOCK 0x11|0x40 //Reads a block the byte address provided as argument
		#define WRITE_BLOCK 0x18|0x40 //Writes a block at the byte address provided as argument
		#define	APP_CMD 0x37|0x40 //Prefix for application commands
		#define CRC_ON_OFF 0x3b|0x40 //Argument sets CRC ON (1) or OFF (0)
		#define SEND_OP_COND 0x29|0x40 //Starts card initialization

	protected:
		File* device;
	public:
		typedef struct SDMMCCommand
		{
			uint8_t code;
			uint32_t argument;
			uint8_ crc7: 7;
			uint8_t reserved: 1;
		};

		typedef struct SDMMCResponse
		{
			const uint8_t reserved: 1;
			uint8_t parameter_error: 1;
			uint8_t address_error: 1;
			uint8_t erase_sequence_error: 1;
			uint8_t command_crc_error: 1;
			uint8_t illegal_command: 1;
			uint8_t erase_reset: 1;
			uint8_t idle_state: 1;			
		};

		typedef struct card_specific_data
		{
			const uint8_t reserved0;
			uint8_t TAAC;
			uint8_t NSAC;
			uint8_t TRAN_SPEED;
			uint16_t CCC: 12;
			uint8_t READ_BL_LEN: 4;
			uint8_t READ_BL_PARTIAL: 1;
			uint8_t WRITE_BLK_MISALIGN: 1;
			uint8_t READ_BLK_MISALIGN: 1;
			uint8_t DSR_IMP: 1;
			const uint8_t reserved1: 2;
			uint16_t C_SIZE: 12;
			uint8_t VDD_R_CURR_MIN: 3;
			uint8_t VDD_R_CURR_MAX: 3;
			uint8_t VDD_W_CURR_MIN: 3;
			uint8_t VDD_W_CURR_MAX: 3;
			uint8_t C_SIZE_MULT: 3;
			uint8_t ERASE_BLK_EN: 1;
			uint8_t SECTOR_SIZE: 7;
			uint8_t WP_GRP_SIZE: 7;
			uint8_t WP_GRP_ENABLE: 1;
			const uint8_t reserved2: 2;
			uint8_t R2W_FACTOR: 3;
			uint8_t WRITE_BL_LEN: 4;
			const uint8_t reserved3: 6;
			uint8_t FILE_FORMAT_GRP: 1;
			uint8_t COPY: 1;
			uint8_t PERM_WRITE_PROTECT: 1;
			uint8_t TMP_WRITE_PROTECTED: 1;
			uint8_t FILE_FORMAT: 2;
			const uint8_t reserved3: 2;
			uint8_t CRC-7: 7;
			const uint8_t reserved4: 1; //should be 1
		};

		typedef struct card_identification
		{
			uint8_t MID; //Manufacturer ID (BIN)
			uint16_t OID; //OEM/Applicaiton ID (ASCII)
			uint64_t PNM: 40; //Product name (ASCII)
			uint8_t PRV; //Product revision (BCD)
			uint32_t PSN; //Product serial Number (BIN)
			uint8_t reserved0: 4;
			uint16_t MDT; //Manufacturer date code (BCD)
			uint8_t CRC-7: 7;
			const uint8_t reserved1: 1; //should be one.
		};

		#define START_TOKEN 0xFE
		typedef struct error_token
		{
			uint8_t not_used: 3;
			uint8_t CARD_IS_LOCKED: 1;
			uint8_t OUT_OF_RANGE: 1;
			uint8_t MEDIA_ECC_FAILED: 1;
			uint8_t CARD_CONTROLLER_ERROR: 1;
			uint8_t ERROR: 1;
		};

		typedef struct data_response
		{
			#define DATA_ACCEPTED 0b010
			#define TRANSMISSION_CRC_ERROR 0b101
			#define DATA_WRITE_ERROR 0b110

			uint8_t not_used0: 4; //Should be set to 0
			uint8_t response: 4;
			uint8_t not_used1: 1; //Should be set to 1
		};

		Fat32SDMMC( File* device );
		~Fat32SDMMC();	

		
		int8_t send_command(uint8_t cmd, uint32_t args, uint8_t* response, string<uint16_t>* buffer, bool is_read);
		

};
