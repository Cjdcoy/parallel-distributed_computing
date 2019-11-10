#! /usr/bin/env python3

import subprocess
import os
import math
import matplotlib.pyplot as plt
from matplotlib import colors
from matplotlib.ticker import PercentFormatter



def daemon(nb_it):
    x = 1
    while x < 5:
        print(nb_it)
        args = ("./barnes-hut/bin/nbody_bh", nb_it, '2')
        print("\nExecuting the C++ program...")
        popen = subprocess.Popen(args, stdout=subprocess.PIPE)

        print("C++ program output:")
        for line in iter(popen.stdout.readline, ""):
            print(line.decode('ascii'), end='')
            if len(line) < 1:
                break
                popen.wait()
        print(x)
        nb_it = str(int(nb_it) * 10)
        x+=1

def open_daemon_output(nb_it):
    algorithm = []
    nb_thread_1 = []

    exec_time_1 = []


    plt.yscale('linear')
    plt.xscale('linear')
    f = open("nbody.out", "r")
    plt.grid(True)

    first = True

    i = 0
    for line in f.readlines():
        infos = line.replace('\n', '').split(' ')
        algorithm.append(infos[0])
        print(infos[1], infos[2])
        nb_thread_1.append(int(infos[1]))
        exec_time_1.append(int(infos[2]) / 1000)
        i+=1
    plt.plot(nb_thread_1, exec_time_1, '-ob', label='barnes hut')
    plt.legend(loc='upper right')
    plt.title('execution time & number of processes (100 body)')
    plt.ylabel('execution time (ms)')
    plt.xlabel('number of iterations')
    plt.show()

    return algorithm, nb_thread, exec_time

def display_histogram(algorithm, nb_thread, exec_time, nb_elements):
    plt.yscale('linear')
    plt.xscale('linear')

    plt.grid(True)
    for dot in nb_thread:
        plt.plot(dot, exec_time, 'o-')


def main():
    nb_iter = "10"

    daemon(nb_iter)
    algorithm, nb_thread, exec_time = open_daemon_output(nb_iter)
    print("Opening nbody.out and generating graph...")
    #display_histogram(algorithm, nb_thread, exec_time, nb_elements)


if __name__ == "__main__":
    main()