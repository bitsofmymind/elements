/* test_request_parsing.h - Header file for the Request parsing unit tests.
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

#ifndef TEST_REQUEST_PARSING_H_
#define TEST_REQUEST_PARSING_H_

#include <core/message.h>

/** Request parsing testing function.
* @return if the test passed.
*/
bool test_request_parsing(void);

/** Test the parsing of data.
 * @param message the object that will parse the data.
 * @param data the data to parse.
 * @param expected_result the expected result of the parsing.
 * @return if there were errors.*/
bool test_parsing(Message* message, const char* data, Message::PARSER_RESULT expected_result);

#endif //TEST_REQUEST_PARSING_H_
