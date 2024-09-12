# BLE_Vehicle

Bluetooth Controlled Vehicle. This example is based on the MAX32655FTHR. 

There are several functionalities supported by this example: 

1. Accelerometer is used to detect the G value and calculate the speed. 

2. Lidar is used to sense the distance between vehicle and the obstacle in front of it. 

3. Motor driver is used to control the vehicle. 

4. ADC is applied to detect the battery level for the vehicle. 

## Hardware Component:
### ADXL343

This is the accelerometer used in this project. The data sheet can be found in **[ADXL343 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl343.pdf)**. 

It measures acceleration forces along the X, Y, and Z axes  in ranges of ±2g, ±4g, ±8g, or ±16g. 

It supports both I²C and SPI communication interfaces. I²C communication interface will be used in the project. 

The acceleration will be detected by using this accelerometer, and the speed will be calculated by using the results from accelerometer. 


### Lidar TFmini Plus

The is the Lidar used in this project to detect the distance from obstacle ahead. **[TFMiniPlus User Manual](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl343.pdf)** can be found here. It uses UART protocal. 
The default of this LiDar setting will send 100 data frames per second. The setting in this project will only send the data frame back when receiving a request. 


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

### TXS0108 Level Shifter

**Voltage Translation:** Converts signals between 1.2V to 3.6V on the A-side and 1.65V to 5.5V on the B-side. Common use cases are 3.3V -> 5V or 1.8V -> 3.3V.

In this project, since the L298N only accepts 5V signal input and ME17 only gives 3.3V signal output, this level shifter is used to convert the voltage from 3.3V to 5V. 

## Required Connections

This project uses ME17FTHR board. 

- Connect P2_6 (LPUART RX) and P2_7 (LPUART TX) with corresponding TX and RX on LiDar. 
- Connect P2_0 (Analog Input) with the VDD of the battery pack. 
- Connect P0_30 (SCL) and P0_31 (SDA) with corresponding SCL and SDA pins on ADXL343 Accelerometer. 



**Put the level shifter between for the following conenctions**
- Connect P1_6 and P1_7 (two GPIOs) with two inputs of right motor control signal. 
- Connect P1_8 and P1_9 (two GPIOs) with two inputs of left motor control signal. 
- Connect P0_24 (PWM) with ENA pin for right motor on motor driver.
- Connect P0_20 (PWM) with ENA pin for left motor on motor driver.

## Software:

There are two python files in client_script folder: `client.py` and `dashboard.py`. 
You should run `client.py` on PC to get connected with ME17 through Bluetooth. `dashboard.py` will be used by `client.py` to draw the dashboard for the vehicle. 

### CMSIS - DSP

There are two functions is used from CMSIS - DSP library in this project: FIR filter and PID motor control. 

#### PID motor control:

The PID motor control is used to make users better control the vehicle. 

#### FIR filter:

The FIR filter is used to filter the data derived from the accelerometer. 



