#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#define ROWS 7
#define COLS 9
#define BUTTON_SIZE 80  // Set desired size for buttons
#define MAXSQUARES 5
 
typedef struct {
    char board[ROWS][COLS];
    GtkWidget *buttons[ROWS][COLS];
    int last_x;
    int last_y;
} Game;
 
void init_game(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            game->board[i][j] = '1';
        }
    }
    game->last_x = -1; // Aucune case sélectionnée
    game->last_y = -1;
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
 
int resolve_pos(const char *pos, int *x, int *y) {
    const char *alpha = "ABCDEFGHI";
    const char *num = "1234567";
     
    if (strlen(pos) == 2 && strchr(alpha, pos[0]) && strchr(num, pos[1])) {
        *x = pos[1] - '1';
        *y = pos[0] - 'A';
        if (*x < ROWS && *y < COLS) {
            return 1;  // valid
        }
    }
    return 0;  // invalid
}
 
void remove_button_hover_from_broken_squares(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game->board[i][j] == '0') {  // Vérifie si la case est cassée
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[i][j]), "button-hover");
            }
        }
    }
}
 
int select_square(Game *game, const char *pos, int *count) {
    int x, y;
    if (resolve_pos(pos, &x, &y)) {
        if (game->board[x][y] != '0') {
            *count = 0;  // Reset le compteur
            for (int row = x; row < ROWS; row++) {
                for (int col = y; col < COLS; col++) {
                    if (game->board[row][col] != '0') {
                        (*count)++;  // Incrémente le compteur
                    }
                }
            }
            if (*count > MAXSQUARES) {
                printf("You can't break over 5 squares!\n");
                return 0;
            }
 
            // Casser les cases
            for (int row = x; row < ROWS; row++) {
                for (int col = y; col < COLS; col++) {
                    if (game->board[row][col] != '0') {
                        game->board[row][col] = '0';
                        gtk_style_context_add_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-broken");
                        gtk_widget_set_sensitive(game->buttons[row][col], FALSE);
                    }
                }
            }
 
            // Appel de la fonction pour retirer "button-hover" des cases cassées
            remove_button_hover_from_broken_squares(game);
 
            // Réinitialiser la couleur de la case précédente
            if (game->last_x != -1 && game->last_y != -1) {
                GtkStyleContext *prev_context = gtk_widget_get_style_context(game->buttons[game->last_x][game->last_y]);
                gtk_style_context_remove_class(prev_context, "last-move"); // Retire la classe de couleur rose
            }
 
            // Mettre à jour la dernière position
            game->last_x = x;
            game->last_y = y;
 
            // Mettre à jour la couleur du dernier coup
            GtkStyleContext *context = gtk_widget_get_style_context(game->buttons[x][y]);
            gtk_style_context_add_class(context, "last-move");
 
            return 1;
        } else {
            printf("Square is already broken!\n");
            return 0;
        }
    } else {
        printf("Position is invalid!\n");
        return 0;
    }
}
 
 
 
 
int count_squares(Game *game) {
    int count_squares = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game->board[i][j] == '1') {
                count_squares++;
            }
        }
    }
    return count_squares;
}
 
GtkWidget *turn;
int current_player = 1;
 
void on_button_enter(GtkWidget *button, gpointer data) {
    Game *game = (Game *)data;
    const char *button_label = (const char *)g_object_get_data(G_OBJECT(button), "label");
     
    int row_index, col_index;
    if (!resolve_pos(button_label, &row_index, &col_index)) {
        return;  // Position invalide
    }
 
    int over_area = 1;  // Aire autour de la case (au départ, la case seule)
 
    // Calcul de l'aire autour de la case, maximum 6 donc 5
    for (int row = row_index; row < ROWS && over_area <= 6; row++) {
        for (int col = col_index; col < COLS && over_area <= 6; col++) {
            if (game->board[row][col] != '0') {
                over_area++;
            }
        }
    }
 
    if (over_area <= 6) {
        // Appliquer la classe "button-hover" sur toutes les cases dans la zone
        for (int row = row_index; row < ROWS; row++) {
            for (int col = col_index; col < COLS; col++) {
                if (game->board[row][col] != '0') {
                    gtk_style_context_add_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-hover");
                }
            }
        }
    } else {
        // Si l'aire dépasse la limite, n'appliquer que sur la case survolée
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "button-hover");
    }
}
 
void on_button_leave(GtkWidget *button, gpointer data) {
    Game *game = (Game *)data;
    const char *button_label = (const char *)g_object_get_data(G_OBJECT(button), "label");
 
    int row_index, col_index;
    if (!resolve_pos(button_label, &row_index, &col_index)) {
        return;  // Position invalide
    }
 
    // Retirer la classe "button-hover" et "last-move" pour toutes les cases dans l'aire
    for (int row = row_index; row < ROWS; row++) {
        for (int col = col_index; col < COLS; col++) {
            if (game->board[row][col] != '0') {
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-hover");
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[row][col]), "last-move");
            }
        }
    }
}
 
 
 
