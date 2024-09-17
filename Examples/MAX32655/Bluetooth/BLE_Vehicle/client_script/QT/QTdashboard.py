import sys
from PyQt5.QtWidgets import QApplication, QLabel, QVBoxLayout, QHBoxLayout, QWidget, QProgressBar, QLCDNumber
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont

class VehicleDashboard(QWidget):
    def __init__(self):
        super().__init__()

        # Set up the main layout
        layout = QVBoxLayout()

        # Set overall font style
        font = QFont("Arial", 14)

        # Speed
        self.speed_label = QLabel("Speed (cm/s):")
        self.speed_label.setFont(font)
        self.speed_display = QLCDNumber()
        self.speed_display.setSegmentStyle(QLCDNumber.Flat)
        self.speed_display.setStyleSheet("border: 2px solid black; color: #00FF00; background: #000000;")
        layout.addWidget(self.speed_label)
        layout.addWidget(self.speed_display)

        # Acceleration X and Z Axis (Vertical Split)
        self.accel_label = QLabel("Acceleration (m/s²):")
        self.accel_label.setFont(font)
        layout.addWidget(self.accel_label)

        accel_layout = QHBoxLayout()  # Horizontal layout for the two accelerations

        # X-Axis Acceleration
        self.accx_label = QLabel("X-Axis:")
        self.accx_label.setFont(font)
        self.accx_display = QLCDNumber()
        self.accx_display.setSegmentStyle(QLCDNumber.Flat)
        self.accx_display.setStyleSheet("border: 2px solid black; color: #FF8000; background: #000000;")

        accel_layout.addWidget(self.accx_label)
        accel_layout.addWidget(self.accx_display)

        # Y-Axis Acceleration
        self.accy_label = QLabel("Y-Axis:")
        self.accy_label.setFont(font)
        self.accy_display = QLCDNumber()
        self.accy_display.setSegmentStyle(QLCDNumber.Flat)
        self.accy_display.setStyleSheet("border: 2px solid black; color: #FF8000; background: #000000;")

        accel_layout.addWidget(self.accy_label)
        accel_layout.addWidget(self.accy_display)

        layout.addLayout(accel_layout)  # Add the acceleration layout to the main layout

        # Battery Level
        self.battery_label = QLabel("Battery Level (%):")
        self.battery_label.setFont(font)
        self.battery_progress = QProgressBar()
        self.battery_progress.setMinimum(0)
        self.battery_progress.setMaximum(100)
        self.battery_progress.setTextVisible(True)
        self.battery_progress.setStyleSheet("""
            QProgressBar {
                border: 2px solid grey;
                border-radius: 5px;
                background-color: #FFFFFF;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #00CCFF;
                width: 10px;
            }
        """)
        layout.addWidget(self.battery_label)
        layout.addWidget(self.battery_progress)

        # Distance
        self.distance_label = QLabel("Distance (m):")
        self.distance_label.setFont(font)
        self.distance_display = QLCDNumber()
        self.distance_display.setSegmentStyle(QLCDNumber.Flat)
        self.distance_display.setStyleSheet("border: 2px solid black; color: #FFFF00; background: #000000;")
        layout.addWidget(self.distance_label)
        layout.addWidget(self.distance_display)

        # Set up the layout and window appearance
        self.setLayout(layout)
        self.setWindowTitle("Vehicle Dashboard")
        self.setStyleSheet("background-color: #282828; color: white;")  # Dark background, white text

        # Timer to simulate data updates
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_data)
        self.timer.start(100)  # Update every second

        # Simulated data (you can replace this with real-time data)
        self.speed = 0
        self.accx = 0
        self.accy = 0
        self.batt = 0
        self.distance = 0

    def update_data(self):

        # Update the displays
        self.speed_display.display(self.speed)
        self.accx_display.display(round(self.accx, 2))
        self.accy_display.display(round(self.accy, 2))
        self.battery_progress.setValue(self.batt)
        self.distance_display.display(round(self.distance, 1))

# Application setup
if __name__ == '__main__':
    app = QApplication(sys.argv)
    dashboard = VehicleDashboard()
    dashboard.show()
    print("here is the thing")
    sys.exit(app.exec_())
