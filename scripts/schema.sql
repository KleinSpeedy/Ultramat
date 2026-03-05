CREATE TABLE IF NOT EXISTS ingredients (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    type INTEGER NOT NULL,
    icon_name TEXT
);

CREATE TABLE IF NOT EXISTS recipes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS recipe_steps (
    recipe_id INTEGER,
    ingredient_id INTEGER,
    count REAL, -- TODO: Or integer?
    unit INTEGER,
    PRIMARY KEY (recipe_id, ingredient_id),
    FOREIGN KEY (recipe_id) REFERENCES recipes(id),
    FOREIGN KEY (ingredient_id) REFERENCES ingredients(id)
);

INSERT INTO ingredients (name, type, icon_name) VALUES("O-Saft", 1, "cmp_osaft.png");
INSERT INTO ingredients (name, type, icon_name) VALUES("A-Saft", 1, "cmp_asaft.png");

INSERT INTO recipes (name) VALUES("Vodka-O");
INSERT INTO recipes (name) VALUES("Vodka-A");
INSERT INTO recipes (name) VALUES("Gin-Tonic");
