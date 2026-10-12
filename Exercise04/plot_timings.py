"""Exercise 4: draw Time vs Processes and Speedup vs Processes for Exercise 2 and 3.

Reads timings.csv (written by run_timings.sh) and saves four PNG files next to it.
Each point is the MEDIAN of the repeated runs; the light band shows the fastest to
slowest run, so you can see how noisy each measurement was.
Usage:  python plot_timings.py
"""
import csv
import os
import statistics

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.abspath(__file__))

SURFACE, INK, INK2, GRID = "#fcfcfb", "#0b0b0b", "#52514e", "#e4e3df"
PROGRAMS = {
    "sum_mpi": ("Exercise 2", "Sum of 1 to 10,000,000", "#2a78d6"),   # categorical slot 1 (blue)
    "pi_mpi":  ("Exercise 3", "Monte Carlo Pi, 10,000,000 points", "#eb6834"),  # slot 2 (orange)
}

plt.rcParams.update({
    "font.family": "DejaVu Sans", "font.size": 10,
    "axes.edgecolor": GRID, "axes.labelcolor": INK2, "axes.titlecolor": INK,
    "xtick.color": INK2, "ytick.color": INK2, "axes.facecolor": SURFACE,
    "figure.facecolor": SURFACE, "savefig.facecolor": SURFACE,
})


def load():
    data = {}
    with open(os.path.join(HERE, "timings.csv")) as f:
        for r in csv.DictReader(f):
            data.setdefault(r["program"], {}).setdefault(int(r["processes"]), []).append(float(r["time_sec"]))
    return data


def style(ax, procs):
    ax.grid(axis="y", color=GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    ax.set_xticks(procs)
    ax.set_xlabel("Number of processes")
    ax.tick_params(length=0)


def plot_program(prog, runs):
    ex, title, color = PROGRAMS[prog]
    procs = sorted(runs)
    med = [statistics.median(runs[p]) for p in procs]
    lo = [min(runs[p]) for p in procs]
    hi = [max(runs[p]) for p in procs]
    reps = len(runs[procs[0]])

    # ---- Time vs number of processes (milliseconds)
    fig, ax = plt.subplots(figsize=(7, 4.2))
    ax.fill_between(procs, [v * 1e3 for v in lo], [v * 1e3 for v in hi], color=color, alpha=0.15, linewidth=0)
    ax.plot(procs, [v * 1e3 for v in med], color=color, linewidth=2, marker="o", markersize=6,
            markeredgecolor=SURFACE, markeredgewidth=1.5)
    style(ax, procs)
    ax.set_ylim(bottom=0)
    ax.set_ylabel("Time (ms)")
    ax.set_title(f"{ex}: {title}\nTime vs number of processes", loc="left", fontsize=11)
    ax.annotate(f"{med[0] * 1e3:.1f} ms", (procs[0], med[0] * 1e3), textcoords="offset points",
                xytext=(8, 4), color=INK, fontsize=9)
    ax.annotate(f"{med[-1] * 1e3:.1f} ms", (procs[-1], med[-1] * 1e3), textcoords="offset points",
                xytext=(0, -16), ha="center", color=INK, fontsize=9)
    fig.text(0.01, 0.01, f"Line: median of {reps} runs. Band: fastest to slowest run.", color=INK2, fontsize=8)
    fig.tight_layout(rect=(0, 0.03, 1, 1))
    fig.savefig(os.path.join(HERE, f"{prog}_time.png"), dpi=150)
    plt.close(fig)

    # ---- Speedup = T(1) / T(p), with the ideal (linear) speedup for reference
    speed = [med[0] / m for m in med]
    fig, ax = plt.subplots(figsize=(7, 4.2))
    ax.plot(procs, procs, color=INK2, linewidth=1.2, linestyle=(0, (4, 3)))
    ax.annotate("Ideal (linear)", (procs[-1] - 1, procs[-1] - 1), textcoords="offset points",
                xytext=(-6, 4), color=INK2, fontsize=9, ha="right", va="bottom")
    ax.plot(procs, speed, color=color, linewidth=2, marker="o", markersize=6,
            markeredgecolor=SURFACE, markeredgewidth=1.5)
    best = max(range(len(speed)), key=speed.__getitem__)
    ax.annotate(f"best {speed[best]:.2f}x at {procs[best]} processes", (procs[best], speed[best]),
                textcoords="offset points", xytext=(0, 10), ha="center", color=INK, fontsize=9)
    style(ax, procs)
    ax.set_ylim(0, procs[-1] + 0.5)
    ax.set_yticks(procs)
    ax.set_ylabel("Speedup  (T1 / Tp)")
    ax.set_title(f"{ex}: {title}\nSpeedup vs number of processes", loc="left", fontsize=11)
    fig.text(0.01, 0.01, f"Speedup from the median of {reps} runs at each process count.", color=INK2, fontsize=8)
    fig.tight_layout(rect=(0, 0.03, 1, 1))
    fig.savefig(os.path.join(HERE, f"{prog}_speedup.png"), dpi=150)
    plt.close(fig)

    return procs, med, lo, hi, speed


if __name__ == "__main__":
    data = load()
    for prog in PROGRAMS:
        procs, med, lo, hi, speed = plot_program(prog, data[prog])
        print(PROGRAMS[prog][0], prog)
        print("| Processes | Median time (s) | Fastest (s) | Slowest (s) | Speedup | Efficiency |")
        print("|---|---|---|---|---|---|")
        for p, m, a, b, s in zip(procs, med, lo, hi, speed):
            print(f"| {p} | {m:.6f} | {a:.6f} | {b:.6f} | {s:.2f} | {s / p * 100:.0f}% |")
        print()
