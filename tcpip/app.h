/* app.h - Defines application hooks for uip
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

#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include <utils/file.h>
#include <core/request.h>

typedef struct elements_app_state
{
	Request* request;
	File* header;
	File* body;
	size_t last_sent;
} uip_tcp_appstate_t;

void elements_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL elements_appcall
#endif /* UIP_APPCALL */

#endif /* __APP_H__ */
