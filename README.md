# XIAO Tail

Hacka-Tail with Seeeduino Studio XIAO BLE Sense

## Flow

```mermaid
graph TD

subgraph setup

init_servo-->init_state


end

subgraph loop

read_ble-->read_imu-->update_state-->tail.update

end

setup-->loop


```

state:

- stroke hard
- stroke soft
- ble command

action:

- swing tail up/down/left/right

## Notes

- mic library: https://github.com/Seeed-Studio/Seeed_Arduino_Mic
