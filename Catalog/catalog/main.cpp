// catalog.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "Catalog.h"

using namespace std;

std::vector<std::string> splitFields(const std::string& data) { // split field values into a vector
	std::vector<std::string> fields;
	std::string token;

	size_t start = 0;
	size_t end = 0;

	while (end != std::string::npos) {
		start = data.find("\"", end);
		if (start == std::string::npos)
			break;

		end = data.find("\"", start + 1);
		if (end == std::string::npos)
			break;

		fields.push_back(data.substr(start + 1, end - start - 1));
		end++;
	}

	return fields;
}

int main()
{
	std::ifstream file("data.txt");

	if (!file.is_open()) {
		return -1;
	}

	std::ofstream output("output.txt");

	vector<pair<string, CatalogEntry*>> entries; // map entries
	std::string mode, entry;
	std::getline(file, mode);
	
	output << "Catalog Read: " << mode << endl;

	if (mode == "book") { 
		std::string entry;
		while (getline(file, entry)) {
			try {
				auto fields = splitFields(entry);

				for (auto& entry : entries) {
					if (entry.first == fields[0])
						throw std::runtime_error("Exception: duplicate entry");
				}

				entries.emplace_back(fields[0], new BookCatalogEntry(fields, entry));
			}
			catch (const std::runtime_error& err) {
				output << err.what() << endl;
				output << entry << endl;
			}
		}
	}
	else if (mode == "music") {
		std::string entry;
		while (getline(file, entry)) {
			try {
				auto fields = splitFields(entry);

				for (auto& entry : entries) {
					if (entry.first == fields[0])
						throw std::runtime_error("Exception: duplicate entry");
				}

				entries.emplace_back(fields[0], new MusicCatalogEntry(fields, entry));
			}
			catch (const std::runtime_error& err) {
				output << err.what() << endl;
				output << entry << endl;
			}
		}
	}
	else if (mode == "movie") {
		std::string entry;
		while (getline(file, entry)) {
			try {
				auto fields = splitFields(entry);

				for (auto& entry : entries) {
					if (entry.first == fields[0])
						throw std::runtime_error("Exception: duplicate entry");
				}

				entries.emplace_back(fields[0], new MovieCatalogEntry(fields, entry));
			}
			catch (const std::runtime_error& err) {
				output << err.what() << endl;
				output << entry << endl;
			}
		}
	}

	output << entries.size() << " unique entries" << endl;

	if (entries.empty())
		return -1;

	std::ifstream commands("commands.txt");

	if (!commands.is_open()) {
		return -1;
	}

	std::string command;
	while (getline(commands, command)) {
		try {
			auto cmdType = command.substr(0, command.find(" "));
			if (cmdType == "search") { // parse command
				size_t searchStart = command.find("\"");
				size_t searchEnd = command.find("\"", searchStart + 1);
				std::string searchString = command.substr(searchStart + 1, searchEnd - searchStart - 1);

				size_t fieldStart = command.find("\"", searchEnd + 1);
				size_t fieldEnd = command.find("\"", fieldStart + 1);
				std::string fieldName = command.substr(fieldStart + 1, fieldEnd - fieldStart - 1);

				if (searchString.empty() || fieldName.empty())
					throw std::runtime_error("command is wrong");

				entries.begin()->second->getField(fieldName); // checking for validitity

				output << command << endl;

				for (auto it = entries.begin(); it != entries.end(); ++it) {
					if (it->second->getField(fieldName).find(searchString) != -1) {
						output << it->second->getEntryString() << endl;
					}
				}
			}
			else if (cmdType == "sort") {
				std::string fieldName;

				size_t start = command.find("\"");
				size_t end = command.rfind("\"");

				if (start != std::string::npos && end != std::string::npos && end > start + 1) {
					fieldName = command.substr(start + 1, end - start - 1);
				}

				if (fieldName.empty())
					throw std::runtime_error("command is wrong");

				entries.begin()->second->getField(fieldName); // checking for validitity

				output << command << endl;
				
				vector<CatalogEntry*> sortedEntries;
				for (auto it = entries.begin(); it != entries.end(); it++) {
					sortedEntries.push_back(it->second);
				}

				std::sort(sortedEntries.begin(), sortedEntries.end(), [&](const CatalogEntry* p1, const CatalogEntry* p2) {
					return p1->getField(fieldName).compare(p2->getField(fieldName)) < 0;
					}); // sort entries

				for (auto entry : sortedEntries) {
					output << entry->getEntryString() << endl;
				}
			}
			else
				throw std::runtime_error("command is wrong");
		}
		catch (const std::runtime_error& err) {
			output << "Exception: command is wrong" << endl;
			output << command << endl;
		}
	}

	// Memory cleanup
	for (auto it = entries.begin(); it != entries.end(); it++)
		delete it->second;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
