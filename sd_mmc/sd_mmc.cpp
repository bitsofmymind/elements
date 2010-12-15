/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2010           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/


#include <avr/io.h>
#include "diskio.h"
#include <pal/pal.h>

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Port Controls  (Platform dependent) */
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define CS_PIN PIN2
#define MISO_PIN PIN4
#define MOSI_PIN PIN3
#define SCK_PIN PIN5

#define CS_LOW()	SPI_PORT &= ~_BV(CS_PIN)			/* MMC CS = L */
#define	CS_HIGH()	SPI_PORT |= _BV(CS_PIN)			/* MMC CS = H */
#define SOCKWP		(PINB & 0x20)		/* Write protected. yes:true, no:false, default:false */
#define SOCKINS		(!(PINB & 0x10))	/* Card detected.   yes:true, no:false, default:true */

//#define	FCLK_SLOW()	SPCR = 0x52		/* Set slow clock (100k-400k) */
//#define	FCLK_FAST()	SPCR = 0x50		/* Set fast clock (depends on the CSD) */


/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

static volatile
DSTATUS Stat = STA_NOINIT | STA_NODISK;	/* Disk status */

static
BYTE CardType;			/* Card type flags */

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

#define xmit_spi(dat) 	SPDR=(dat); loop_until_bit_is_set(SPSR,SPIF)



/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi (void)
{
	SPDR = 0xFF;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)	SPDR=0xFF; loop_until_bit_is_set(SPSR,SPIF); *(dst)=SPDR



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (void)	/* 1:OK, 0:Timeout */
{
	uint64_t timer = get_uptime() + 500;	/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do
		if (rcvr_spi() == 0xFF) return 1;
	while (get_uptime() < timer);

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
	CS_HIGH();
	rcvr_spi();
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
int select (void)	/* 1:Successful, 0:Timeout */
{
	CS_LOW();
	if (!wait_ready()) {
		deselect();
		return 0;
	}
	return 1;
}



/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
int power_status(void)		/* Socket power state: 0=off, 1=on */
{
	return 1;//(PORTE & 0x80) ? 0 : 1;
}


static
void power_on (void)
{
	//PORTE &= ~0x80;				/* Socket power on */
	//for (Timer1 = 2; Timer1; );	/* Wait for 20ms */
	uint64_t timer = get_uptime() + 20;
	while(get_uptime() < timer);
	//PORTB = 0b10110101;			/* Enable drivers */
	//DDRB  = 0b11000111;

	SPI_DDR |= (_BV(SCK_PIN) | _BV(MOSI_PIN) | _BV(CS_PIN));
	SPI_DDR &= ~(_BV(MISO_PIN));

	CS_HIGH();

	SPCR |= _BV(SPE) + _BV(MSTR) + _BV(SPR1) + _BV(SPR0);			/* Enable SPI function in mode 0 */
	SPSR &= ~(_BV(SPI2X));			/* SPI 2x mode */
}


static
void power_off (void)
{
	SPCR = 0;				/* Disable SPI function */
	//DDRB  = 0b11000000;		/* Disable drivers */
	//PORTB = 0b10110000;

	SPI_DDR &= ~(_BV(SCK_PIN) + _BV(MOSI_PIN) + _BV(CS_PIN) + _BV(MISO_PIN));

	//PORTE |=  0x80;			/* Socket power off */
	Stat |= STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;

	uint64_t timer = get_uptime() + 200;

	do {							/* Wait for data packet in timeout of 200ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && timer > get_uptime());
	if(token != 0xFE) return 0;		/* If not valid data token, retutn with error */

	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (		/* Returns R1 resp (bit7==1:Send failed) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select()) return 0xFF;
	/* Send command packet */
	xmit_spi(0x40 | cmd);				/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE n, cmd, ty, ocr[4];


	if (drv) return STA_NOINIT;			/* Supports only single drive */
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	power_on();							/* Force socket power on */
	//FCLK_SLOW();
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

	select();

	ty = 0;
	if (send_cmd(CMD0, 0) == 1)
	{	/* Enter Idle state */
		//Timer1 = 100;
		/* Initialization timeout of 1000 msec */

		uint64_t timer = get_uptime() + 1000;
		if (send_cmd(CMD8, 0x1AA) == 1)
		  {	/* SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA)
			  {				/* The card can work at vdd range of 2.7-3.6V */
				while (timer > get_uptime() && send_cmd(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (timer > get_uptime() && send_cmd(CMD58, 0) == 0)
				  {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		}
		else
		{/* SDv1 or MMCv3 */

			if (send_cmd(ACMD41, 0) <= 1)
			{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			}
			else
			{
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			while (timer > get_uptime() && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			if (!(timer > get_uptime())|| send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
			{
				ty = 0;
			}
		}
	}
	CardType = ty;
	deselect();

	if (ty) {			/* Initialization succeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
		//FCLK_FAST();
	} else {			/* Initialization failed */
		power_off();
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block read */
		if (send_cmd(CMD17, sector) == 0 )
		{
			/* READ_SINGLE_BLOCK */
			if( rcvr_datablock(buff, 512))
			{
				count = 0;
			}
		}
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

/*
 * sd_mmc.cpp
 *
 *  Created on: 2010-12-14
 *      Author: antoine
 */

#include "sd_mmc.h"
#include <stdlib.h>

SDMMC::SDMMC(void): Resource()
{
	CARD_DETECT_DDR &= ~_BV(CARD_DETECT_PIN);
	CARD_DETECT_PORT |= _BV(CARD_DETECT_PIN); //Turns on the pull-up for CARD_DETECT_PIN
	run(); /*run() is in charge of periodically checking for disk presence and
	initialization so calling it will take care of booting our disk.*/
}

Response* SDMMC::http_get(Request* request)
{

}

void SDMMC::run(void)
{

	if(CARD_DETECT_PINREG & CARD_DETECT_PIN) //If there is a disk in the socket
	{
		if(Stat & (STA_NOINIT)) //If the disk in the socket has not been initialized
		{
			if(Stat & STA_NODISK) //If there was previously no disk in the socket
			{
				/*Do nothing. With the setting of STA_NODISK below and the scheduling
				 * of the next run in 100ms, this will effectively provide a debouncing
				 * delay.*/
			}
			else if(!disk_initialize(0)) //If there is a disk and it has been debounced
			{
				/*STA_NOINIT was cleared in disk_initialized because it succeeded*/
				fatfs = (FATFS*)malloc(sizeof(FATFS)); //Allocates FATFS struct
				if(!fatfs)
				{
					//allocation of fatfs failed!
				}
				else
				{
					f_mount(0, fatfs); //Mounts the disk
				}
			}
			/*Initialization will be attempted again in 100ms if it failed.*/
			Stat &= ~STA_NODISK; //Indicate the presence of disk in the socket

		}
		schedule(10);
	}
	else //If there is no disk in the socket
	{
		if(!fatfs) //If fatfs was previously allocated
		{
			free(fatfs);
			f_mount(0, NULL); //unmounts the disk
			power_off(); //Power it off
		}
		Stat = STA_NODISK + STA_NOINIT; /*Indicate there is no disk and it has not been
		initialized*/
	}

	schedule(100);
}
