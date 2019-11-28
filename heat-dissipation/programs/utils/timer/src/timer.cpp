#include "timer.h"

/**
 * @param id - identifier that will be applied to the timer
 *
 * @brief Add the identifier to _start_ids the time into _starts
 */
void        Timer::start(const string &id)
{
    time_point<high_resolution_clock>   start = high_resolution_clock::now();
    _start_ids.emplace_back(id);
    _starts.emplace_back(start);
}


/**
 * @param id - identifier that will be applied to the timer
 *
 * @brief Verify that the identifier is UNIQUE then starts a timer, add the identifier to _start_ids the time into _starts.
 */
int        Timer::safe_start(const string &id)
{
    for (auto &x : _start_ids) {
        if (id.compare(x) == 0)
            return 1;
    }

    time_point<high_resolution_clock>   start = high_resolution_clock::now();
    _start_ids.emplace_back(id);
    _starts.emplace_back(start);
    return 0;
}

/**
 * @param id - identifier that will be applied to the end timer.
 *
 * @brief Add the identifier to _stop_ids the time into _stops.
 */
void        Timer::stop(const string &id)
{
    time_point<high_resolution_clock>   stop = high_resolution_clock::now();
    _stop_ids.emplace_back(id);
    _stops.emplace_back(stop);
}


/**
 * @param id - identifier that we search.
 * @return position of the identifier in the vector
 * @return -1 if not found
 *
 */

int         Timer::find_start(const string &id)
{
    for (size_t x = 0 ; x < _start_ids.size() ; x++ ) {
        if (id.compare(_start_ids[x]) == 0)
            return x;
    }
    return -1;
}

/**
 *
 * @param id - identifier that we search.
 * @return position of the identifier in the vector
 * @return -1 if not found
 */
int         Timer::find_stop(const string &id)
{
    for (size_t x = 0 ; x < _start_ids.size() ; x++ ) {
        if (id.compare(_stop_ids[x]) == 0)
            return x;
    }
    return -1;
}

/**
 *
 * @param start_id - identifier of the stop timer
 * @param stop_id  - identifier of the stop timer
 * @param remove_start - boolean that decides if we remove the stop timer
 *
 * @brief print the time elapsed between the start and the stop ids (in micro and nano seconds)
 */
void        Timer::print_elapsed_time(const string &start_id, const string &stop_id, bool remove_start)
{
    int     pos_start = find_start(start_id);
    int     pos_stop = find_stop(stop_id);

    if (pos_start == -1 || pos_stop == -1)
        return;

    auto    duration = duration_cast<microseconds>(_stops[pos_stop] - _starts[pos_start]);

    if (remove_start) {
        _start_ids.erase(_start_ids.begin() + pos_start);
        _starts.erase(_starts.begin() + pos_start);
    }

    _stop_ids.erase(_stop_ids.begin() + pos_stop);
    _stops.erase(_stops.begin() + pos_stop);
    cout << duration.count() << " us (" << duration.count() / 1000 << " ms)" << endl;
}

/**
 *
 * @param start_id - identifier of the stop timer
 * @param stop_id  - identifier of the stop timer
 * @param remove_start - boolean that decides if we remove the stop timer
 * @return elapsed time in nanoseconds
 */
int64_t        Timer::get_elapsed_time(const string &start_id, const string &stop_id, bool remove_start)
{
    stop(stop_id);
    int     pos_start = find_start(start_id);
    int     pos_stop = find_stop(stop_id);

    if (pos_start == -1 || pos_stop == -1)
        return -1;

    auto    duration = duration_cast<microseconds>(_stops[pos_stop] - _starts[pos_start]);

    if (remove_start) {
        _start_ids.erase(_start_ids.begin() + pos_start);
        _starts.erase(_starts.begin() + pos_start);
    }

    _stop_ids.erase(_stop_ids.begin() + pos_stop);
    _stops.erase(_stops.begin() + pos_stop);
    return duration.count();
}

/**
 *
 * @param start_id - identifier of the stop timer
 * @param stop_id  - identifier of the stop timer
 * @param remove_start - boolean that decides if we remove the stop timer
 *
 * @brief Prints the time elapsed between start and now.
 */
void        Timer::stop_print(const string &start_id, const string &stop_id, bool remove_start)
{
    stop(stop_id);
    print_elapsed_time(start_id, stop_id, remove_start);
}

/**
 *
 * @param id - identifier of the stop timer
 *
 * @brief remove a stop timer
 */
void        Timer::remove_stop(const string &id)
{
    int     pos_stop = find_stop(id);

    if (pos_stop == -1)
        return;
    _stop_ids.erase(_stop_ids.begin() + pos_stop);
    _stops.erase(_stops.begin() + pos_stop);
}

/**
 *
 * @param id - identifier of the stop timer
 *
 * @brief remove a stop timer
 */
void        Timer::remove_start(const string &id)
{
    int     pos_start = find_start(id);

    if (pos_start == -1)
        return;

    _start_ids.erase(_start_ids.begin() + pos_start);
    _starts.erase(_starts.begin() + pos_start);
}