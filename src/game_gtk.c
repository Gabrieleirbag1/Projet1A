#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_logic.h"
#include "../include/game_gtk.h"
#include "../include/ia.h"

void init_game(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            game->board[i][j] = 1;
        }
    }
    game->last_x = -1;
    game->last_y = -1;
}


void print_game(Game *game) {
    printf("\n   A  B  C  D  E  F  G  H  I\n");
    for (int i = 0; i < ROWS; i++) {
        printf("%d  ", i + 1);
        for (int j = 0; j < COLS; j++) {
            printf("%d  ", game->board[i][j]);
        }
        printf("\n");
    }
}

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
            if (game->board[row][col] != 0) {
                over_area++;
            }
        }
    }

    if (over_area <= 6) {
        // Appliquer la classe "button-hover" sur toutes les cases dans la zone
        for (int row = row_index; row < ROWS; row++) {
            for (int col = col_index; col < COLS; col++) {
                if (game->board[row][col] != 0) {
                    gtk_style_context_add_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-hover");
                }
            }
        }
    } else {
        // Si l'aire dépasse la limite, n'appliquer que sur la case survolée
        gtk_style_context_add_class(gtk_widget_get_style_context(button), "button-wrong-hover");
    }

    // Vérifier si c'est le tour du joueur actuel pour changer le 
    if (current_player  != player) {
        // Changer le curseur en "pointer"
        GdkWindow *window = gtk_widget_get_window(button);
        GdkCursor *cursor = gdk_cursor_new_from_name(gdk_display_get_default(), "pointer");
        gdk_window_set_cursor(window, cursor);
        g_object_unref(cursor);
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
            if (game->board[row][col] != 0) {
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-hover");
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[row][col]), "last-move");
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[row][col]), "button-wrong-hover");
            }
        }
    }

    // Réinitialiser le curseur
    GdkWindow *window = gtk_widget_get_window(button);
    gdk_window_set_cursor(window, NULL);
}

// Fonction de rappel pour changer le curseur en pointer
gboolean on_button_enter_pointer(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data) {
    GdkWindow *window = gtk_widget_get_window(widget);
    GdkDisplay *display = gdk_window_get_display(window);
    GdkCursor *cursor = gdk_cursor_new_from_name(display, "pointer");
    gdk_window_set_cursor(window, cursor);
    g_object_unref(cursor);
    return FALSE;
}

// Fonction de rappel pour réinitialiser le curseur
gboolean on_button_leave_pointer(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data) {
    GdkWindow *window = gtk_widget_get_window(widget);
    gdk_window_set_cursor(window, NULL);
    return FALSE;
}

void on_button_Restart_clicked(GtkWidget *widget, gpointer window) {
    // Préparer les arguments pour exécuter à nouveau l'application
    const gchar *argv_with_ia[] = { "./build/game", "-l", "-ia", NULL };
    const gchar *argv_without_ia[] = { "./build/game", "-l", NULL };
    const gchar **argv = ia == 1 ? argv_with_ia : argv_without_ia;

    // Redémarrer l'application en créant un nouveau processus
    GError *error = NULL;
    if (!g_spawn_async(NULL, (gchar **)argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
        g_print("Erreur lors du redémarrage du jeu : %s\n", error->message);
        g_clear_error(&error);
    }

    // Quitter l'application actuelle
    gtk_main_quit();
    exit(0);
}

void on_button_clicked(GtkWidget *button, gpointer data) {
    if (current_player == player) {
        return;
    }
    else {
        const char *button_label = (const char *)g_object_get_data(G_OBJECT(button), "label");
        // Vérifie si le coup est valide et met à jour le plateau
        int status = manage_game(&game, button_label);
        if (local == 0){
            if (status == 0){
                send_message(button_label);
            }
        } else{
            if (ia == 1 && status == 0){
                ia_move(&game);
            }
        }
    }
}

void remove_button_hover_from_broken_squares(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game->board[i][j] == 0) {  // Vérifie si la case est cassée
                gtk_style_context_remove_class(gtk_widget_get_style_context(game->buttons[i][j]), "button-hover");
            }
        }
    }
}

// Fonction pour arrêter l'animation
void stop_animation(GtkWidget *label) {
    gtk_style_context_remove_class(gtk_widget_get_style_context(label), "victory-message");
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "victory-message-static");
}

