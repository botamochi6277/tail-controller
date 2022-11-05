# XIAO Tail

Hacka-Tail with Seeeduino Studio XIAO BLE Sense

## mic library

https://github.com/Seeed-Studio/Seeed_Arduino_Mic

## Flow

```mermaid
graph TD

imu.update-->tail.update

```

state:

- stroke hard
- stroke soft
- ble command

action:

- swing tail up/down/left/right
