#ifndef CSVHELPER_H
#define CSVHELPER_H

#include <string>
#include <fstream>
#include "Vector.h"

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

inline std::string trim(const std::string &str)
{
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

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

template <typename T>
Vector<T> buildFromFile(const std::string &filepath, T (*builder)(const Vector<std::string> &))
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

template <typename T>
void saveToFile(const std::string &filepath, const std::string &header, const Vector<T> &data, Vector<std::string> (*rowBuilder)(const T &))
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

#endif
