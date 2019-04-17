import numpy as np
import matplotlib.pyplot as plt

def collect_means(samples):
    means = []
    current = 0.0
    for sampleidx in range(len(samples)):
        current = (current * sampleidx + samples[sampleidx]) / (sampleidx + 1)
        means.append(current)
    return means

seri_samples = None
para_samples = None

with open("serial.txt") as serial:
    seri_samples = np.loadtxt(serial)
with open("parallel.txt") as parallel:
    para_samples = np.loadtxt(parallel)

seri_means = collect_means(seri_samples)
para_means = collect_means(para_samples)

plt.plot(list(zip(seri_means, para_means)))
plt.title("Mean time to execute 1000 operations")
plt.xlabel("Iteration")
plt.ylabel("Time (s)")
plt.legend(["Synchronous", "Asynchronous"])
plt.show()

with open("threads.txt") as threads:
    threads_means = np.loadtxt(threads)

plt.plot(range(1,25), threads_means)
plt.title("Mean time to execute 1000 operations (500 samples)")
plt.xlabel("Number of consumers")
plt.ylabel("Time (s)")
plt.show()