#ifndef CSVHELPER_H
#define CSVHELPER_H

#include <string>
#include <fstream>
#include <functional>
#include "Vector.h"
#include "Dictionary.h"

/// Splits a CSV line into fields, handling quoted values and escaped quotes.
inline Vector<std::string> splitCSVLine(const std::string &line)
{
	Vector<std::string> fields;
	std::string field;
	bool inQuotes = false;

	for (size_t i = 0; i < line.length(); i++)
	{
		char c = line[i];

		if (c == '"' && (field.empty() || inQuotes))
		{
			if (inQuotes && i + 1 < line.length() && line[i + 1] == '"')
			{
				field += '"';
				i++;
			}
			else
			{
				inQuotes = field.empty();
			}
		}
		else if (c == ',' && !inQuotes)
		{
			fields.append(field);
			field.clear();
		}
		else
		{
			field += c;
		}
	}
	fields.append(field);
	return fields;
}

/// Removes leading and trailing whitespace from a string.
inline std::string trim(const std::string &str)
{
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

/// Escapes a field for CSV output by quoting if necessary and doubling internal quotes.
inline std::string escapeCSVField(const std::string &field)
{
	bool needsQuotes = false;
	for (char c : field)
	{
		if (c == ',' || c == '"' || c == '\n')
		{
			needsQuotes = true;
			break;
		}
	}

	if (!needsQuotes)
		return field;

	std::string escaped = "\"";
	for (char c : field)
	{
		if (c == '"')
			escaped += "\"\"";
		else
			escaped += c;
	}
	escaped += "\"";
	return escaped;
}

/// Loads objects from a CSV file using a builder function to construct each object from CSV fields.
template <typename T>
Vector<T> buildFromFile(const std::string &filepath, std::function<T(const Vector<std::string> &)> builder)
{
	Vector<T> result;
	std::ifstream file(filepath.c_str());

	if (!file.is_open())
	{
		printf("Warning: Could not open %s. Starting with empty list.\n", filepath.c_str());
		return result;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		result.append(builder(splitCSVLine(line)));
	}

	file.close();
	return result;
}

/// Loads objects from a CSV file and builds a Dictionary using a key extraction function.
template <typename K, typename T>
Dictionary<K, T> buildDictFromFile(const std::string &filepath, std::function<T(const Vector<std::string> &)> builder, std::function<K(const T &)> keyFunc)
{
	Dictionary<K, T> result;
	std::ifstream file(filepath.c_str());

	if (!file.is_open())
	{
		printf("Warning: Could not open %s. Starting with empty dictionary.\n", filepath.c_str());
		return result;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;
		T item = builder(splitCSVLine(line));
		result.insert(keyFunc(item), item);
	}

	file.close();
	return result;
}

/// Saves objects to a CSV file using a rowBuilder function to convert each object to CSV fields.
template <typename T>
void saveToFile(const std::string &filepath, const std::string &header, const Vector<T> &data, std::function<Vector<std::string>(const T &)> rowBuilder)
{
	std::ofstream file(filepath.c_str());

	if (!file.is_open())
	{
		printf("Error: Could not open %s for writing.\n", filepath.c_str());
		return;
	}

	file << header << "\n";

	for (int i = 0; i < data.getSize(); i++)
	{
		Vector<std::string> fields = rowBuilder(data.get(i));
		for (int j = 0; j < fields.getSize(); j++)
		{
			if (j > 0)
				file << ",";
			file << escapeCSVField(fields.get(j));
		}
		file << "\n";
	}

	file.close();
}

/// Saves Dictionary values to a CSV file a rowBuilder function.
template <typename K, typename V>
void saveToFile(const std::string &filepath, const std::string &header, const Dictionary<K, V> &data, std::function<Vector<std::string>(const V &)> rowBuilder)
{
	std::ofstream file(filepath.c_str());

	if (!file.is_open())
	{
		printf("Error: Could not open %s for writing.\n", filepath.c_str());
		return;
	}

	file << header << "\n";

	data.forEach([&](const K &key, const V &value)
				 {
		Vector<std::string> fields = rowBuilder(value);
		for (int j = 0; j < fields.getSize(); j++)
		{
			if (j > 0)
				file << ",";
			file << escapeCSVField(fields.get(j));
		}
		file << "\n"; });

	file.close();
}

#endif
