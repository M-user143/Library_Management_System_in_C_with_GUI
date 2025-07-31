#ifndef CORE_FUNCTIONS_H
#define CORE_FUNCTIONS_H

#include <gtk/gtk.h>

// ====================== GUI Structures ======================
typedef struct {
    GtkWidget *window;
    GtkWidget *stack;
    
    // Login Page
    GtkWidget *login_password;
    
    // Main Menu
    GtkWidget *add_btn, *delete_btn, *search_btn, *view_btn, *edit_btn, *pass_btn, *exit_btn, *theme_btn;
    
    // Add Books
    GtkWidget *add_category, *add_id, *add_name, *add_author, *add_quantity, *add_price, *add_rackno;
    
    // View Books
    GtkWidget *view_tree;
    
    // Search Books
    GtkWidget *search_type, *search_term, *search_results;
    
    // Edit Books
    GtkWidget *edit_id, *edit_name, *edit_author, *edit_quantity, *edit_price, *edit_rackno;
    
    // Delete Books
    GtkWidget *delete_id;
    
    // Change Password
    GtkWidget *old_pass, *new_pass, *confirm_pass;
} AppWidgets;

    int checkid(int t);
    void load_books();
    void save_books();
    void show_message(GtkWidget *parent, const char *title, const char *message);
    void show_error(GtkWidget *parent, const char *message);
    void update_view_tab(GtkTreeView *treeview);
    void apply_theme(GtkWidget *widget, gpointer data);
    void on_login(GtkWidget *widget, gpointer data);
    void on_add_book(GtkWidget *widget, gpointer data);
    void on_search_book(GtkWidget *widget, gpointer data);
    void on_edit_book(GtkWidget *widget, gpointer data);
    void on_delete_book(GtkWidget *widget, gpointer data);
    void on_change_password(GtkWidget *widget, gpointer data);
    void on_toggle_theme(GtkWidget *widget, gpointer data);
    void on_navigate(GtkWidget *widget, gpointer user_data);
    void build_login_page(AppWidgets *app);
    void build_main_menu(AppWidgets *app);
    void build_add_books_page(AppWidgets *app);
    void build_view_books_page(AppWidgets *app);
    void build_search_books_page(AppWidgets *app);
    void build_edit_books_page(AppWidgets *app);
    void build_delete_books_page(AppWidgets *app);
    void build_change_password_page(AppWidgets *app);




#endif // CORE_FUNCTIONS_H