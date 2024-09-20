#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROWS 7
#define COLS 9
#define BUTTON_SIZE 100  // Taille des boutons
#define MAXSQUARES 5     // Nombre maximum de cases à désactiver

typedef struct {
    char board[ROWS][COLS];                // Plateau de jeu
    GtkWidget *buttons[ROWS][COLS];        // Tableau des boutons
} Game;

void init_game(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            game->board[i][j] = '1';
        }
    }
}

void print_game(Game *game) {
    printf("   A  B  C  D  E  F  G  H  I\n");
    for (int i = 0; i < ROWS; i++) {
        printf("%d  ", i + 1);
        for (int j = 0; j < COLS; j++) {
            printf("%c  ", game->board[i][j]);
        }
        printf("\n");
    }
}

// Fonction pour résoudre la position des boutons
int resolve_pos(const char *pos, int *x, int *y) {
    const char *alpha = "ABCDEFGHI";
    const char *num = "1234567";
    
    if (strlen(pos) == 2 && strchr(alpha, pos[0]) && strchr(num, pos[1])) {
        *x = pos[1] - '1';
        *y = pos[0] - 'A';
        if (*x < ROWS && *y < COLS) {
            return 1;  // Position valide
        }
    }
    return 0;  // Position invalide
}

// Vérifie combien de cases seront désactivées et s'il est valide de casser ces cases
int can_break_squares(Game *game, int x, int y) {
    int count = 0;
    for (int row = x; row < ROWS && count <= MAXSQUARES; row++) {
        for (int col = y; col < COLS; col++) {
            if (game->board[row][col] != '0') {
                count++;
            }
        }
    }
    return count <= MAXSQUARES;
}

// Sélectionne une case et met à jour le plateau si valide
int select_square(Game *game, const char *pos) {
    int x, y;
    if (resolve_pos(pos, &x, &y)) {
        if (game->board[x][y] != '0') {
            if (can_break_squares(game, x, y)) {
                for (int row = x; row < ROWS; row++) {
                    for (int col = y; col < COLS; col++) {
                        if (game->board[row][col] != '0') {
                            game->board[row][col] = '0';
                        }
                    }
                }
                return 1;  // Case valide, mise à jour effectuée
            } else {
                printf("You can't break over %d squares!\n", MAXSQUARES);
                return 0;  // Case invalide, dépassement des MAXSQUARES
            }
        } else {
            printf("Square is already broken!\n");
            return 0;  // Case déjà désactivée
        }
    } else {
        printf("Position is invalid!\n");
        return 0;  // Position invalide
    }
}

// Désactive tous les boutons dont la valeur est '0' et applique la classe CSS pour griser
void update_button_states(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game->board[i][j] == '0') {
                gtk_widget_set_sensitive(game->buttons[i][j], FALSE);
                gtk_style_context_add_class(gtk_widget_get_style_context(game->buttons[i][j]), "button-disabled");
            }
        }
    }
}

GtkWidget *turn;
int current_player = 1;

// Fonction déclenchée lorsqu'un bouton est cliqué
void on_button_clicked(GtkWidget *button, gpointer data) {
    Game *game = (Game *)data;
    
    const char *button_label = (const char *)g_object_get_data(G_OBJECT(button), "label");
    g_print("%s\n", button_label);

    int x, y;
    if (resolve_pos(button_label, &x, &y)) {
        // Vérifie si la sélection est valide et met à jour le plateau
        if (select_square(game, button_label)) {
            print_game(game);
            update_button_states(game);  // Désactive les boutons avec '0'
            gtk_widget_set_sensitive(button, FALSE); // Désactiver le bouton cliqué
            gtk_style_context_add_class(gtk_widget_get_style_context(button), "button-disabled"); // Griser le bouton
        } else {
            // Si la case n'est pas valide, ne pas désactiver le bouton et afficher un message
            printf("Selection failed, button stays active!\n");
            // Ne change pas le style du bouton ici
        }
    }

    // Gestion des tours de jeu
    if (current_player == 1) {
        gtk_label_set_markup(GTK_LABEL(turn), "\n <b>It's the turn of player B</b> \n");
        current_player = 2;
    } else {
        gtk_label_set_markup(GTK_LABEL(turn), "\n <b>It's the turn of player A</b> \n");
        current_player = 1;
    }
}

// Fonction pour créer des labels en gras
GtkWidget* create_bold_label(const char* text) {
    GtkWidget *label = gtk_label_new(NULL);
    char markup_text[256];
    snprintf(markup_text, sizeof(markup_text), "<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(label), markup_text);
    return label;
}

// Fonction principale
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button_grid;

    gtk_init(&argc, &argv);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Plateau 7x9");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    turn = create_bold_label("\n It's the turn of player A \n");
    gtk_style_context_add_class(gtk_widget_get_style_context(turn), "label-turn");
    gtk_box_pack_start(GTK_BOX(vbox), turn, FALSE, FALSE, 0);

    button_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), button_grid, TRUE, TRUE, 0);


    for (int i = 0; i < ROWS; i++) {
        char row_label[3];
        snprintf(row_label, sizeof(row_label), "%d", i + 1);
        GtkWidget *label = create_bold_label(row_label);
        gtk_grid_attach(GTK_GRID(button_grid), label, 0, i + 1, 1, 1);
    }

    for (int j = 0; j < COLS; j++) {
        char col_label[3];
        snprintf(col_label, sizeof(col_label), "%c", 'A' + j);
        GtkWidget *label = create_bold_label(col_label);
        gtk_grid_attach(GTK_GRID(button_grid), label, j + 1, 0, 1, 1);
    }

    Game game;
    init_game(&game);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            char button_label[3];
            snprintf(button_label, sizeof(button_label), "%c%d", 'A' + j, i + 1);
            game.buttons[i][j] = gtk_button_new();

            gtk_widget_set_size_request(game.buttons[i][j], BUTTON_SIZE, BUTTON_SIZE);
            g_object_set_data(G_OBJECT(game.buttons[i][j]), "label", g_strdup(button_label));

            gtk_grid_attach(GTK_GRID(button_grid), game.buttons[i][j], j + 1, i + 1, 1, 1);
            g_signal_connect(game.buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), &game);
        }
    }

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *bottom_label = create_bold_label("GROUPE 3");
    gtk_box_pack_end(GTK_BOX(hbox), bottom_label, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
