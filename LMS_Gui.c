#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ====================== Data Structures & Constants ======================
#define MAX_BOOKS 1000
char categories[][15] = {"Computer", "Electronics", "Electrical", "Civil", "Mechanical", "Architecture"};
char password[10] = "admin123"; // Default password

struct BOOK {
    int id;
    char name[20];
    char Author[20];
    int quantity;
    float Price;
    int rackno;
    char cat[15]; // Changed from char* to fixed-size array
};
struct BOOK book;
struct BOOK library[MAX_BOOKS];
int book_count = 0;
int dark_mode = 0;

// ====================== Core Logic Functions ======================
int checkid(int t) {
    for (int i = 0; i < book_count; i++) {
        if (library[i].id == t) {
            return 0;
        }
    }
    return 1;
}

void load_books() {
    FILE *file = fopen("library.dat", "rb");
    if (!file) return;
    
    book_count = fread(library, sizeof(struct BOOK), MAX_BOOKS, file);
    fclose(file);
}

void save_books() {
    FILE *file = fopen("library.dat", "wb");
    if (file) {
        fwrite(library, sizeof(struct BOOK), book_count, file);
        fclose(file);
    }
}

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

// ====================== GUI Utility Functions ======================
void show_message(GtkWidget *parent, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error(GtkWidget *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void update_view_tab(GtkTreeView *treeview) {
    GtkListStore *store = gtk_list_store_new(7, 
        G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, 
        G_TYPE_STRING, G_TYPE_INT, G_TYPE_FLOAT, G_TYPE_INT);
        
    for (int i = 0; i < book_count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
            0, library[i].cat,
            1, library[i].id,
            2, library[i].name,
            3, library[i].Author,
            4, library[i].quantity,
            5, library[i].Price,
            6, library[i].rackno,
            -1);
    }
    
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void apply_theme(GtkWidget *widget, gpointer data) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    
    if (dark_mode) {
        const gchar *dark_css = 
            "window, .view { background-color: #2d2d2d; color: #f0f0f0; }"
            "button { background-color: #3c3c3c; color: #ffffff; border: 1px solid #555; border-radius: 4px; padding: 8px; }"
            "button:hover { background-color: #4a4a4a; }"
            "entry, textview, combobox { background-color: #3c3c3c; color: #ffffff; border: 1px solid #555; }"
            "treeview { background-color: #2d2d2d; color: #f0f0f0; }"
            "label { color: #f0f0f0; }";
        
        gtk_css_provider_load_from_data(provider, dark_css, -1, &error);
    } else {
        const gchar *light_css = 
            "window, .view { background-color: #f8f8f8; color: #333333; }"
            "button { background-color: #4a86e8; color: white; border-radius: 4px; padding: 8px; }"
            "button:hover { background-color: #3a76d8; }"
            "entry, textview, combobox { background-color: white; color: #333333; border: 1px solid #ccc; }"
            "treeview { background-color: white; color: #333333; }"
            "label { color: #333333; }";
        
        gtk_css_provider_load_from_data(provider, light_css, -1, &error);
    }
    
    if (error) {
        g_error("CSS error: %s", error->message);
        g_error_free(error);
    }
    
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

// ====================== Callback Functions ======================
void on_login(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    const char *entered = gtk_entry_get_text(GTK_ENTRY(app->login_password));
    
    if (strcmp(entered, password) == 0) {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "main_menu");
    } else {
        show_error(app->window, "Incorrect password!");
    }
}

void on_add_book(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    
    struct BOOK new_book;
    new_book.id = atoi(gtk_entry_get_text(GTK_ENTRY(app->add_id)));
    
    if (!checkid(new_book.id)) {
        show_error(app->window, "Book ID already exists!");
        return;
    }
    
    // Get category and copy it to the fixed-size array
    int active = gtk_combo_box_get_active(GTK_COMBO_BOX(app->add_category));
    if (active < 0) active = 0;
    strncpy(new_book.cat, categories[active], sizeof(new_book.cat));
    new_book.cat[sizeof(new_book.cat) - 1] = '\0'; // Ensure null-termination
    
    strncpy(new_book.name, gtk_entry_get_text(GTK_ENTRY(app->add_name)), sizeof(new_book.name));
    new_book.name[sizeof(new_book.name) - 1] = '\0'; // Ensure null-termination
    strncpy(new_book.Author, gtk_entry_get_text(GTK_ENTRY(app->add_author)), sizeof(new_book.Author));
    new_book.Author[sizeof(new_book.Author) - 1] = '\0'; // Ensure null-termination
    new_book.quantity = atoi(gtk_entry_get_text(GTK_ENTRY(app->add_quantity)));
    new_book.Price = atof(gtk_entry_get_text(GTK_ENTRY(app->add_price)));
    new_book.rackno = atoi(gtk_entry_get_text(GTK_ENTRY(app->add_rackno)));
    
    // Validate input
    if (new_book.id <= 0 || new_book.quantity <= 0 || new_book.Price <= 0) {
        show_error(app->window, "Invalid input values");
        return;
    }
    
    library[book_count++] = new_book;
    save_books();
    update_view_tab(GTK_TREE_VIEW(app->view_tree));
    show_message(app->window, "Success", "Book added successfully!");
    
    // Clear fields
    gtk_entry_set_text(GTK_ENTRY(app->add_id), "");
    gtk_entry_set_text(GTK_ENTRY(app->add_name), "");
    gtk_entry_set_text(GTK_ENTRY(app->add_author), "");
    gtk_entry_set_text(GTK_ENTRY(app->add_quantity), "");
    gtk_entry_set_text(GTK_ENTRY(app->add_price), "");
    gtk_entry_set_text(GTK_ENTRY(app->add_rackno), "");
}

void on_search_book(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    const char *term = gtk_entry_get_text(GTK_ENTRY(app->search_term));
    int search_type = gtk_combo_box_get_active(GTK_COMBO_BOX(app->search_type));
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->search_results));
    gtk_text_buffer_set_text(buffer, "", -1);
    
    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);
    
    int found = 0;
    char info[512];
    
    for (int i = 0; i < book_count; i++) {
        int match = 0;
        
        switch(search_type) {
            case 0: // ID
                if (library[i].id == atoi(term)) match = 1;
                break;
            case 1: // Name
                if (strstr(library[i].name, term) != NULL) match = 1;
                break;
            case 2: // Author
                if (strstr(library[i].Author, term) != NULL) match = 1;
                break;
            case 3: // Category
                if (strstr(library[i].cat, term) != NULL) match = 1;
                break;
        }
        
        if (match) {
            found = 1;
            snprintf(info, sizeof(info),
                "Book Found!\n"
                "----------------------------\n"
                "Category: %s\n"
                "ID: %d\n"
                "Name: %s\n"
                "Author: %s\n"
                "Quantity: %d\n"
                "Price: %.2f\n"
                "Rack No: %d\n\n",
                library[i].cat, library[i].id, library[i].name, 
                library[i].Author, library[i].quantity, 
                library[i].Price, library[i].rackno);
            
            gtk_text_buffer_insert(buffer, &iter, info, -1);
        }
    }
    
    if (!found) {
        gtk_text_buffer_insert(buffer, &iter, "No books found matching your search criteria", -1);
    }
}

