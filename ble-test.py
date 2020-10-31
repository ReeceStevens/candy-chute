from playsound import playsound
from bluepy import btle, scanner

AUDIO_FILE = ""

class MyDelegate(btle.DefaultDelegate):
    def __init__(self, params):
        btle.DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print("Notification received")
        playsound(AUDIO_FILE)


peripheral_name = "candy-chute-client"

scanner = scanner.Scanner()
devices = scanner.scan(timeout=3)
print([d.getValueText(9) for d in scanner.getDevices()])

peripheral = None
for device in scanner.getDevices():
    if device.getValueText(9) == peripheral_name:
        peripheral = device


per = btle.Peripheral(peripheral.addr, peripheral.addrType)
per.setDelegate( MyDelegate(None) )

service_name = '00001523-1212-efde-1523-785feabcd123'
service = None
for s in per.getServices():
    if s.uuid.getCommonName() == service_name:
        service = s
ch = service.getCharacteristics()[0]
print(ch.valHandle)
per.writeCharacteristic(ch.valHandle+1, b"\x01\x00")

while True:
    if per.waitForNotifications(1.0):
        continue

    print("Waiting...")
