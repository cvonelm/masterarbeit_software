import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.gridspec import GridSpec
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
data = read_data("pcall", "pcall_empty_lat1.txt") + read_data("syscall", "syscall_empty.txt") + read_data("pagetable", "ptable_noflush.txt") + read_data("procswitch", "procswitch_light.txt")
df = pd.DataFrame(data, columns=["Calling Semantic","Latency (cycles)"])
sns.set(rc={'figure.figsize':(9,4.8)})
sns.set_style("ticks")
gs = GridSpec(2, 2, height_ratios=[1, 3])
fig = plt.figure()
#fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)\
ax1 = fig.add_subplot(gs.new_subplotspec((0, 0), colspan=2))
ax2 = fig.add_subplot(gs.new_subplotspec((1, 0), colspan=2))
ax = fig.add_subplot(111, frameon=False)
ax.set_yticks([])
ax.set_xticks([])
fig.subplots_adjust(hspace=0.05)  # adjust space between axes
# plot the same data on both axes
sns.barplot(data=df, x="Calling Semantic", y="Latency (cycles)", ax=ax1)
sns.barplot(data=df, x="Calling Semantic", y="Latency (cycles)", ax = ax2 )
# zoom-in / limit the view to different portions of the data
ax1.set_ylim(2000, 3000)  # outliers only
ax2.set_ylim(0, 250)  # most of the data
ax1.set(xlabel="", ylabel="")
ax2.set(xlabel="", ylabel="")
ax.yaxis.set_major_locator(ticker.MultipleLocator(5))
ax.yaxis.set_major_formatter(ticker.ScalarFormatter())
ax2.tick_params(axis='both', which='major', labelsize=19)
ax1.tick_params(axis='both', which='major', labelsize=19)
ax.set_xlabel("Calling Convention", fontsize=20, labelpad=25)
ax.set_ylabel("Latency (cycles)", fontsize=20, labelpad=55)
# hide the spines between ax and ax2
ax1.spines.bottom.set_visible(False)
ax2.spines.top.set_visible(False)
ax1.xaxis.tick_top()
ax1.tick_params(labeltop=False)  # don't put tick labels at the top
ax2.xaxis.tick_bottom()

# Now, let's turn towards the cut-out slanted lines.
# We create line objects in axes coordinates, in which (0,0), (0,1),
# (1,0), and (1,1) are the four corners of the axes.
# The slanted lines themselves are markers at those locations, such that the
# lines keep their angle and position, independent of the axes size or scale
# Finally, we need to disable clipping.

d = .7  # proportion of vertical to horizontal extent of the slanted line
kwargs = dict(marker=[(-1, -d), (1, d)], markersize=12,
              linestyle="none", color='k', mec='k', mew=1, clip_on=False)


ax1.plot([0, 1], [0, 0], transform=ax1.transAxes, **kwargs)
ax2.plot([0, 1], [1, 1], transform=ax2.transAxes, **kwargs)

plt.tight_layout()
plt.savefig("calling-large.pdf")
