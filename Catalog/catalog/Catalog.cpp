#include "Catalog.h"

CatalogEntry::CatalogEntry(std::string entryString) : entry(entryString) {}

std::string CatalogEntry::getField(const std::string& fieldName) const {
	// check if field exists

	for (auto& field : fields) {
		if (field.first == fieldName)
			return field.second;
	}

	throw std::runtime_error("Field not found: " + fieldName);
}

void CatalogEntry::setEntryString(std::string entry) {
	this->entry = entry;
}

std::string CatalogEntry::getEntryString() const {
	return entry;
}

BookCatalogEntry::BookCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString) : CatalogEntry(entryString) {
	std::vector<std::string> fieldNames = { "title", "authors", "year", "tags" };

	if (fieldValues.size() != fieldNames.size()) { // check if complete fields
		throw std::runtime_error("Exception: missing field");
	}

	fields = {};

	for (std::size_t i = 0; i < fieldNames.size(); ++i) {
		fields.emplace_back(fieldNames[i], fieldValues[i]);
	}
}


MusicCatalogEntry::MusicCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString) : CatalogEntry(entryString) {
	std::vector<std::string> fieldNames = { "title", "artists", "year", "genre" };

	if (fieldValues.size() != fieldNames.size()) { // check if complete fields
		throw std::runtime_error("Exception: missing field");
	}

	fields = {};

	for (std::size_t i = 0; i < fieldNames.size(); ++i) {
		fields.emplace_back(fieldNames[i], fieldValues[i]);
	}
}


MovieCatalogEntry::MovieCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString) : CatalogEntry(entryString) {
	std::vector<std::string> fieldNames = { "title", "director", "year", "genre", "starring" };

	if (fieldValues.size() != fieldNames.size()) { // check if complete fields
		throw std::runtime_error("Exception: missing field");
	}

	fields = {};

	for (std::size_t i = 0; i < fieldNames.size(); ++i) {
		fields.emplace_back(fieldNames[i], fieldValues[i]);
	}
}