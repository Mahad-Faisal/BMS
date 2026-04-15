import libusb_package
libusb_package.get_libusb1_backend()

from gs_usb.gs_usb import GsUsb
from gs_usb.gs_usb_frame import GsUsbFrame
from gs_usb.constants import CAN_EFF_FLAG, GS_CAN_MODE_NORMAL

devs = GsUsb.scan()
print(f"found: {devs}")

if not devs:
    print("ERROR: no CANable found")
    exit()

gs = devs[0]
gs.set_bitrate(500000)
gs.start(GS_CAN_MODE_NORMAL)
print(f"started in NORMAL mode (flags=0)")

print("Listening for CAN frames...")
count = 0
gs.start(0)  # force flags=0, bypass any constant issues
print(f"started with flags=0")

while True:
    frame = GsUsbFrame()
    if gs.read(frame, 100):
        can_id = frame.arbitration_id & ~CAN_EFF_FLAG
        print(f"ID: {can_id:#05x}  DLC: {frame.dlc}  Data: {frame.data[:frame.dlc].hex(' ')}")
    else:
        count += 1
        if count % 10 == 0:
            print(f"no frame yet ({count} timeouts)")