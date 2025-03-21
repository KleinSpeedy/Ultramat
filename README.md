![build workflow](https://github.com/KleinSpeedy/Ultramat/actions/workflows/c-cpp.yml/badge.svg)

# Ultramat
## Automatic Drink Dispenser Project

UI for a self-built mixing robot.
Stepper control through serial connection to a Arduino.

### Project Setup

Ultramat-project has 2 repositorys:  
[UI Repository using GTK](https://github.com/KleinSpeedy/Ultramat)

and  

[Arduino Source Repository](https://github.com/KleinSpeedy/Ultramat-Arduino)

### Development

Setup meson:
```sh
meson setup build
```

Build debug target:
```sh
./dewit.sh -d
```

Build all:
```sh
ninja -C build
```

Run debug target with GTK inspector:
```sh
./run.sh -d
```

#### Simulation

Testing using virtual serial ports with `socat`:
```sh
./scripts/serial_testing.sh
```
This opens two serial ports that are symlinked to `/tmp/ttyArduino` and
`/tmp/ttyBase`.
They represent the GUI and microcontroller applications.

After starting `socat` open a second terminal:
```sh
python3 ./scripts/microcontroller_serial_sim.py
```
This starts a python program simulating the microcontroller application.
It sends and receives serial messages like the microcontroller with a delayed
response.

The simulation needs the `pyserial` module.
Install it through `pip` or the system package manager or create e virtual
environment:
```sh
# Create virtual env
python3 -m venv .venv
# start venv
. .venv/bin/activate
# stop venv
deactivate
```

### TODOs

- Implement out of order arrival for response messages from microcontroller
    - its possible that to recieve a **done** while waiting for an **acknowledge**
