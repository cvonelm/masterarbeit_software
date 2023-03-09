import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

sns.set(font_scale=2)
sns.set_style("ticks")
def read_data(name, file):
    file1 = open(file)
    res = []
    for line in file1:
        if "numCycles" in line:
            splitted = line.split()
            val = int(splitted[1])
            if val < 5000:
                res.append([name, val])
    return res[10:]
data = read_data("funccall", "fcall.txt") + read_data("ccall","ccall_lat1.txt") + read_data("pcall", "pcall_empty_lat1.txt") + read_data("syscall", "syscall_empty.txt")
df = pd.DataFrame(data, columns=["Calling Semantic","Latency (cycles)"])

ax = sns.barplot(data=df, x="Calling Semantic", y="Latency (cycles)")
ax.tick_params(axis='both', which='major', labelsize=20)
ax.set_xlabel("Calling Convention", fontsize=22)
ax.set_ylabel("Latency (cycles)", fontsize=22)

plt.tight_layout()
plt.savefig("calling-small.pdf")
