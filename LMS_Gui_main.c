#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core_functions.h"

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

// ====================== END of LMS_Gui.c ======================//