void on_edit_book(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(app->edit_id)));
    int found = 0;
    
    for (int i = 0; i < book_count; i++) {
        if (library[i].id == id) {
            found = 1;
            
            // Update book data
            strncpy(library[i].name, gtk_entry_get_text(GTK_ENTRY(app->edit_name)), sizeof(library[i].name));
            library[i].name[sizeof(library[i].name) - 1] = '\0';
            strncpy(library[i].Author, gtk_entry_get_text(GTK_ENTRY(app->edit_author)), sizeof(library[i].Author));
            library[i].Author[sizeof(library[i].Author) - 1] = '\0';
            library[i].quantity = atoi(gtk_entry_get_text(GTK_ENTRY(app->edit_quantity)));
            library[i].Price = atof(gtk_entry_get_text(GTK_ENTRY(app->edit_price)));
            library[i].rackno = atoi(gtk_entry_get_text(GTK_ENTRY(app->edit_rackno)));
            
            save_books();
            update_view_tab(GTK_TREE_VIEW(app->view_tree));
            show_message(app->window, "Success", "Book updated successfully!");
            break;
        }
    }
    
    if (!found) {
        show_error(app->window, "Book ID not found!");
    }
}

void on_delete_book(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(app->delete_id)));
    int found_index = -1;
    
    // Find the book
    for (int i = 0; i < book_count; i++) {
        if (library[i].id == id) {
            found_index = i;
            break;
        }
    }
    
    if (found_index == -1) {
        show_error(app->window, "Book ID not found!");
        return;
    }
    
    // Confirm deletion
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app->window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to delete book %d: %s?", 
        library[found_index].id, library[found_index].name);
    
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        // Shift all books after the deleted one
        for (int i = found_index; i < book_count - 1; i++) {
            library[i] = library[i + 1];
        }
        book_count--;
        
        save_books();
        update_view_tab(GTK_TREE_VIEW(app->view_tree));
        show_message(app->window, "Success", "Book deleted successfully!");
        
        // Clear field
        gtk_entry_set_text(GTK_ENTRY(app->delete_id), "");
    }
}

