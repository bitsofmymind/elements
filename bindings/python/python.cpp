/* python.cpp - Main file for the python bindings.
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
#include <Python.h>
#include <stdio.h>
#include <iostream>

// Platform function implementations.

void init() {  PyRun_SimpleString("init()"); }

void terminate() { PyRun_SimpleString("terminate()"); }

void processing_wake(){ PyRun_SimpleString("processing_wake()"); }

void processing_sleep(uptime_t time)
{
	char function[256];
	/* Call processing_sleep defined in python with time converted
	 * into a python long integer. */
	sprintf(function, "processing_sleep(%uL)", time);
	PyRun_SimpleString(function);
}

void heart_beat(){ PyRun_SimpleString("heart_beat()"); }

void Debug::print_char(char character)
{
	std::cout << character;
}

void Debug::println()
{
	std::cout << std::endl;
}

