import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import math

time_format = mdates.DateFormatter("[%H:%M:%S]")

class LivePlot():
    def __init__(self):
        plt.ion()
        self.fig = plt.figure()
        self.fig.canvas.set_window_title("live flight graph - bacon simulator")
        self.ax = self.fig.add_subplot(111)
        self.ax.set_xlabel("time (s)")
        self.ax.set_ylabel("alt (m)")
        self.ax.format_xdata = time_format

    def create_line(self):
        line, = self.ax.plot([], [])
        return line

    def update_line(self, x, y, line):
        prev_x = line.get_xdata()
        prev_y = line.get_ydata()
        if (len(prev_x) > 100):
            prev_x = prev_x[-100:]
        if (len(prev_y) > 100):
            prev_y = prev_y[-100:]
        x = np.append(prev_x, x)
        y = np.append(prev_y, y)
        line.set_xdata(x)
        line.set_ydata(y)
        self.ax.relim()
        self.fig.autofmt_xdate()
        self.ax.autoscale_view()
        plt.tight_layout()
        self.fig.canvas.draw()

if __name__ == "__main__":
    lp = LivePlot()
    line1 = lp.create_line()
    line2 = lp.create_line()
    for phase in np.linspace(0, 15*np.pi, 300):
        lp.update_line(phase, math.sin(phase), line1)
        lp.update_line(-phase, -math.sin(phase), line2)
