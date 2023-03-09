import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt

from matplotlib import gridspec

def read_data(name, file):
    file1 = open(file)
    res = []
    for line in file1:
        if "numCycles" in line:
            splitted = line.split()
            val = int(splitted[1])
            if val < 5000:
                res.append([name, val])

    # Leave out the first 10 entries as they are considered warmup runs
    return res[10:]

data_sandbox = read_data("ccall","ccall_lat1.txt") + \
               read_data("ptabswitch", "ptable.txt") + \
               read_data("procswitch", "procswitch_light.txt")

df_sandbox   = pd.DataFrame(data_sandbox,
                            columns=["Experiment","Latency (cycles)"])

# Axes as follows: upper lefthand = ax1, upper righthand = ax2,
#                  lower lefthand = ax3, lower righthand = ax4
fig = plt.figure(figsize = (6, 2.5))

#gspec = gridspec.GridSpec(ncols = 2, nrows = 2,# hspace = 0.4,
#                          height_ratios = [1, 3])
ax2 = fig.add_subplot()#gspec[1])
ax4 = fig.add_subplot()#gspec[3])

palette = sns.color_palette("tab10", n_colors = 6)
p1 = palette[0:3]
p2 = palette[3:6]

#plt.subplots_adjust(wspace = 0.4)

sns.barplot(data=df_sandbox, x="Experiment", y="Latency (cycles)", ax = ax2, palette = p2)
sns.barplot(data=df_sandbox, x="Experiment", y="Latency (cycles)", ax = ax4, palette = p2)

# Set limits of y axes
ax2.set_ylim(2550, 2650)

ax2.spines.bottom.set_visible(False)
ax2.yaxis.set_ticks((2550, 2650))   # Constrain to two labels on y axis

ax2.tick_params(labeltop=False)         # don't put tick labels at the top
ax2.tick_params(bottom = False, labelbottom = False, labelsize = "large")
ax2.set(xlabel = None)

ax2.set(ylabel = None)

ax4.set_ylim(0, 300)

ax4.spines.top.set_visible(False)

ax4.set(xlabel = None)
ax4.xaxis.tick_bottom()

ax4.set(ylabel = None)

# Make xlabels rotated
plt.draw()
ax4.set_xticks(ax4.get_xticks())
ax4.set_xticklabels(ax4.get_xticklabels(), rotation=30, ha='right')
ax4.tick_params(labelsize = "large")

# Draw slanted lines between the two y axes

d = .5  # proportion of vertical to horizontal extent of the slanted line
kwargs = dict(marker=[(-1, -d), (1, d)], markersize=12,
              linestyle="none", color='k', mec='k', mew=1, clip_on=False)

ax2.plot([0, 1], [0, 0], transform=ax2.transAxes, **kwargs)
ax4.plot([0, 1], [1, 1], transform=ax4.transAxes, **kwargs)

# Use a common label for all subplots (we actually dont need one on the x axis)
# fig.supxlabel("Experiment", size = "medium")
fig.supylabel("Latency (cycles)", size = "large")

# plt.tight_layout()
plt.savefig("calling3.pdf", bbox_inches = 'tight')
