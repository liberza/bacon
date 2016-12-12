import matplotlib.pyplot as plt
import numpy as np
import math

class LivePlot():
    def __init__(self):
        plt.ion()
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(111)

    def create_line(self):
        line, = self.ax.plot([], [])
        return line

    def update_line(self, x, y, line):
        x = np.append(line.get_xdata()[-100:], x)
        y = np.append(line.get_ydata()[-100:], y)
        line.set_xdata(x)
        line.set_ydata(y)
        self.ax.relim()
        self.ax.autoscale_view()
        self.fig.canvas.draw()

if __name__ == "__main__":
    lp = LivePlot()
    line1 = lp.create_line()
    line2 = lp.create_line()
    for phase in np.linspace(0, 15*np.pi, 300):
        lp.update_line(phase, math.sin(phase), line1)
        lp.update_line(-phase, -math.sin(phase), line2)