void on_change_password(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    const char *old = gtk_entry_get_text(GTK_ENTRY(app->old_pass));
    const char *new = gtk_entry_get_text(GTK_ENTRY(app->new_pass));
    const char *confirm = gtk_entry_get_text(GTK_ENTRY(app->confirm_pass));
    
    if (strcmp(old, password) != 0) {
        show_error(app->window, "Old password is incorrect!");
        return;
    }
    
    if (strcmp(new, confirm) != 0) {
        show_error(app->window, "New passwords don't match!");
        return;
    }
    
    if (strlen(new) < 4) {
        show_error(app->window, "Password must be at least 4 characters!");
        return;
    }
    
    strncpy(password, new, sizeof(password));
    password[sizeof(password) - 1] = '\0'; // Ensure null-termination
    show_message(app->window, "Success", "Password changed successfully!");
    
    // Clear fields
    gtk_entry_set_text(GTK_ENTRY(app->old_pass), "");
    gtk_entry_set_text(GTK_ENTRY(app->new_pass), "");
    gtk_entry_set_text(GTK_ENTRY(app->confirm_pass), "");
    
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "main_menu");
}

void on_toggle_theme(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    dark_mode = !dark_mode;
    apply_theme(app->window, NULL);
    
    // Update button label
    if (dark_mode) {
        gtk_button_set_label(GTK_BUTTON(app->theme_btn), "Light Mode");
    } else {
        gtk_button_set_label(GTK_BUTTON(app->theme_btn), "Dark Mode");
    }
}

void on_navigate(GtkWidget *widget, gpointer user_data) {
    AppWidgets *app = (AppWidgets *)user_data;
    const char *page_name = gtk_widget_get_name(widget);
    
    if (page_name) {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), page_name);
        if (strcmp(page_name, "view_books") == 0) {
            update_view_tab(GTK_TREE_VIEW(app->view_tree));
        }
    }
}

// ====================== GUI Construction ======================
void build_login_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 15);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 30);
    
    GtkWidget *label = gtk_label_new("Library Management System");
    gtk_widget_set_hexpand(label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    
    // Replace deprecated gtk_widget_override_font with CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, 
        "label { font: bold 20px Sans; }", -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), 
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    
    // Rest of the function remains unchanged
    app->login_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->login_password), "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(app->login_password), FALSE);
    
    GtkWidget *login_btn = gtk_button_new_with_label("Login");
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login), app);
    
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Password:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->login_password, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), login_btn, 0, 2, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "login");
}

void build_main_menu(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    GtkWidget *label = gtk_label_new("Main Menu");
    gtk_widget_set_hexpand(label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    
    app->add_btn = gtk_button_new_with_label("Add Books");
    app->delete_btn = gtk_button_new_with_label("Delete Books");
    app->search_btn = gtk_button_new_with_label("Search Books");
    app->view_btn = gtk_button_new_with_label("View Books");
    app->edit_btn = gtk_button_new_with_label("Edit Books");
    app->pass_btn = gtk_button_new_with_label("Change Password");
    app->theme_btn = gtk_button_new_with_label(dark_mode ? "Light Mode" : "Dark Mode");
    app->exit_btn = gtk_button_new_with_label("Exit");
    
    // Set widget names and connect signals with app as user_data
    gtk_widget_set_name(app->add_btn, "add_books");
    g_signal_connect(app->add_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    gtk_widget_set_name(app->view_btn, "view_books");
    g_signal_connect(app->view_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    gtk_widget_set_name(app->search_btn, "search_books");
    g_signal_connect(app->search_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    gtk_widget_set_name(app->edit_btn, "edit_books");
    g_signal_connect(app->edit_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    gtk_widget_set_name(app->delete_btn, "delete_books");
    g_signal_connect(app->delete_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    gtk_widget_set_name(app->pass_btn, "change_password");
    g_signal_connect(app->pass_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    g_signal_connect(app->theme_btn, "clicked", G_CALLBACK(on_toggle_theme), app);
    g_signal_connect(app->exit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_btn, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->view_btn, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->search_btn, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_btn, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->delete_btn, 0, 5, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->pass_btn, 0, 6, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->theme_btn, 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), app->exit_btn, 0, 8, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "main_menu");
}

void build_add_books_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    // Create category dropdown
    app->add_category = gtk_combo_box_text_new();
    for (int i = 0; i < 6; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->add_category), categories[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->add_category), 0);
    
    // Create input fields
    app->add_id = gtk_entry_new();
    app->add_name = gtk_entry_new();
    app->add_author = gtk_entry_new();
    app->add_quantity = gtk_entry_new();
    app->add_price = gtk_entry_new();
    app->add_rackno = gtk_entry_new();
    
    // Set placeholders
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_id), "Unique ID");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_name), "Book Title");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_author), "Author Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_quantity), "Quantity");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_price), "Price");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->add_rackno), "Rack Number");
    
    // Add button
    GtkWidget *add_btn = gtk_button_new_with_label("Add Book");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_book), app);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Category:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_category, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Book ID:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_id, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Name:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_name, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Author:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_author, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Quantity:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_quantity, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Price:"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_price, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Rack No:"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->add_rackno, 1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), add_btn, 0, 7, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), back_btn, 0, 8, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "add_books");
}

