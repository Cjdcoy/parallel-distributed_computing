#ifndef MY_LOGS_H_
#define MY_LOGS_H_

#include "errors.h"
#include <iostream>
#include <unistd.h>
#include <cstring>


#define SORT_DEBUG_LEVEL		1

#define DEBUG_FD 1

/**
 * @namespace Logs
 *
 * @brief this namespace contains tools for logs
 */
namespace Logs {
	void		basic(int);
	void		even_odd_sort(const char *log, int level);
};

#endif //MY_LOGS_H_
