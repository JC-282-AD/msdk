import asyncio
from bleak import BleakClient
from pynput import keyboard

# which double click two times quickly, it will in press state at the end!!!!!!!
address = "00:18:80:03:AE:F6"
up_uuid = "85fc567f-31d9-4185-87c6-339924d1c5be"
down_uuid = "85fc5680-31d9-4185-87c6-339924d1c5be"
left_uuid = "85fc5681-31d9-4185-87c6-339924d1c5be"
right_uuid = "85fc5682-31d9-4185-87c6-339924d1c5be"
stop_uuid = "85fc5683-31d9-4185-87c6-339924d1c5be"

notify_characteristic_uuid = "85fc567e-31d9-4185-87c6-339924d1c5be"
#data = bytearray([0x01])  # Example data to send
test= 0 

def notification_handler(sender, data):
    print(f"Notification from {sender}: {data}")

async def discover_services():
    async with BleakClient(address) as client:
        services = await client.get_services()
        for service in services:
            print(f"Service: {service.uuid}")
            for characteristic in service.characteristics:
                print(f"  Characteristic: {characteristic.uuid} | Properties: {characteristic.properties}")

async def on(client, uuid):
    await client.write_gatt_char(uuid, bytearray([0x01]))
    
    print("press")
async def off(client, uuid):
    await client.write_gatt_char(uuid, bytearray([0x00]))
    
    print("release")

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
            elif key == keyboard.Key.esc:
                exit(0)
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
        except AttributeError:
            pass  # Ignore special keys

async def main():
    async with BleakClient(address) as client:
        #await client.connect()
        print(f"Connected: {client.is_connected}")

        await client.start_notify(notify_characteristic_uuid, notification_handler)
        print("Subscribed to notifications")

        key_handler = KeyHandler(client)
        loop = asyncio.get_event_loop()
        listener = keyboard.Listener(on_press=lambda event: loop.create_task(key_handler.on_press(event)), 
                                     on_release=lambda event: loop.create_task(key_handler.on_release(event)),
                                     suppress=True)
        listener.start()

        try:
            while True:
                await asyncio.sleep(0.1)  # Small delay to prevent high CPU usage
        except asyncio.CancelledError:
            await client.stop_notify(notify_characteristic_uuid)
            print("Unsubscribed from notifications")
        finally:
            await client.disconnect()
            print("Disconnected")

loop = asyncio.get_event_loop()

try:
    loop.run_until_complete(main())
except KeyboardInterrupt:
    print("Program interrupted by user")
finally:
    loop.run_until_complete(loop.shutdown_asyncgens())
    loop.close()
