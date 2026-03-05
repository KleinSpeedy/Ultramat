#!/usr/bin/env python3

import sqlite3
import sys

# --- KONFIGURATION DER ENUMS ---

UNIT_MAP = {
    0: "ml",
    1: "cl",
    2: "bbd", # barbutler dosis / step
    3: "g", # zukünftig ice crusher
}

TYPE_MAP = {
    0: "Softdrink",
    1: "Saft",
    2: "Spirituose"
}

DB_NAME = "bar_mixer.db"

# --- HELFER FUNKTIONEN ---

def get_valid_int_input(prompt, valid_options=None):
    """Fragt den User nach einer Zahl und validiert die Eingabe."""
    while True:
        try:
            val = int(input(prompt))
            if valid_options and val not in valid_options:
                print(f"Bitte eine Zahl aus den Optionen wählen: {list(valid_options.keys())}")
                continue
            return val
        except ValueError:
            print("Bitte eine gültige Ganzzahl eingeben.")

def print_enum_options(enum_map):
    """Zeigt die verfügbaren Optionen für Einheiten/Typen an."""
    for key, value in enum_map.items():
        print(f"  [{key}] {value}")

def list_entries(table_name):
    """Listet einfache ID/Name Paare auf (für Auswahlmenüs)."""
    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    c.execute(f"SELECT id, name FROM {table_name}")
    rows = c.fetchall()
    conn.close()

    if not rows:
        print(f"Keine Einträge in '{table_name}' gefunden.")
        return False

    print(f"\n--- {table_name.upper()} ---")
    for row in rows:
        print(f"ID: {row[0]} | Name: {row[1]}")
    print("-------------------")
    return True

# --- HAUPTFUNKTIONEN ---

def add_ingredient():
    print("\n--- NEUE ZUTAT ANLEGEN ---")
    name = input("Name der Zutat: ").strip()
    if not name:
        print("Name darf nicht leer sein.")
        return

    print("Wähle den Typ:")
    print_enum_options(TYPE_MAP)
    typ_id = get_valid_int_input("Typ-ID eingeben: ", TYPE_MAP)
    icon = input("Name der Icon Datei (Empty allowed): ").strip()

    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    try:
        c.execute("INSERT INTO ingredients (name, type, icon_name) VALUES (?, ?, ?)", (name, typ_id, icon))
        conn.commit()
        print(f"Zutat '{name}' ({TYPE_MAP[typ_id]}) erfolgreich gespeichert.")
    except sqlite3.Error as e:
        print(f"Datenbankfehler: {e}")
    finally:
        conn.close()

def add_recipe():
    print("\n--- NEUES REZEPT ANLEGEN ---")
    name = input("Name des Rezepts: ").strip()
    if not name: return

    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    try:
        c.execute("INSERT INTO recipes (name) VALUES (?)", (name,))
        conn.commit()
        print(f"Rezept '{name}' angelegt.")
    except sqlite3.Error as e:
        print(f"Fehler: {e}")
    finally:
        conn.close()

def add_recipe_ingredient():
    print("\n--- ZUTAT ZU REZEPT HINZUFÜGEN ---")

    # 1. Rezept auswählen
    if not list_entries("recipes"): return
    r_id = get_valid_int_input("Rezept ID wählen: ")

    # 2. Zutat auswählen
    if not list_entries("ingredients"): return
    z_id = get_valid_int_input("Zutat ID wählen: ")

    # 3. Menge und Einheit
    menge = get_valid_int_input("Menge (Ganzzahl): ")

    print("Wähle die Einheit:")
    print_enum_options(UNIT_MAP)
    e_id = get_valid_int_input("Einheit-ID eingeben: ", UNIT_MAP)

    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()
    c.execute("PRAGMA foreign_keys = ON;") # Wichtig für Integrity Check
    try:
        c.execute("""
            INSERT INTO recipe_steps (recipe_id, ingredient_id, count, unit)
            VALUES (?, ?, ?, ?)
        """, (r_id, z_id, menge, e_id))
        conn.commit()
        print("Zutat erfolgreich zum Rezept hinzugefügt.")
    except sqlite3.IntegrityError:
        print("Fehler: ID existiert nicht oder Zutat ist bereits in diesem Rezept enthalten.")
    except sqlite3.Error as e:
        print(f"SQL Fehler: {e}")
    finally:
        conn.close()


def show_recipe_details():
    """Zeigt ein Rezept komplett mit aufgelösten Enums an."""
    if not list_entries("recipes"): return
    r_id = get_valid_int_input("Rezept ID für Details wählen: ")

    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()

    # Join Query um Namen und Enums aufzulösen
    query = """
        SELECT r.name, i.name, rs.count, rs.unit, i.type
        FROM recipe_steps rs
        JOIN recipes r ON r.id = rs.recipe_id
        JOIN ingredients i ON i.id = rs.ingredient_id
        WHERE r.id = ?
    """
    c.execute(query, (r_id,))
    rows = c.fetchall()
    conn.close()

    if not rows:
        print("Keine Zutaten für dieses Rezept gefunden.")
        return

    recipe_name = rows[0][0]
    print(f"\nRezept: {recipe_name}")
    print("=" * (len(recipe_name) + 8))

    for row in rows:
        z_name = row[1]
        menge = row[2]
        einheit_int = row[3]
        typ_int = row[4]

        # Mapping der Integers zurück zu Strings für die Anzeige
        einheit_str = UNIT_MAP.get(einheit_int, "?")
        typ_str = TYPE_MAP.get(typ_int, "?")

        print(f"- {menge} {einheit_str} {z_name} ({typ_str})")


def show_ingredients():
    conn = sqlite3.connect(DB_NAME)
    c = conn.cursor()

    c.execute("SELECT * FROM ingredients")
    rows = c.fetchall()
    conn.close()

    print()
    if not rows:
        print("Keine Zutaten gefunden\n")
        return

    for row in rows:
        id = row[0]
        name = row[1]
        typ_int = row[2]
        typ_str = TYPE_MAP.get(typ_int, "?")
        icon = row[3]

        print(f"{id} - {name} | {typ_str}({typ_int}) - {icon}")


# --- MAIN LOOP ---
def main():
    while True:
        print("\n--- BAR MANAGER ---")
        print("1. Zutat anlegen")
        print("2. Rezept anlegen")
        print("3. Zutat zu Rezept hinzufügen")
        print("4. Rezept anzeigen")
        print("5. Zutaten anzeigen")
        print("q. Beenden")

        choice = input("Auswahl: ").lower()

        if choice == '1':
            add_ingredient()
        elif choice == '2':
            add_recipe()
        elif choice == '3':
            add_recipe_ingredient()
        elif choice == '4':
            show_recipe_details()
        elif choice == '5':
            show_ingredients()
        elif choice == 'q':
            sys.exit()
        else:
            print("Ungültige Eingabe.")

if __name__ == "__main__":
    main()