void build_view_books_page(AppWidgets *app) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    // Create scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    // Create tree view
    app->view_tree = gtk_tree_view_new();
    
    // Create columns
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Category", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Author", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Qty", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Price", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Rack", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app->view_tree), column);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), app->view_tree);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    gtk_box_pack_start(GTK_BOX(vbox), back_btn, FALSE, FALSE, 5);
    
    gtk_stack_add_named(GTK_STACK(app->stack), vbox, "view_books");
}

void build_search_books_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    // Create search type dropdown
    app->search_type = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->search_type), "ID");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->search_type), "Name");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->search_type), "Author");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->search_type), "Category");
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->search_type), 0);
    
    // Search term entry
    app->search_term = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->search_term), "Search term");
    
    // Search button
    GtkWidget *search_btn = gtk_button_new_with_label("Search");
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search_book), app);
    
    // Results text view
    app->search_results = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->search_results), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->search_results), GTK_WRAP_WORD);
    
    // Create scrolled window for results
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), app->search_results);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Search By:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->search_type, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Search Term:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->search_term, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), search_btn, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), back_btn, 0, 4, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "search_books");
}


void build_edit_books_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    // Book ID to edit
    app->edit_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_id), "Enter Book ID to edit");
    
    // Edit fields
    app->edit_name = gtk_entry_new();
    app->edit_author = gtk_entry_new();
    app->edit_quantity = gtk_entry_new();
    app->edit_price = gtk_entry_new();
    app->edit_rackno = gtk_entry_new();
    
    // Set placeholders
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_name), "New Title");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_author), "New Author");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_quantity), "New Quantity");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_price), "New Price");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->edit_rackno), "New Rack Number");
    
    // Edit button
    GtkWidget *edit_btn = gtk_button_new_with_label("Update Book");
    g_signal_connect(edit_btn, "clicked", G_CALLBACK(on_edit_book), app);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Book ID to Edit:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_id, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Name:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_name, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Author:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_author, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Quantity:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_quantity, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Price:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_price, 1, 4, 1, 1); // Corrected line
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Rack No:"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->edit_rackno, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), edit_btn, 0, 6, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), back_btn, 0, 7, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "edit_books");
}


void build_delete_books_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    // Book ID to delete
    app->delete_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->delete_id), "Enter Book ID to delete");
    
    // Delete button
    GtkWidget *delete_btn = gtk_button_new_with_label("Delete Book");
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_book), app);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Book ID to Delete:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->delete_id, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), delete_btn, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), back_btn, 0, 2, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "delete_books");
}

void build_change_password_page(AppWidgets *app) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    app->old_pass = gtk_entry_new();
    app->new_pass = gtk_entry_new();
    app->confirm_pass = gtk_entry_new();
    
    // Set password visibility to false
    gtk_entry_set_visibility(GTK_ENTRY(app->old_pass), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(app->new_pass), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(app->confirm_pass), FALSE);
    
    // Set placeholders
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->old_pass), "Current password");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->new_pass), "New password");
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->confirm_pass), "Confirm new password");
    
    // Change button
    GtkWidget *change_btn = gtk_button_new_with_label("Change Password");
    g_signal_connect(change_btn, "clicked", G_CALLBACK(on_change_password), app);
    
    // Back button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Menu");
    gtk_widget_set_name(back_btn, "main_menu");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_navigate), app);
    
    // Layout
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Current Password:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->old_pass, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Password:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->new_pass, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Confirm Password:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), app->confirm_pass, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), change_btn, 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), back_btn, 0, 4, 2, 1);
    
    gtk_stack_add_named(GTK_STACK(app->stack), grid, "change_password");
}

// ====================== Main Application ======================
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    load_books();
    
    AppWidgets *app = g_malloc(sizeof(AppWidgets));
    
    // Create main window
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(app->window), 10);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create stack for multi-page interface
    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(app->stack), 300);
    gtk_container_add(GTK_CONTAINER(app->window), app->stack);
    
    // Build all pages
    build_login_page(app);
    build_main_menu(app);
    build_add_books_page(app);
    build_view_books_page(app);
    build_search_books_page(app);
    build_edit_books_page(app);
    build_delete_books_page(app);
    build_change_password_page(app);
    
    // Show login page first
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "login");
    
    // Apply initial theme
    apply_theme(app->window, NULL);
    
    gtk_widget_show_all(app->window);
    gtk_main();
    
    save_books();
    g_free(app);
    return 0;
}