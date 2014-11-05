// SVN FILE: $Id: $
/**
 * @file configuration.h
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Header file for the MIME namespace that declares MIME types.
 */

#ifndef MIME_H_
#define MIME_H_

/// MIME is a namespace for MIME types.
/**
 * MIME types a grouped inside a namespace to they can be logically
 * manipulated.
 */
namespace MIME
{
	extern const char TEXT_HTML[];
	extern const char APPLICATION_JSON[];
	extern const char APPLICATION_OCTET_STREAM[];
	//extern const char* APPLICATION_XHTML_XML;
}
#endif /* MIME_H_ */
