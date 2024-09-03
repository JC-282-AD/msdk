import asyncio
from bleak import BleakScanner, BleakClient

async def discover_services(device_address):
    async with BleakClient(device_address) as client:
        services = await client.get_services()
        print(f"Connected to {device_address}")
        for service in services:
            print(f"Service: {service.uuid}")
            for char in service.characteristics:
                print(f"  Characteristic: {char.uuid}, Properties: {char.properties}")
                for desc in char.descriptors:
                    print(f"    Descriptor: {desc.uuid}")

async def main():
    # Scan for devices
    print("Scanning for devices...")
    devices = await BleakScanner.discover()
    
    # Print discovered devices
    for i, device in enumerate(devices):
        print(f"{i}: {device.name} - {device.address}")
    
    # Select a device (for demonstration, select the first device)
    if devices:
        device_address = devices[0].address
        print(f"Connecting to {devices[0].name} - {device_address}")
        
        # Discover services
        await discover_services(device_address)
    else:
        print("No devices found.")

# Run the asyncio event loop
asyncio.run(main())
