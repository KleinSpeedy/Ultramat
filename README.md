![build workflow](https://github.com/KleinSpeedy/Ultramat/actions/workflows/c-cpp.yml/badge.svg)

# Ultramat
## Automatic Drink Dispenser Project

UI for a self-built mixing robot. Stepper control through serial connection to a Arduino.

---
### Project Setup

Ultramat-project has 2 repositorys:  
[UI Repository using GTK](https://github.com/KleinSpeedy/Ultramat)

and  

[Arduino Source Repository](https://github.com/KleinSpeedy/Ultramat-Arduino)


**Project Setup using VSCode:**  

- `git clone https://github.com/KleinSpeedy/Ultramat.git`
- `git clone https://github.com/KleinSpeedy/Ultramat-Arduino.git`
- `cd Ultramat` and open VSCode in the project directory
- Add Ultramat-Arduino Folder to VSC-Workspace

---
### Dependencies
**"Main" Repo:**

- [GTK Setup for Linux](https://www.gtk.org/docs/installations/linux)
- Install all Dependencies
- Install MS-VSCode C++ Extension Pack

**Arduino Repo:**

- See Arduino README

---

`dewit.sh`  

Builds GTK Project.

`run.sh`  

Runs Executable.

### TODOS

- Make `Ingredient_t` and `Recipe_t` a `G_TYPE_BOXED`

---

- Remove global variables in `gui.c`
- Refactor `drinks.c`, `drinklist.c` and `drinkarray.c` 
- Check for duplicate IDs while parsing recipes and ingredients
- Implement cool images on page 1
