import sys
import asyncio
import logging

from playsound import playsound
from bleak import BleakClient, BleakScanner, discover

logger = logging.getLogger(__name__)

AUDIO_FILE = ""

async def run_ble_client(name: str):
    async def callback_handler(sender, data):
        print("Data received: {}".format(data))

    def filter_func(d, ad):
        print(d.name)
        return d.name and d.name.lower() == name.lower()

    devices = await discover()
    print(devices)

    device = await BleakScanner.find_device_by_filter(filter_func)
    print(device)

    async with BleakClient(device.address) as client:
        logger.info(f"Connected: {client.is_connected}")
        print(await client.get_services())

        # await client.start_notify(char_uuid, callback_handler)
        # await asyncio.sleep(10.0)
        # await client.stop_notify(char_uuid)



async def main(name: str):
    await run_ble_client(name)

if __name__ == '__main__':
   logging.basicConfig(level=logging.INFO)
   asyncio.run(
        main(
            sys.argv[1] if len(sys.argv) > 1 else ADDRESS,
            # sys.argv[2] if len(sys.argv) > 2 else CHARACTERISTIC_UUID,
        )
   )