void stop_animation(GtkWidget *label) {
    // Retirer l'animation en remplaçant la classe CSS par la classe statique
    gtk_style_context_remove_class(gtk_widget_get_style_context(label), "victory-message");
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "victory-message-static");
}
 
void show_victory_message(GtkWidget *parent) {
    const char *message;
 
    if (current_player == 1) {
        message = "Player 2 wins!\n      🥳🥳🥳";
    } else {
        message = "Player 1 wins!\n      🥳🥳🥳";
    }
 
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_OK,
                                                NULL);
 
    GtkWidget *content_area = gtk_message_dialog_get_message_area(GTK_MESSAGE_DIALOG(dialog));
 
    // Créer une étiquette pour le message avec le style
    GtkWidget *label = gtk_label_new(message);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
 
    // Appliquer la classe CSS à l'étiquette pour démarrer l'animation
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "victory-message");
 
    // Ajouter l'étiquette au dialogue
    gtk_container_add(GTK_CONTAINER(content_area), label);
 
    gtk_widget_show_all(dialog);
 
    // Retirer l'animation après 3 secondes (3000 millisecondes)
    g_timeout_add(3000, (GSourceFunc)stop_animation, label);
     
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
 
 
 
void on_button_clicked(GtkWidget *button, gpointer data) {
    Game *game = (Game *)data;
     
    const char *button_label = (const char *)g_object_get_data(G_OBJECT(button), "label");
    g_print("%s\n", button_label);
 
    int count = 0; // Compteur de cases
 
    // Vérifie si le coup est valide et met à jour le plateau
    if (select_square(game, button_label, &count)) {
        print_game(game);
         
        // Vérifie si la case A1 a été cassée
        if (strcmp(button_label, "A1") == 0 && count <= 5) {
            show_victory_message(gtk_widget_get_toplevel(button)); // Affiche le message de victoire
        }
 
        // Changement de joueur
        if (current_player == 1) {
            gtk_label_set_markup(GTK_LABEL(turn), "\n <b>It's the turn of player 2</b> \n");
            current_player = 2;
        } else {
            gtk_label_set_markup(GTK_LABEL(turn), "\n <b>It's the turn of player 1</b> \n");
            current_player = 1;
        }
    }
}
 
GtkWidget* create_bold_label(const char* text) {
    GtkWidget *label = gtk_label_new(NULL);
    char markup_text[256];
    snprintf(markup_text, sizeof(markup_text), "<b>%s</b>", text);
    gtk_label_set_markup(GTK_LABEL(label), markup_text);
    return label;
}
 
// Fonction pour lire le fichier rules.txt et afficher son contenu dans une fenêtre pop-up
void show_rules(GtkWidget *widget, gpointer window) {
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;

    // Lire le contenu du fichier rules.txt
    if (!g_file_get_contents("rules.txt", &content, &length, &error)) {
        g_print("Failed to open rules.txt: %s\n", error->message);
        g_clear_error(&error);
        return;
    }

    // Vérifier si le contenu est une chaîne UTF-8 valide
    if (!g_utf8_validate(content, length, NULL)) {
        g_print("Invalid UTF-8 string in rules.txt\n");
        g_free(content);
        return;
    }

    // Créer une fenêtre de dialogue avec le texte des règles
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Rules:\n%s", content);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    g_free(content);
}

