#! /usr/bin/env python3

import subprocess
import os
import math
import matplotlib.pyplot as plt
from matplotlib import colors
from matplotlib.ticker import PercentFormatter



def daemon(nb_it):
    x = 1
    while x < 17:
        if x > 6:
            args = [("./pthread/bin/heat_pthread", nb_it, str(x), '1'), ("mpirun", "-n", str(x), "./mpi/bin/heat_mpi", nb_it, '1'), ("./openmp/bin/heat_openmp", nb_it, str(x), '1')]
        else:
            args = [("./pthread/bin/heat_pthread", nb_it, str(x), '1'), ("mpirun", "-n", str(x), "./mpi/bin/heat_mpi", nb_it, '1'), ("./openmp/bin/heat_openmp", nb_it, str(x), '1')]
        for y in range(0, len(args)):
                print("\nExecuting the C++ program...")
                popen = subprocess.Popen(args[y], stdout=subprocess.PIPE)

                print("C++ program output:")
                for line in iter(popen.stdout.readline, ""):
                    print(line.decode('ascii'), end='')
                    if len(line) < 1:
                        break
                popen.wait()
        print(x)
        x+=1

def open_daemon_output(nb_it):
    algorithm = []
    nb_thread_1 = []
    nb_thread_2 = []
    nb_thread_3 = []

    exec_time_1 = []
    exec_time_2 = []
    exec_time_3 = []


    plt.yscale('linear')
    plt.xscale('linear')
    f = open("heat.out", "r")
    plt.grid(True)

    first = True

    i = 0
    for line in f.readlines():
        infos = line.replace('\n', '').split(' ')
        algorithm.append(infos[0])
        if i == 0:
            nb_thread_1.append(int(infos[1]))
            exec_time_1.append(int(infos[2]) / 1000)
        elif i == 1:
            nb_thread_2.append(int(infos[1]))
            exec_time_2.append(int(infos[2]) / 1000)
        elif i == 2:
            nb_thread_3.append(int(infos[1]))
            exec_time_3.append(int(infos[2]) / 1000)
        if i == 2:
            i = 0
        else:
            i+=1
    plt.plot(nb_thread_1, exec_time_1, '-or', label='pthread')
    plt.plot(nb_thread_2, exec_time_2, '-og', label='mpi')
    plt.plot(nb_thread_3, exec_time_3, '-ob', label='openmp')
    plt.legend(loc='upper right')
    plt.title('execution time & number of processes (map 100*100, ' + nb_it + ' iterations)')
    plt.ylabel('execution time (ms)')
    plt.xlabel('number of processes')
    plt.savefig('./image.png')
    plt.show()

    return algorithm, nb_thread, exec_time

def display_histogram(algorithm, nb_thread, exec_time, nb_elements):
    plt.yscale('linear')
    plt.xscale('linear')

    plt.grid(True)
    for dot in nb_thread:
        plt.plot(dot, exec_time, 'o-')


def main():
    nb_iter = "5000"

    daemon(nb_iter)
    algorithm, nb_thread, exec_time = open_daemon_output(nb_iter)
    print("Opening heat.out and generating graph...")
    #display_histogram(algorithm, nb_thread, exec_time, nb_elements)


if __name__ == "__main__":
    main()