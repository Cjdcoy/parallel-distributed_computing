#ifndef MY_TIMER_H_
#define MY_TIMER_H_

#include <iostream>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

/** \addtogroup Utils
 * 	\brief Module containing tools that may be helpfull to any other class
 *
 * 	\author Fabien Labarbe-Desvignes
 * 	\date May 2019
 *
 *
 *  @{
 */

/**
 * \class Timer
 */
class Timer {
public:
	/**
	 * @brief default constructor, does nothing
	 */
    Timer() {};
    void		start(const string &id);
    int			safe_start(const string &id);
    void		stop(const string &id);
private:
    int			find_start(const string &id);
    int			find_stop(const string &id);
public:
    void		print_elapsed_time(const string &start_id, const string &stop_id, bool reset_start);
    int64_t		get_elapsed_time(const string &start_id, const string &stop_id, bool reset_start);
    void		stop_print(const string &start_id, const string &stop_id, bool reset_start);
    void		remove_stop(const string &id);
    void		remove_start(const string &id);

    int         get_blocks_received() const { return _blocks_received; }
    void        incr_blocks_received() { _blocks_received++; }
    void        reset_blocks_received() { _blocks_received = 0; }
private:

	int	_blocks_received = 0; /** A block iterator */
	vector<string>								_start_ids;	/** Contains the timers identifications for starters. */
    vector<string>								_stop_ids;	/** Contains the timers indentifications for stops. */
    vector<time_point<high_resolution_clock>>   _starts;	/** Contains the starting times. */
    vector<time_point<high_resolution_clock>>   _stops;		/**	Contains the stopping times. */
};
/** @}*/

#endif //MY_TIMER_H_