void on_button_Restart_clicked(GtkWidget *widget, gpointer window) {
    // Fermer la fenêtre actuelle
    gtk_widget_destroy(window);

    // Préparer les arguments pour exécuter à nouveau l'application
    const gchar *argv[] = { "./gtk_merge_game", NULL };

    // Redémarrer l'application en créant un nouveau processus
    GError *error = NULL;
    if (!g_spawn_async(NULL, (gchar **)argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
        g_print("Erreur lors du redémarrage du jeu : %s\n", error->message);
        g_clear_error(&error);
    }

    // Quitter l'application actuelle
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button_grid;
    GtkWidget *buttons[ROWS][COLS];

    gtk_init(&argc, &argv);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "SQUARE GAME");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_style_context_add_class(gtk_widget_get_style_context(window), "main-window");

    // Ajouter une hbox en haut pour contenir le label et le bouton
    GtkWidget *hbox_turn = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_turn, FALSE, FALSE, 0);

    // Créer le label "It's the turn of player 1"
    turn = create_bold_label("It's the turn of player 1"); // Utilisez la variable globale
    gtk_style_context_add_class(gtk_widget_get_style_context(turn), "label-turn");

    // Ajouter un espace vide pour centrer le label
    gtk_box_pack_start(GTK_BOX(hbox_turn), turn, TRUE, TRUE, 0);

    // Remplacer le bouton "R" par un bouton image
    GtkWidget *button_Restart = gtk_button_new();
    gtk_widget_set_size_request(button_Restart, 50, 30);  // Taille du bouton R
    gtk_style_context_add_class(gtk_widget_get_style_context(button_Restart), "restart-button");

    // Charger l'image pour le bouton "restart"
    GdkPixbuf *R_pixbuf = gdk_pixbuf_new_from_file("restart.png", NULL);

    if (R_pixbuf != NULL) {
        int image_width = gdk_pixbuf_get_width(R_pixbuf);
        int image_height = gdk_pixbuf_get_height(R_pixbuf);
        int target_size = MIN(image_width, image_height) * 0.19;

        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(R_pixbuf, target_size, target_size, GDK_INTERP_BILINEAR);
        GtkWidget *rules_image = gtk_image_new_from_pixbuf(scaled_pixbuf);
        gtk_button_set_image(GTK_BUTTON(button_Restart), rules_image);

        g_object_unref(scaled_pixbuf);
        g_object_unref(R_pixbuf);
    } else {
        g_print("Erreur lors du chargement de l'image.\n");
    }

    // Ajouter le bouton "Restart" tout à droite dans l'hbox
    gtk_box_pack_end(GTK_BOX(hbox_turn), button_Restart, FALSE, FALSE, 0);

    // Connecter le signal "clicked" du bouton R à une fonction de rappel
    g_signal_connect(button_Restart, "clicked", G_CALLBACK(on_button_Restart_clicked), NULL);
    
    // Créer un conteneur pour la grille avec une marge de 15 pixels à droite
    GtkWidget *grid_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), grid_container, TRUE, TRUE, 0);
    
    button_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(grid_container), button_grid, TRUE, TRUE, 0);
    
    // Ajoutez un espace vide de 15 pixels à droite
    GtkWidget *spacer = gtk_event_box_new();
    gtk_widget_set_size_request(spacer, 15, 0); // Largeur de 15 pixels, hauteur 0 (non visible)
    gtk_box_pack_start(GTK_BOX(grid_container), spacer, FALSE, FALSE, 0);
    
    // Reste du code pour créer la grille et les boutons
    for (int i = 0; i < ROWS; i++) {
        char row_label[3];
        snprintf(row_label, sizeof(row_label), "%d ", i + 1);
        GtkWidget *label = create_bold_label(row_label);
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "row-label");
        gtk_grid_attach(GTK_GRID(button_grid), label, 0, i + 1, 1, 1);
    }
    
    for (int j = 0; j < COLS; j++) {
        char col_label[3];
        snprintf(col_label, sizeof(col_label), "%c ", 'A' + j);
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
            gtk_style_context_add_class(gtk_widget_get_style_context(game.buttons[i][j]), "button");
    
            gtk_grid_attach(GTK_GRID(button_grid), game.buttons[i][j], j + 1, i + 1, 1, 1);
            g_signal_connect(game.buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), &game);
            g_signal_connect(game.buttons[i][j], "enter", G_CALLBACK(on_button_enter), &game);
            g_signal_connect(game.buttons[i][j], "leave", G_CALLBACK(on_button_leave), &game);
        }
    }

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
 
    // Créer un bouton vide pour les règles
    GtkWidget *rules_button = gtk_button_new();
    gtk_widget_set_size_request(rules_button, 50, 50);

    // Charger l'image pour le bouton "rules"
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("Rules.png", NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(rules_button), "rules-button");

    if (pixbuf != NULL) {
        int image_width = gdk_pixbuf_get_width(pixbuf);
        int image_height = gdk_pixbuf_get_height(pixbuf);
        int target_size = MIN(image_width, image_height) * 0.25;

        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_size, target_size, GDK_INTERP_BILINEAR);
        GtkWidget *rules_image = gtk_image_new_from_pixbuf(scaled_pixbuf);
        gtk_button_set_image(GTK_BUTTON(rules_button), rules_image);

        g_object_unref(scaled_pixbuf);
        g_object_unref(pixbuf);
    } else {
        g_print("Erreur lors du chargement de l'image.\n");
    }

    gtk_box_pack_end(GTK_BOX(hbox), rules_button, FALSE, FALSE, 0);
    g_signal_connect(rules_button, "clicked", G_CALLBACK(show_rules), window);

    // Ajouter le label "GROUPE 3" à droite du bouton
    GtkWidget *bottom_label = create_bold_label("GROUPE 3");
    gtk_widget_set_margin_start(bottom_label, 25);
    gtk_box_pack_start(GTK_BOX(hbox), bottom_label, FALSE, FALSE, 0);
 
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
 
    gtk_widget_show_all(window);
 
    gtk_main();
 
    return 0;
}
