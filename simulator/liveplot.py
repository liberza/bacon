import matplotlib
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np
import math

NUM_POINTS = 200

class LivePlot():
    ''' For displaying a live plot of balloons as they go through simulated flight. '''
    def __init__(self):
        # Make plot interactive.
        matplotlib.rcParams['toolbar'] = 'None'
        plt.ion()
        self.fig = plt.figure()
        self.fig.canvas.set_window_title("live flight graph - bacon simulator")
        self.ax = self.fig.add_subplot(111)
        # set up axes
        self.ax.set_xlabel("time (s)")
        self.ax.set_ylabel("alt (m)")

    def create_line(self):
        # create a line and return it
        line, = self.ax.plot([], [])
        return line

    def update_line(self, x, y, line):
        prev_x = line.get_xdata()
        prev_y = line.get_ydata()
        # only plot NUM_POINTS most recent points.
        if (len(prev_x) > NUM_POINTS):
            prev_x = prev_x[-NUM_POINTS:]
        if (len(prev_y) > NUM_POINTS):
            prev_y = prev_y[-NUM_POINTS:]
        x = np.append(prev_x, x)
        y = np.append(prev_y, y)
        line.set_xdata(x)
        line.set_ydata(y)
        # reset axes
        self.ax.relim()
        self.ax.autoscale_view()
        plt.tight_layout()
        self.fig.canvas.draw()

if __name__ == "__main__":
    # live sin plot, neato
    lp = LivePlot()
    line1 = lp.create_line()
    line2 = lp.create_line()
    for phase in np.linspace(0, 15*np.pi, 300):
        lp.update_line(phase, math.sin(phase), line1)
        lp.update_line(-phase, -math.sin(phase), line2)
