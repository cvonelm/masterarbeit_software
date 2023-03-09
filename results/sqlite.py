import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

sns.set(rc={'figure.figsize':(9,4.8)})
sns.set(font_scale=1.5)
sns.set_style("ticks")
def read_data(name, file):
    file1 = open(file)
    res = []
    for line in file1:
        if "numCycles" in line:
            splitted = line.split()
            val = int(splitted[1])/1000000
            res.append([name, val])
    return res[1:]

data= read_data("unsecure", "unsecure-5.txt") + read_data("secure", "secure-5.txt") + read_data("process secure", "procsec-5.txt")

print(data)
df = pd.DataFrame(data, columns=["Experiment","Runtime (ms)"])


sns.barplot(data=df, x="Experiment", y="Runtime (ms)")
plt.ylim(2.7, 3.3)
plt.tight_layout()
plt.savefig("sqlite.pdf")
