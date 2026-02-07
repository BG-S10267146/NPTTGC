#include "Date.h"
#include <ctime>

std::string getCurrentDateTime()
{
    std::time_t now = std::time(nullptr);
    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &now);
#else
    timeInfo = *std::localtime(&now);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeInfo);
    return std::string(buffer);
}

int compareDates(const std::string& date1, const std::string& date2)
{
    if (date1 < date2)
        return -1;
    else if (date1 > date2)
        return 1;
    else
        return 0;
}
