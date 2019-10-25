#include "big-O-loops.h"

void O(float n)
{
    std::cout << "O(1) \t\t took ";
}

void O_log(float n, float step)
{
    long long it = 0; //this is O(1)

    for (float i = 1 ; i <= n ; i*=step)
        it++;
    std::cout << "O(log(n)) \t " << it << " iterations (step *=" << step << ") took ";
}


void O_n(float n, float step)
{
    long long it = 0;

    for (int i = 1 ; i <= n ; i+=step)
        it++;
    std::cout << "O(n) \t\t " << it << " iterations (step +=" << step << ") took ";
}

void O_n_log_n(float n, float step_1, float step_2)
{
    long long it = 0;

    for (int i = 1 ; i <= n ; i+=step_1)
        for (float j = 1 ; j <= n ; j*=step_2 )
            it++;

    std::cout << "O(n*log(n)) " << it << " iterations (step_1 +=" << step_1 << ", step_2 *= " << step_2 << ") took ";
}

void O_n_pow_2(float n, float step_1, float step_2)
{
    long long it = 0;

    for (int i = 1 ; i <= n ; i+=step_1)
        for (int j = 1 ; j <= n ; j+=step_2 )
            it++;

    std::cout << "O(n^2) \t\t" << it << " iterations (step_1 +=" << step_1 << ", step_2 += " << step_2 << ") took ";
}
