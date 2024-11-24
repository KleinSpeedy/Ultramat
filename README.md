![build workflow](https://github.com/KleinSpeedy/Ultramat/actions/workflows/c-cpp.yml/badge.svg)

# Ultramat
## Automatic Drink Dispenser Project

UI for a self-built mixing robot. Stepper control through serial connection to a Arduino.

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

Testing using virtual serial port with `socat`:
```sh
# start socat and check output, should start 2 ports
socat -d -d -v pty,rawer,echo=0,link=./ttyV0 pty,rawer,echo=0,link=./ttyV1
# Set application serial path and build project
```
