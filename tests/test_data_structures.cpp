/* test_data_structures.cpp - Source file for the data structures unit tests.
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

#include <utils/utils.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include "string.h"

bool test_data_structures(void)
{
	bool error = false;

	std::cout << "*** testing data structures..." << std::endl;

	List<const char*>* list;

	//######################################################

	std::cout << "   > empty list creation and deletion ... ";

	list = new List<const char*>();

	if(list)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete list;

	//######################################################

	std::cout << "   > filling up a list ... ";

	list = new List<const char*>();

	while(list->append("x") != STRUCTURE_FULL);

	delete list;

	std::cout << "(done)" << std::endl;

	//######################################################

	std::cout << "   > growing an shrinking a list ... ";

	list = new List<const char*>();

	for(uint16_t i = 0; i < 100; i++)
	{
		list->append("x");
	}

	for(uint16_t i = 0; i < 80; i++)
	{
		list->remove(0);
	}

	delete list;

	std::cout << "(done)" << std::endl;

	//######################################################

	std::cout << "   > appending items to the list ... ";

	list = new List<const char*>();
	bool done = true;

	for(uint16_t i = 0; i < 100; i++)
	{
		char* item = (char*)calloc(4 + 4, 1);

		strcat(item, "item");
		sprintf(item + 3, "%d", i);

		if(list->append(item))
		{
			done = false;
		}
	}

	if(done)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > inserting item at the beginning of the list ... ";

	list->insert("test_item", 0);

	if(strcmp(list->remove(0), "test_item") == 0)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > inserting items at the same position ... ";

	list->insert("test_item1", 20);
	list->insert("test_item2", 20);

	if(strcmp((*list)[20], "test_item2") == 0 && strcmp((*list)[21], "test_item1") == 0)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	list->remove(20);
	list->remove(20);

	//######################################################

	std::cout << "   > inserting item at illegal position ... ";

	if(list->insert("test_item", 120) != 0)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > removing specific item ... ";

	const char * string = "test_item";

	list->insert(string, 40);

	if(strcmp(list->remove_item(string), string) == 0)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > freeing all items ... ";

	list->free_all();

	if(list->get_item_count() == 0)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete list;

	Dictionary<const char*>* dictionary;

	//######################################################

	std::cout << "   > empty dictionary creation and deletion ... ";

	dictionary = new Dictionary<const char*>();

	if(list)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete dictionary;

	//######################################################

	std::cout << "   > adding items to the dictionary ... ";

	dictionary = new Dictionary<const char*>();
	done = true;

	for(uint16_t i = 0; i < 100; i++)
	{
		char* key = (char*)calloc(3 + 4, 1);

		strcat(key, "key");
		sprintf(key + 3, "%d", i);

		char* value = (char*)calloc(5 + 4, 1);

		strcat(value, "value");
		sprintf(value + 5, "%d", i);

		if(dictionary->add(key, value))
		{
			done = false;
		}
	}

	if(done)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding an item by key from the dictionary ... ";

	if(dictionary->find("key45"))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding a non existing item by key from the dictionary ... ";

	if(!dictionary->find("key0000"))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding an item by value from the dictionary ... ";

	if(dictionary->find_val((*dictionary)[60]->value))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding a non existing item by value from the dictionary ... ";

	if(!dictionary->find_val("value0000"))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	//######################################################

	std::cout << "   > removing an item from the dictionary ... ";

	const char* value = dictionary->remove("key45", true);

	if(value && !dictionary->find("key45"))
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	free((void*)value);

	//######################################################

	std::cout << "   > removing a non-existing item from the dictionary ... ";

	value = dictionary->remove("key00000", true);

	if(!value)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	dictionary->free_all(true);

	delete dictionary;

	return error;
}
