#! /usr/bin/env python3

import subprocess
import os
import matplotlib.pyplot as plt
from matplotlib import colors
from matplotlib.ticker import PercentFormatter



def daemon(nb_elements, seed):
    x = 1
    args_mpi = ("mpirun", "-n", str(x), "./bin/odd-even-sort", nb_elements, seed, "mpi")
    args_pthread = ("./bin/odd-even-sort", nb_elements, seed, "pthread", str(x))
    popen = subprocess.Popen(args_mpi, stdout=subprocess.PIPE)

    popen.wait()
    x +=1
    while x < 21:
        print(x)
        args_mpi = ("mpirun", "-n", str(x), "./bin/odd-even-sort", nb_elements, seed, "mpi")
        args_pthread = ("./bin/odd-even-sort", nb_elements, seed, "pthread", str(x))
        print("\nExecuting the C++ program...")
        popen = subprocess.Popen(args_mpi, stdout=subprocess.PIPE)

        print("C++ program output:")
        for line in iter(popen.stdout.readline, ""):
            print(line.decode('ascii'), end='')
            if len(line) < 1:
                break
        popen.wait()
        x += 1

def open_daemon_output():
    algorithm = []
    nb_thread = []
    exec_time = []
    f = open("odd_even_sort.out", "r")
    for line in f.readlines():
        infos = line.replace('\n', '').split(' ')
        algorithm.append(infos[0])
        nb_thread.append(int(infos[1]))
        exec_time.append(int(infos[2]) / 1000)

    return algorithm, nb_thread, exec_time

def display_histogram(algorithm, nb_thread, exec_time, nb_elements):
    plt.yscale('linear')
    plt.xscale('linear')

    plt.grid(True)
    plt.plot(nb_thread, exec_time, 'o-')
    plt.title('execution time & number of processes (' + nb_elements + ' elements)')
    plt.ylabel('execution time (ms)')
    plt.xlabel('number of processes')
    plt.show()

def main():
    #this variable define the size of the random vector that will be generated
    nb_elements = "10000"
    seed = "42"

    daemon(nb_elements, seed)
    algorithm, nb_thread, exec_time = open_daemon_output()
    print("Opening odd_even_sort.out and generating graph...")
    display_histogram(algorithm, nb_thread, exec_time, nb_elements)


if __name__ == "__main__":
    main()