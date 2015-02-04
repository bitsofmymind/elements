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
#include "string.h"
#include "test_request_parsing.h"
#include "test_file_manipulation.h"

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

	error |= test_request_parsing();
	error |= test_file_manipulation();

	std::cout << (error ? "Failed": "Done") << std::endl;

	return error ? 1: 0;
}