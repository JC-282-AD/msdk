# BLE_Vehicle

Bluetooth Controlled Vehicle. This example is based on the MAX32655FTHR. 

There are several functionalities is supported by this example: 

1. Accelerometer is used to detect the G value and calculate the speed. 

2. Lidar is used to sense the distance between vehicle and the obstacle in front of it. 

## Hardware Component:
### ADXL343

This is the accelerometer used in this project. The data sheet can be found in **[ADXL343 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl343.pdf)**. 

It measures acceleration forces along the X, Y, and Z axes  in ranges of ±2g, ±4g, ±8g, or ±16g. 

It supports both I²C and SPI communication interfaces. I²C communication interface will be used in the project. 

The acceleration will be detected by using this accelerometer, and the speed will be calculated by using the results from accelerometer. 


#### Connection:


### Lidar TFmini Plus

The is the Lidar used in this project to detect the distance from obstacle ahead. **[TFMiniPlus User Manual](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl343.pdf)** can be found here. It uses UART protocal. 

#### Connection:


### L298N Motor Driver

The L298N motor driver is a dual H-bridge motor driver integrated circuit (IC) used for controlling two DC motors. 

**Direction Control**: By sending appropriate HIGH or LOW signals to the input pins (IN1, IN2, IN3, IN4), the motor can be made to rotate forward or reverse.

**Speed Control**: The pulse-width modulation (PWM) will be used to control of motor speed by varying the duty cycle of the signal sent to pin ENA and ENB (Hardware Details table will be included below).


#### Hardware Details:

| Name                          |  Description                  |
| ----------------------------- | ----------------------------- |
| OUT1                          | MOTOR A (+)                   |
| OUT2                          | MOTOR A (-)                   |
| OUT3                          | MOTOR B (-)                   |
| OUT4                          | MOTOR B (+)                   |
| +5V                           | 5V regulated power out        |
| GND                           | Ground (-)                    |
| +12V                          | 12V (+) Power Supply          |
| ENA                           | Input to enable Motor A       |
| IN1                           | Input to control Motor A      |
| IN2                           | Input to control Motor A      |
| ENB                           | Input to enable Motor B       |
| IN3                           | Input to control Motor B      |
| IN4                           | Input to control Motor B      |


## Software

### Project Usage

Universal instructions on building, flashing, and debugging this project can be found in the **[MSDK User Guide](https://analogdevicesinc.github.io/msdk/USERGUIDE/)**.

### Required Connections

If using the Standard EV Kit board (EvKit\_V1):
-   Connect a USB cable between the PC and the CN2 (USB/PWR - UART) connector.
-   Close jumpers JP7 (RX_EN) and JP8 (TX_EN).
-   Close jumpers JP5 (LED1 EN) and JP6 (LED2 EN).

