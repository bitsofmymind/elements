/* test_file_manipulation.cpp - File manipulation unit tests.
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

#include <utils/memfile.h>
#include <utils/template.h>
#include <iostream>
#include <stdlib.h>
#include "string.h"

bool read_from_file(File* file)
{
	// Test extraction.
	char* buffer = (char*)malloc(file->get_size());

	file->extract(buffer);

	free(buffer);

	// Test reads.

	buffer = (char*)malloc(file->get_size() + 100);
	char* index = buffer;

	index += file->read(buffer, 5);

	file->set_cursor(0);
	index = buffer;

	index += file->read(buffer, 10);

	index += file->read(buffer, file->get_size()); // Read past the end of the file.

	free(buffer);

	if(index - buffer > file->get_size())
	{
		return false;
	}

	return true;
}

/** File manipulation testing function.
* @return if the test passed.
*/
bool test_file_manipulation(void)
{
	bool error = false;

	std::cout << "*** testing file_manipulation..." << std::endl;

	const char* const_data = "1234567891011121314151617181920";
	char* data;
	File* file;

	//######################################################

	std::cout << "   > memfile with const char data ... ";

	file = new MemFile(const_data);

	if(read_from_file(file))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete file;

	//######################################################

	std::cout << "   > memfile with char data ... ";

	data = (char*)malloc(strlen(const_data) + 1);

	strcpy(data, const_data);

	file = new MemFile(data, false);

	if(read_from_file(file))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete file;

	//######################################################

	std::cout << "   > memfile with char data ... ";

	data = (char*)malloc(10);

	strncpy(data, const_data, 10);

	file = new MemFile(data, 10, false);

	if(read_from_file(file))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete file;

	//######################################################

	std::cout << "   > template with char data ... ";

	Template* t = new Template(new MemFile("12345 ~ 6789 ~/~ 1011~12131415 ~ 1 ~ 6 ~ 1 ~ 7181920 ~", true));

	t->add_narg((uint8_t)50);
	t->add_narg((uint16_t)UINT16_MAX);
	t->add_narg(UINT32_MAX);

	char* argument = (char*)malloc(strlen("<arg4>") + 1);
	strcpy(argument, "<arg4>");
	t->add_arg(argument);

	argument = (char*)malloc(strlen("<arg5>") + 1);
	strcpy(argument, "<arg5>");
	t->add_arg(argument);

	t->skip_argument(); // Empty argument

	argument = (char*)malloc(strlen("<arg7>") + 1);
	strcpy(argument, "<arg7>");
	t->add_arg(argument);

	if(!read_from_file(t)) // Missing an argument.
	{
		error = true;
	}

	t->set_cursor(0);

	argument = (char*)malloc(strlen("<arg8>") + 1);
	strcpy(argument, "<arg8>");
	t->add_arg(argument);

	if(!read_from_file(t))
	{
		error = true;
	}

	t->set_cursor(0);

	argument = (char*)malloc(strlen("<arg9>") + 1);
	strcpy(argument, "<arg9>");
	t->add_arg(argument);

	if(!read_from_file(t)) // Extra argument.
	{
		error = true;
	}

	delete t;

	std::cout << (error ? "(error)": "(done)") << std::endl;

	std::cout << "*** tested file manipulation" << std::endl;

	return error;
}
