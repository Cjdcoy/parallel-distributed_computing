#include "logs.h"

#include <iostream>
#include <chrono>
#include <ctime>

void Logs::basic(int level)
{
	std::time_t	end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char		*date = std::ctime(&end_time);

	if (write(1, "[", 1) == -1 || write(1, date, strlen(date) -1) == -1 || write(1, "]", 1) == -1)
		throw ERROR_WRITE;
	if (write(1, "[", 1) == -1 || write(1, std::to_string(level).c_str(), std::to_string(level).size()) == -1|| write(1, "]", 1) == -1)
		throw ERROR_WRITE;
}

/**
 * @brief This function is a debugging tool.
 *
 * Only the levels equals or lower than the DEBUG_LEVEL define will be printed. This allows you to select how verbose you want your program to be.
 *
 * @param log log to display
 * @param level log priority (0 == maximum priority)
 */
void		Logs::even_odd_sort(const char *log, int level)
{
	if (SORT_DEBUG_LEVEL >= level) {
		Logs::basic(level);
		if ((write(1, "[even_odd_sort] ", strlen("[even_odd_sort] "))) == -1)
			throw ERROR_WRITE;
		if (write(DEBUG_FD, log, strlen(log)) == -1)
			throw ERROR_WRITE;
	}
}