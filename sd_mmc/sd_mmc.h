/*
 * sd_mmc.h
 *
 *  Created on: 2010-12-14
 *      Author: antoine
 */

#ifndef SD_MMC_H_
#define SD_MMC_H_

#include <core/resource.h>

#include "ff.h"

#define CARD_DETECT_PORT PORTB
#define CARD_DETECT_DDR DDRB
#define CARD_DETECT_PINREG PINB
#define CARD_DETECT_PIN PIN1

class SDMMC: public Resource
{
	private:
		FATFS* fatfs;
		FIL* file;

	public:
		SDMMC();

	protected:
		virtual Resource* find_resource( URL* url );
		virtual Response* http_get(Request* request);
		virtual void run(void);

};

#endif /* SD_MMC_H_ */
