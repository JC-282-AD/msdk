import asyncio
from bleak import BleakClient
from pynput import keyboard
from dashboard import VehicleDashboard
import pygame
import struct
import math
import time

# add function to auto-detect the vehicle address

# which double click two times quickly, it will in press state at the end!!!!!!!
#addressevkit = "00:18:80:03:AE:F6"
#address = "00:18:80:EF:8E:90"
address = "00:18:80:77:FF:0E"

up_uuid = "85fc567f-31d9-4185-87c6-339924d1c5be"
down_uuid = "85fc5680-31d9-4185-87c6-339924d1c5be"
left_uuid = "85fc5681-31d9-4185-87c6-339924d1c5be"
right_uuid = "85fc5682-31d9-4185-87c6-339924d1c5be"
stop_uuid = "85fc5683-31d9-4185-87c6-339924d1c5be"
acc_uuid = "85fc567e-31d9-4185-87c6-339924d1c5be"
lid_uuid = "85fc5684-31d9-4185-87c6-339924d1c5be"
battery_uuid = "00002a19-0000-1000-8000-00805f9b34fb"


program_end = False
dashboard = VehicleDashboard()

ACC_TIME_PERIOD = 0.025 # 40 package per second
ACC_G = 9.80665
ADXL343_SF_2G = 0.0039 # scale factor for adxl343 accelerometer

accx_init = None
accy_init = None
accz_init = None

def acc_handler(sender, data):
    global accx_init
    global accy_init
    global accz_init

    ints = struct.unpack('<hhhhh', data)
    accs = ints[0:3]
    speedx = ints[3]
    speedy = ints[4]
    results = [math.floor(x * ADXL343_SF_2G * 100) / 100 for x in accs]
    round_results = [round(x,1) for x in results]

    if (accx_init == None):
        accx_init = round_results[0]
        accy_init = round_results[1]
        accz_init = round_results[2]
    else:
        dashboard.accx = round_results[0] - accx_init
        dashboard.accy = round_results[1] - accy_init
        dashboard.accz = round_results[2] - accz_init
        dashboard.speed = (speedx ** 2 + speedy ** 2) ** (1/2)
        dashboard.draw_dashboard()



def lid_handler(sender, data):
    result_cm = struct.unpack('<h', data)
    result_m = result_cm[0] / 100
    dashboard.distance = result_m
    dashboard.draw_dashboard()


def bat_handler(sender, data):
    dashboard.batt = int.from_bytes(data, byteorder='big')
    dashboard.draw_dashboard()


async def discover_services():
    async with BleakClient(address) as client:
        services = await client.get_services()
        for service in services:
            print(f"Service: {service.uuid}")
            for characteristic in service.characteristics:
                print(f"  Characteristic: {characteristic.uuid} | Properties: {characteristic.properties}")


async def on(client, uuid):
    global speedx
    global speedy
    global speedz
    await client.write_gatt_char(uuid, bytearray([0x01]))
    if (uuid == stop_uuid):
        speedx = 0
        speedy = 0
        speedz = 0

async def off(client, uuid):
    await client.write_gatt_char(uuid, bytearray([0x00]))

class KeyHandler:
    def __init__(self, client):
        self.client = client
        self.up = 0
        self.down = 0
        self.left = 0
        self.right = 0
        self.stop = 0

    async def on_press(self, key):
        try:
            if key == keyboard.Key.up:
                if (self.up == 0):
                    self.up = 1
                    await on(self.client, up_uuid)
            elif key == keyboard.Key.down:
                if (self.down == 0):
                    self.down = 1
                    await on(self.client, down_uuid)
            elif key == keyboard.Key.left:
                if (self.left == 0):
                    self.left = 1
                    await on(self.client, left_uuid)
            elif key == keyboard.Key.right:
                if (self.right == 0):
                    self.right = 1
                    await on(self.client, right_uuid)
            elif key == keyboard.Key.space:
                if (self.stop == 0):
                    self.stop = 1
                    await on(self.client, stop_uuid)
            
        except Exception:
            pass  # Ignore special keys
    async def on_release(self, key):
        try:
            if key == keyboard.Key.up:
                self.up = 0
                await off(self.client, up_uuid)
            elif key == keyboard.Key.down:
                self.down = 0
                await off(self.client, down_uuid)
            elif key == keyboard.Key.left:
                self.left = 0
                await off(self.client, left_uuid)
            elif key == keyboard.Key.right:
                self.right = 0
                await off(self.client, right_uuid)
            elif key == keyboard.Key.space:
                self.stop = 0
                await off(self.client, stop_uuid)
            elif key == keyboard.Key.esc:
                global program_end 
                program_end = True
        except Exception:
            pass  # Ignore special keys

async def main():
    dashboard.initialize()
    pygame.event.get()
    async with BleakClient(address) as client:

        print(f"Connected: {client.is_connected}")

        await client.start_notify(acc_uuid, acc_handler)
        await client.start_notify(lid_uuid, lid_handler)
        await client.start_notify(battery_uuid, bat_handler)
        
        print("Subscribed to notifications")

        key_handler = KeyHandler(client)
        loop = asyncio.get_event_loop()
        listener = keyboard.Listener(on_press=lambda event: loop.create_task(key_handler.on_press(event)), 
                                     on_release=lambda event: loop.create_task(key_handler.on_release(event)),
                                     suppress=True)
        listener.start()
        dashboard.draw_dashboard()
        try:
            while True:
                pygame.event.get()
                if program_end:
                    dashboard.finalize()
                    return
                await asyncio.sleep(0.1)  # Small delay to prevent high CPU usage
        except Exception:
            await client.stop_notify(acc_uuid)
            await client.stop_notify(lid_uuid)
            await client.stop_notify(battery_uuid)
            print("Unsubscribed from notifications")
        finally:
            await client.disconnect()
            print("Disconnected")

loop = asyncio.get_event_loop()

try:
    loop.run_until_complete(main())
except Exception as e:
    print("Program interrupted by user")
    print(e)
finally:
    loop.run_until_complete(loop.shutdown_asyncgens())
    loop.close()