// Fonction pour ajuster la taille de la fenêtre en fonction de la taille de l'étiquette
gboolean adjust_window_size(GtkWidget *window) {
    if (!GTK_IS_WINDOW(window)) {
        return FALSE; // Arrêter l'ajustement si la fenêtre n'est plus valide
    }

    GtkWidget *label = g_object_get_data(G_OBJECT(window), "label");
    GtkRequisition natural_size;
    gtk_widget_get_preferred_size(label, NULL, &natural_size);
    int width, height;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    gtk_window_resize(GTK_WINDOW(window), natural_size.width + 50, natural_size.height + 100); // Ajuster la largeur et la hauteur avec une marge
    return TRUE; // Continuer l'ajustement
}

// Fonction pour fermer la fenêtre
void close_window(GtkWidget *button, GtkWidget *window) {
    gtk_widget_destroy(window);
}

// Fonction pour afficher un message de victoire
void show_victory_message(GtkWidget *parent) {
    const char *message;

    if (current_player == 1) {
        message = "Player 2 wins!\n      🥳🥳🥳";
    } else {
        message = "Player 1 wins!\n      🥳🥳🥳";
    }

    // Créer une nouvelle fenêtre
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Victory");
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);

    // Créer une boîte verticale pour contenir le message et le bouton
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // Créer une étiquette pour le message avec le style
    GtkWidget *label = gtk_label_new(message);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "victory-message");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

    // Stocker l'étiquette dans les données de l'objet fenêtre
    g_object_set_data(G_OBJECT(window), "label", label);

    // Créer un bouton OK pour fermer la fenêtre
    GtkWidget *button = gtk_button_new_with_label("Validate");
    gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
    g_signal_connect(button, "clicked", G_CALLBACK(close_window), window);
    g_signal_connect(button, "enter-notify-event", G_CALLBACK(on_button_enter_pointer), NULL);
    g_signal_connect(button, "leave-notify-event", G_CALLBACK(on_button_leave_pointer), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    // Retirer l'animation après 3 secondes (3000 millisecondes)
    g_timeout_add(3000, (GSourceFunc)stop_animation, label);

    // Ajuster la taille de la fenêtre en fonction de la taille de l'étiquette
    g_timeout_add(50, (GSourceFunc)adjust_window_size, window);
}

// Fonction pour créer un label en gras
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
    if (!g_file_get_contents("src/medias/rules.txt", &content, &length, &error)) {
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

    // Obtenir une référence au bouton "OK"
    GtkWidget *ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    // Connecter les signaux enter-notify-event et leave-notify-event au bouton "OK"
    g_signal_connect(ok_button, "enter-notify-event", G_CALLBACK(on_button_enter_pointer), NULL);
    g_signal_connect(ok_button, "leave-notify-event", G_CALLBACK(on_button_leave_pointer), NULL);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    g_free(content);
}

