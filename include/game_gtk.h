/**
 * @file game_gtk.h
 * @brief Header file for the GTK game interface.
 *
 * @startuml
 * class Game {
 *   char board[ROWS][COLS];
 *   GtkWidget* buttons[ROWS][COLS];
 * }
 * 
 * class GtkWidget {
 *   // GTK widget class
 * }
 * 
 * Game --> GtkWidget : uses
 * @enduml
 */

#ifndef GAME_GTK_H
#define GAME_GTK_H

/**
 * @brief Initializes the game.
 * 
 * @param game Pointer to the Game structure.
 * 
 * @startuml
 * Game *-- GtkWidget : initializes
 * @enduml
 */
void init_game(Game *game);

/**
 * @brief Prints the game state.
 * 
 * @param game Pointer to the Game structure.
 * 
 * @startuml
 * Game *-- GtkWidget : prints
 * @enduml
 */
void print_game(Game *game);

/**
 * @brief Callback for when the mouse enters a button.
 * 
 * @param button The button widget.
 * @param data User data.
 * 
 * @startuml
 * GtkWidget <|-- on_button_enter
 * @enduml
 */
void on_button_enter(GtkWidget *button, gpointer data);

/**
 * @brief Callback for when the mouse leaves a button.
 * 
 * @param button The button widget.
 * @param data User data.
 * 
 * @startuml
 * GtkWidget <|-- on_button_leave
 * @enduml
 */
void on_button_leave(GtkWidget *button, gpointer data);

/**
 * @brief Callback for when the mouse pointer enters a widget.
 * 
 * @param widget The widget.
 * @param event The crossing event.
 * @param user_data User data.
 * @return gboolean
 * 
 * @startuml
 * GtkWidget <|-- on_button_enter_pointer
 * @enduml
 */
gboolean on_button_enter_pointer(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);

/**
 * @brief Callback for when the mouse pointer leaves a widget.
 * 
 * @param widget The widget.
 * @param event The crossing event.
 * @param user_data User data.
 * @return gboolean
 * 
 * @startuml
 * GtkWidget <|-- on_button_leave_pointer
 * @enduml
 */
gboolean on_button_leave_pointer(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);

/**
 * @brief Callback for when a button is clicked.
 * 
 * @param button The button widget.
 * @param data User data.
 * 
 * @startuml
 * GtkWidget <|-- on_button_clicked
 * @enduml
 */
void on_button_clicked(GtkWidget *button, gpointer data);

/**
 * @brief Callback function for the "Restart" button click event.
 *
 * This function is called when the "Restart" button is clicked in the GTK window.
 * It takes a GtkWidget pointer and a gpointer as parameters.
 *
 * @param widget A pointer to the GtkWidget that triggered the event.
 * @param window A gpointer to the GTK window.
 */
void on_button_Restart_clicked(GtkWidget *widget, gpointer window);

/**
 * @brief Removes the hover effect from broken squares.
 * 
 * @param game Pointer to the Game structure.
 * 
 * @startuml
 * Game *-- GtkWidget : removes hover effect
 * @enduml
 */
void remove_button_hover_from_broken_squares(Game *game);

/**
 * @brief Stops the animation on a label.
 * 
 * @param label The label widget.
 * 
 * @startuml
 * GtkWidget <|-- stop_animation
 * @enduml
 */
void stop_animation(GtkWidget *label);

/**
 * @brief Shows the victory message.
 * 
 * @param parent The parent widget.
 * 
 * @startuml
 * GtkWidget <|-- show_victory_message
 * @enduml
 */
void show_victory_message(GtkWidget *parent);

/**
 * @brief Creates a bold label.
 * 
 * @param text The text for the label.
 * @return GtkWidget* The created label widget.
 * 
 * @startuml
 * GtkWidget <|-- create_bold_label
 * @enduml
 */
GtkWidget* create_bold_label(const char* text);

/**
 * @brief Shows the rules in a pop-up window.
 * 
 * @param widget The widget.
 * @param window The parent window.
 * 
 * @startuml
 * GtkWidget <|-- show_rules
 * @enduml
 */
void show_rules(GtkWidget *widget, gpointer window);

/**
 * @brief Sets up the UI.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * 
 * @startuml
 * GtkWidget <|-- setup_UI
 * @enduml
 */
void setup_UI(int argc, char *argv[]);

#endif /* GAME_GTK_H */