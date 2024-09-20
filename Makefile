# Nom de l'exécutable
TARGET = game

# Dossier contenant les fichiers sources
SRC_DIR = src

# Dossier contenant les fichiers objets
OBJ_DIR = build/obj

# Dossier contenant l'exécutable
BIN_DIR = build

# Fichiers sources
SRCS = $(SRC_DIR)/game.c $(SRC_DIR)/server.c $(SRC_DIR)/client.c $(SRC_DIR)/game_gtk.c $(SRC_DIR)/game_logic.c $(SRC_DIR)/ia.c

# Fichiers objets
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Options du compilateur
CFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lpthread -lX11

# Règle par défaut
all: $(BIN_DIR)/$(TARGET) docs

# Règle pour créer l'exécutable à partir des fichiers objets
$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	gcc -o $@ $(OBJS) $(LDFLAGS)

# Règle pour compiler les fichiers sources en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	gcc -c -o $@ $< $(CFLAGS)

# Créer les dossiers nécessaires s'ils n'existent pas
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Règle pour forcer la recompilation
force: ;

# Règle pour générer la documentation
docs:
	doxygen Doxyfile

# Règle pour nettoyer les fichiers générés
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	rm -rf docs

# Règle pour nettoyer les fichiers temporaires
.PHONY: clean force