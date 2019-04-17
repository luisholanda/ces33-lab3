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

with open("serial.txt") as concurrent:
    seri_samples = np.loadtxt(concurrent)
# with open("parallel.txt") as parallel:
#     para_samples = np.fromfile(parallel)

means = collect_means(seri_samples)

plt.plot(means)
plt.title("Mean time to execute 1000 serial operations")
plt.xlabel("Iteration")
plt.ylabel("Mean time (s)")
plt.show()
plt.savefig("serial.jpg")