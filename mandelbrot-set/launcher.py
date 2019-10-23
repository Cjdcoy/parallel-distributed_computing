#! /usr/bin/env python3

import subprocess
import os
import matplotlib.pyplot as plt
from matplotlib import colors
from matplotlib.ticker import PercentFormatter



def daemon(nb_elements):
    x = 1
    args_mpi = ("mpirun", "-n", str(x), "./mpi/bin/mandlebrot_mpi", nb_elements, '-1')
    args_pthread = ("./pthread/bin/mandlebrot_pthread", nb_elements, str(x), '-1')
    popen = subprocess.Popen(args_mpi, stdout=subprocess.PIPE)

    popen.wait()
    x +=1
    while x < 17:
        print(x)
        args_mpi = ("mpirun", "-n", str(x), "./mpi/bin/mandlebrot_mpi", nb_elements, '-1')
        args_pthread = ("./pthread/bin/mandlebrot_pthread", nb_elements, str(x), '-1')
        print("\nExecuting the C++ program...")
        popen = subprocess.Popen(args_mpi, stdout=subprocess.PIPE)

        print("C++ program output:")
        for line in iter(popen.stdout.readline, ""):
            print(line.decode('ascii'), end='')
            if len(line) < 1:
                break
        popen.wait()
        x+=1

def open_daemon_output(nb_elements):
    algorithm = []
    nb_thread_1 = []
    nb_thread_2 = []
    nb_thread_3 = []

    exec_time_1 = []
    exec_time_2 = []
    exec_time_3 = []

    plt.yscale('linear')
    plt.xscale('linear')
    f = open("mandlebrot.out", "r")
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
    plt.plot(nb_thread_1, exec_time_1, '-or', label='line')
    plt.plot(nb_thread_2, exec_time_2, '-og', label='pixel')
    plt.plot(nb_thread_3, exec_time_3, '-ob', label='zone')
    plt.legend(loc='upper right')
    plt.title('execution time & number of processes (' + nb_elements + ' iterations, ' + 'mpi' + ')')
    plt.ylabel('execution time (ms)')
    plt.xlabel('number of processes')
    plt.show()

    return algorithm, nb_thread, exec_time

def display_histogram(algorithm, nb_thread, exec_time, nb_elements):
    plt.yscale('linear')
    plt.xscale('linear')

    plt.grid(True)
    for dot in nb_thread:
        plt.plot(dot, exec_time, 'o-')


def main():
    nb_elements = "10000"

    daemon(nb_elements)
    algorithm, nb_thread, exec_time = open_daemon_output(nb_elements)
    print("Opening mandlebrot.out and generating graph...")
    #display_histogram(algorithm, nb_thread, exec_time, nb_elements)


if __name__ == "__main__":
    main()