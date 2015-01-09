/* sd_mmc.h - Implements a resource to interface with FAT on SD/MMC cards
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SD_MMC_H_
#define SD_MMC_H_

#include <core/resource.h>
#include "ff.h"

// Card detect pin configuration.

/// The PORT where the card detected pin is.
#define CARD_DETECT_PORT PORTB

/// The DDR where the card detected pin is.
#define CARD_DETECT_DDR DDRB

/// The PINREG where the card detected pin is.
#define CARD_DETECT_PINREG PINB

/// The pin number of the card detect pin.
#define CARD_DETECT_PIN PIN1

/// SDMMC allows usage of a FAT file system as a Resource.
class SDMMC: public Resource
{
	private:

		/// The encapsulated FAT file system structure.
		FATFS fatfs;

	public:

		/// Class constructor.
		SDMMC();

	protected:

        /// Process a request message.
        /**
         * Override of parent implementation to process file system queries
         * @param request the request to process.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         */
		virtual Response::status_code process(Request* request, Response* response);

		/// Runs the resource, allowing it to do processing.
		virtual void run(void);
};

#endif /* SD_MMC_H_ */
