//
// Created by cjdcoy on 19/09/19.
//

#ifndef EVEN_ODD_SORT_PTHREAD_ODD_EVEN_SORT_H
#define EVEN_ODD_SORT_PTHREAD_ODD_EVEN_SORT_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <unistd.h>

void my_pthread_launcher(vector<int> rlist, int nb_thread);


#endif //EVEN_ODD_SORT_PTHREAD_ODD_EVEN_SORT_H
