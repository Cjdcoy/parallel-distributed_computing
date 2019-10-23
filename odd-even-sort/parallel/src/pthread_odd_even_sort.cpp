//
// Created by cjdcoy on 19/09/19.
//

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <cmath>

#include "timer.h"

int                     g_state = -1;
std::atomic<int>        g_switch(0);
std::atomic<bool>        g_switched(false);
std::mutex              mtx;
std::condition_variable cv;

/**
 *
 * @param content content to display
 * @param size length of the array
 * @param n display until n
 */
static void print_data(int *content, int size, int n)
{
    for (int i = 0 ; i < size && i < n ; i++)
        std::cout << content[i] << " ";
    std::cout<<std::endl;
}

static void print_vector(std::vector<unsigned int> &arr) {
    for (auto number : arr)
        std::cout << number << std::endl;
}

static void threaded_odd_even_sort(std::vector<int> &rlist, std::pair<int, int> ids)
{
    Timer   t;
    int     start = 1;
    int     local_switch = 0;
    int     begin = std::ceil(ids.first * std::ceil(rlist.size() / (float)ids.second));
    int     local_max = begin + std::ceil((rlist.size()) / (float)ids.second);

    // std::unique_lock<std::mutex> lck(mtx);

    while (1) {
        while (g_state != (1 + start) && g_state != 3) { usleep(2); }
        if (g_state == 3)
            break;
        /** we're doing -1 so the last thread always have a pair of number to deal with */
        for (int pos = begin + start;  pos <= local_max && pos < rlist.size() ; pos +=2) {
            if (rlist[pos] > rlist[pos + 1]) {
                std::swap(rlist[pos], rlist[pos + 1]);
                g_switched = true;
            }
            local_switch+=1;
        }
        g_switch+=local_switch;
        start = start == 1 ? 0 : 1;
        local_switch = 0;
    }
}

static bool check_state()
{
    if (g_switched == false)
        return true;
    else g_switched = false;
    return false;
}

static void change_state(int state, int nb_switch)
{
    while (g_switch < nb_switch) {
        usleep(1); }
    g_state = state;
    g_switch = 0;
}

void my_pthread_launcher(std::vector<int> rlist, int nb_thread)
{
    int nb_odd = (rlist.size() - 1) / 2;
    int nb_even = rlist.size() / 2;
    Timer test;
    std::vector<std::thread> threads;

    g_state = -1;
    for (int i = 0; i < nb_thread && i < rlist.size() / 2; i++)
        threads.push_back(std::thread(threaded_odd_even_sort, std::ref(rlist), make_pair(i, nb_thread)));
    g_state = 2;
    g_switch = 0;

    print_data(&rlist[0], rlist.size(), 20);
    while (1) {
        change_state(1, nb_odd);
        if (check_state() == true) {
            change_state(3, nb_even);
            break;
        }
        change_state(2, nb_even);
    }
    print_data(&rlist[1], rlist.size(), 20);
    for (auto &thread : threads)
        thread.join();
}