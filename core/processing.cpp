/* processing.cpp - Source file for the Processing class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

//INCLUDES
#include <pal/pal.h>
#include "processing.h"

#if PROCESSING_AS_AUTHORITY
Processing::Processing(Resource* bound): Authority(),
#else
Processing::Processing(Resource* bound): Resource(),
#endif
		_bound(bound),
		_current(this) //A Processing object starts by running itself.
#if HEARTBEAT
		,_heartbeat(1000)
#endif
{}

