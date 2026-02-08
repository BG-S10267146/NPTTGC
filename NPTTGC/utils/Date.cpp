#include "Date.h"
#include <ctime>
#include <cstring>

namespace DateHelper
{
    std::string getCurrentDateTime()
    {
        std::time_t now = std::time(nullptr);
        if (now == static_cast<std::time_t>(-1))
        {
            return "1970-01-01 00:00"; // Fallback for time() failure
        }

        std::tm timeInfo;
#ifdef _WIN32
        if (localtime_s(&timeInfo, &now) != 0)
        {
            return "1970-01-01 00:00"; // Fallback for localtime_s failure
        }
#else
        std::tm *timePtr = std::localtime(&now);
        if (timePtr == nullptr)
        {
            return "1970-01-01 00:00"; // Fallback for localtime failure
        }
        timeInfo = *timePtr;
#endif

        char buffer[20];
        size_t result = std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeInfo);
        if (result == 0)
        {
            return "1970-01-01 00:00"; // Fallback for strftime failure
        }

        return std::string(buffer);
    }

    int compareDates(const std::string &date1, const std::string &date2)
    {
        // Lexicographic comparison works for YYYY-MM-DD format
        if (date1 < date2)
        {
            return -1;
        }
        else if (date1 > date2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
