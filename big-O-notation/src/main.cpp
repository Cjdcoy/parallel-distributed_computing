//
// Created by cjdcoy on 25/10/19.
//

#include <iostream>
#include "timer.h"
#include "big-O-loops.h"

void launcher(int n)
{
    Timer   t;

    t.start("O");
    O(n);
    t.stop_print("O", "O_end", true);
    std::cout << std::endl;

    t.start("Olog");
    O_log(n, 4);
    t.stop_print("Olog", "O_end", true);
    t.start("Olog");
    O_log(n, 1.1);
    t.stop_print("Olog", "O_end", true);
    std::cout << std::endl;

    t.start("On");
    O_n(n, 2);
    t.stop_print("On", "O_end", true);
    t.start("On");
    O_n(n, 1);
    t.stop_print("On", "O_end", true);
    std::cout << std::endl;

    t.start("Onlogn");
    O_n_log_n(n, 2, 3);
    t.stop_print("Onlogn", "O_end", true);
    t.start("Onlogn");
    O_n_log_n(n, 1, 1.5);
    t.stop_print("Onlogn", "O_end", true);
    std::cout << std::endl;

    t.start("On^2");
    O_n_pow_2(n, 2, 2);
    t.stop_print("On^2", "O_end", true);
    t.start("On^2");
    O_n_pow_2(n, 1, 1);
    t.stop_print("On^2", "O_end", true);
    std::cout << std::endl;
}

int main(int ac, char **av)
{
    int n;

    if (ac != 2) {
        std::cout << "usage ./bigO n" << std::endl;
        return 1;
    }

    n = std::atoi(av[1]);
    launcher(n);
    return 0;
}