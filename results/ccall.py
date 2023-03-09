import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

def read_data(name, file):
    file1 = open(file)
    res = []
    for line in file1:
        if "numCycles" in line:
            splitted = line.split()
            val = int(splitted[1])
            if val < 1000:
                res.append([name, val])
    return res[10:]
sns.set(rc={'figure.figsize':(7.5,4.8)})
sns.set(font_scale=2)
sns.set_style("ticks")
data = read_data("(fcall)", "fcall.txt") + read_data("0","ccall_lat1.txt") + read_data("1", "ccall_lat2.txt") + read_data("2", "ccall_lat3.txt") + read_data("3", "ccall_lat4.txt")

df = pd.DataFrame(data, columns=["Extra Latency","Overall Latency (cycles)"])

sns.barplot(data=df, x="Extra Latency", y="Overall Latency (cycles)")
plt.tight_layout()
plt.savefig("ccall.pdf")
