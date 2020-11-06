#include <gtk/gtk.h>
//TODO: save as c file
#include "img.h"

GtkWidget* transparent_window_new(){
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(window, visual);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_size_request(window, 600, 600);

    /* GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("suke_icon_500.png", NULL); */
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_inline(-1, suke_icon, FALSE, NULL);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_container_add(GTK_CONTAINER(window), image);

    /* GtkWidget* area = gtk_drawing_area_new(); */
    /* gtk_container_add(GTK_CONTAINER(window), area); */
    return window;
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    GtkWidget *window = transparent_window_new();
    g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
