#ifndef CSVHELPER_H
#define CSVHELPER_H

#include <string>
#include <fstream>
#include <sstream>
#include "Vector.h"

// Helper function to split a CSV line into fields
inline Vector<std::string> splitCSVLine(const std::string& line) {
	Vector<std::string> fields;
	std::string field;
	bool inQuotes = false;
    
	for (size_t i = 0; i < line.length(); i++) {
		char c = line[i];
        
		if (c == '"') {
			inQuotes = !inQuotes;
		} else if (c == ',' && !inQuotes) {
			fields.append(field);
			field.clear();
		} else {
			field += c;
		}
	}
	fields.append(field); // Add the last field
	return fields;
}

// Helper function to trim whitespace from strings
inline std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

// Helper function to escape CSV fields (add quotes if needed)
inline std::string escapeCSVField(const std::string& field) {
	bool needsQuotes = false;
	for (char c : field) {
		if (c == ',' || c == '"' || c == '\n') {
			needsQuotes = true;
			break;
		}
	}
    
	if (!needsQuotes) return field;
    
	std::string escaped = "\"";
	for (char c : field) {
		if (c == '"') escaped += "\"\""; // Escape quotes by doubling
		else escaped += c;
	}
	escaped += "\"";
	return escaped;
}

#endif
