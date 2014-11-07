/* posix_pal.h - Defines the platform abstraction layer for a POSIX platform.
 * Copyright (C) 2014 Antoine Mercier-Linteau
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

#ifndef POSIX_PAL_H_
#define POSIX_PAL_H_

/**
 * Initializes the hardware of the platform to make it ready for executing the
 * framework.
 * */
void init(void);

#endif /* POSIX_PAL_H_ */