void setup_UI(int argc, char *argv[]) { 
    // Initialiser les threads X11
    XInitThreads();
    // Initialiser GTK+
    gtk_init(&argc, &argv);
    // Créer les widgets
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button_grid;
    GtkWidget *buttons[ROWS][COLS];
    GtkSettings *settings;
    // Charger le fichier CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "src/medias/style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    // Créer la fenêtre principale
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // Définir l'icône de l'application
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "src/medias/square_game_icon.png", NULL);

    char title[50];
    if (local == 1) {
        snprintf(title, sizeof(title), "SQUARE GAME - Local Game");
    } else {
        snprintf(title, sizeof(title), "SQUARE GAME - Player %s", player == 1 ? "2" : "1");
    }
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);

    settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_style_context_add_class(gtk_widget_get_style_context(window), "main-window");

    // Ajouter une hbox en haut pour contenir le label et le bouton
    GtkWidget *hbox_turn = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_turn, FALSE, FALSE, 0);

    turn = create_bold_label("\n Turn : Player 1 \n");
    gtk_style_context_add_class(gtk_widget_get_style_context(turn), "label-turn");

    // Ajouter un espace vide pour centrer le label
    gtk_box_pack_start(GTK_BOX(hbox_turn), turn, TRUE, TRUE, 0);

    if (local == 1) {
        // Remplacer le bouton "R" par un bouton image
        GtkWidget *button_Restart = gtk_button_new();
        gtk_widget_set_size_request(button_Restart, 50, 30);  // Taille du bouton R
        gtk_style_context_add_class(gtk_widget_get_style_context(button_Restart), "restart-button");
        // Connecter les signaux enter-notify-event et leave-notify-event
        g_signal_connect(button_Restart, "enter-notify-event", G_CALLBACK(on_button_enter_pointer), NULL);
        g_signal_connect(button_Restart, "leave-notify-event", G_CALLBACK(on_button_leave_pointer), NULL);

        // Charger l'image pour le bouton "restart"
        GdkPixbuf *R_pixbuf = gdk_pixbuf_new_from_file("src/medias/restart.png", NULL);

        if (R_pixbuf != NULL) {
            int image_width = gdk_pixbuf_get_width(R_pixbuf);
            int image_height = gdk_pixbuf_get_height(R_pixbuf);
            int target_size = MIN(image_width, image_height) * 0.2;

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
    }
    // Créer un conteneur pour la grille avec une marge de 15 pixels à droite
    GtkWidget *grid_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), grid_container, TRUE, TRUE, 0);

    // Créer la grille de boutons
    button_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(grid_container), button_grid, TRUE, TRUE, 0);

    // Ajoutez un espace vide de 15 pixels à droite
    GtkWidget *spacer = gtk_event_box_new();
    gtk_widget_set_size_request(spacer, 15, 0); // Largeur de 15 pixels, hauteur 0 (non visible)
    gtk_box_pack_start(GTK_BOX(grid_container), spacer, FALSE, FALSE, 0);

    for (int i = 0; i < ROWS; i++) {
        char row_label[3];
        snprintf(row_label, sizeof(row_label), "%d ", i + 1);
        GtkWidget *label = create_bold_label(row_label);
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "row-label");  // Ajout de la classe CSS
        gtk_grid_attach(GTK_GRID(button_grid), label, 0, i + 1, 1, 1);
    }

    for (int j = 0; j < COLS; j++) {
        char col_label[3];
        snprintf(col_label, sizeof(col_label), "%c ", 'A' + j);
        GtkWidget *label = create_bold_label(col_label);
        gtk_grid_attach(GTK_GRID(button_grid), label, j + 1, 0, 1, 1);
    }

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
            if (ia == 0 || local == 1) {
                g_signal_connect(game.buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), &game);
                g_signal_connect(game.buttons[i][j], "enter", G_CALLBACK(on_button_enter), &game);
                g_signal_connect(game.buttons[i][j], "leave", G_CALLBACK(on_button_leave), &game);
            } else{
                gtk_widget_set_sensitive(game.buttons[i][j], FALSE);
            }
        }
    }
 
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Créer un bouton règles
    GtkWidget *rules_button = gtk_button_new();
    gtk_widget_set_size_request(rules_button, 50, 50);  // Fixe une taille de bouton de 50x50 (par exemple)
    // Connecter les signaux enter-notify-event et leave-notify-event
    g_signal_connect(rules_button, "enter-notify-event", G_CALLBACK(on_button_enter_pointer), NULL);
    g_signal_connect(rules_button, "leave-notify-event", G_CALLBACK(on_button_leave_pointer), NULL);

    // Charger l'image depuis un fichier
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("src/medias/Rules.png", NULL);  // Remplacez par le chemin de votre image
    gtk_style_context_add_class(gtk_widget_get_style_context(rules_button), "rules-button");

    if (pixbuf != NULL) {
        // Obtenir la taille de l'image
        int image_width = gdk_pixbuf_get_width(pixbuf);
        int image_height = gdk_pixbuf_get_height(pixbuf);

        // Calculer une taille proportionnelle pour l'image dans le bouton (par exemple, 50% de la plus petite dimension)
        int target_size = MIN(image_width, image_height) * 0.2;  // 50% de la plus petite dimension

        // Redimensionner le pixbuf à la taille cible
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_size, target_size, GDK_INTERP_BILINEAR);

        // Créer une nouvelle image à partir du pixbuf redimensionné
        GtkWidget *rules_image = gtk_image_new_from_pixbuf(scaled_pixbuf);

        // Ajouter l'image au bouton
        gtk_button_set_image(GTK_BUTTON(rules_button), rules_image);

        // Libérer la mémoire des pixbuf
        g_object_unref(scaled_pixbuf);
        g_object_unref(pixbuf);
    } 
    else {
        g_print("Erreur lors du chargement de l'image.\n");
    }

    gtk_box_pack_end(GTK_BOX(hbox), rules_button, FALSE, FALSE, 0);  // Ajouter le bouton à la hbox
    g_signal_connect(rules_button, "clicked", G_CALLBACK(show_rules), window);


    // Ajouter le label "GROUPE 3" à droite du bouton
    GtkWidget *bottom_label = create_bold_label("GROUP 3");
    gtk_widget_set_margin_start(bottom_label, 28);
    gtk_box_pack_start(GTK_BOX(hbox), bottom_label, FALSE, FALSE, 0);

    if (client == 1 && ia == 1){
        ia_move(&game);
    }
 
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();
}
