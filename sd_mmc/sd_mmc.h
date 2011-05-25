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
		FATFS fatfs;

	public:
		SDMMC();

	protected:
		virtual Response::status_code process( Request* request, File** return_body, const char** mime );
		virtual void run(void);

};

#endif /* SD_MMC_H_ */
