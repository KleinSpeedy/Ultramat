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

Run debug target with GTK inspector:
```sh
./run.sh -d
```

### TODOS

- Introduce **position property** and connect it to ComboBox/ComboModel
- Remove global variables in `gui.c`
- Check for duplicate IDs while parsing recipes and ingredients
- Implement cool images on page 1
- Use G_LOG_LEVEL_DEBUG for debug printing and add CMake debug target
