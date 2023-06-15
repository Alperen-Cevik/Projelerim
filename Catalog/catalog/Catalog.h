#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

class CatalogEntry {
protected:
    std::vector<std::pair<std::string, std::string>> fields;

public:
    CatalogEntry() = default;

    explicit CatalogEntry(std::string entryString);

    // Getter method to access fields
    std::string getField(const std::string& fieldName) const;
    
    // Getter & setter method to access & set entry string
    void setEntryString(std::string entry);
    std::string getEntryString() const;

    std::string entry;
};

class BookCatalogEntry : public CatalogEntry {
public:
    BookCatalogEntry() = default;

    explicit BookCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString);
};

class MusicCatalogEntry : public CatalogEntry {
public:
    MusicCatalogEntry() = default;

    explicit MusicCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString);
};

class MovieCatalogEntry : public CatalogEntry {
public:
    MovieCatalogEntry() = default;

    explicit MovieCatalogEntry(const std::vector<std::string>& fieldValues, std::string entryString);
};