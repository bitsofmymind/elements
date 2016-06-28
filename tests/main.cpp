/* main.cpp - Main file for the unit tests.
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

// INCLUDES
#include <pal/pal.h>
#include <iostream>
#include <stdlib.h>
#include <test_request.h>
#include <test_response.h>
#include "string.h"
#include "test_data_structures.h"
#include "test_file_manipulation.h"
#include "test_message_passing.h"

// Platform function implementations.
void processing_wake(){}
void processing_sleep(uint64_t time){}
void Debug::print_char(char c){ std::cout << c; }
void Debug::println(){ std::cout << std::endl; }
void heart_beat(){}

/// The program's entry point.
int main()
{
	bool error = false;

	std::cout << "Starting unit tests..." << std::endl;

	error |= test_data_structures();
	error |= test_request();
	error |= test_response();
	error |= test_file_manipulation();
	error |= test_message_passing();

	std::cout << (error ? "Failed": "Done") << std::endl;

	return error ? 1: 0;
}
