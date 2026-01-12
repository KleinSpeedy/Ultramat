![build workflow](https://github.com/KleinSpeedy/Ultramat/actions/workflows/c-cpp.yml/badge.svg)

# Ultramat
## Automatic Drink Dispenser Project

UI for a self-built mixing robot.

### Development

Setup meson, meson version > `1.0` needed:
```sh
meson setup build
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

- lots
