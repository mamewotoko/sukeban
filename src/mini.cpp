#include <gtk/gtk.h>
#include "img.h"
#include <iostream>
#include <list>

using namespace std;

class point_t {
public:
    point_t(int x, int y){
        this->x = x;
        this->y = y;
    }
    ~point_t(){}
    int x;
    int y;
};

typedef list<point_t> line_t;

struct app_context_t {
    list<line_t> shapes;
};

struct app_context_t app_context;

void draw_line(cairo_t* cairo, line_t* line){
    list<point_t>::iterator iter = line->begin();
    GdkRGBA color;
    color.red = 0.0;
    color.green = 0.0;
    color.blue = 0.0;
    color.alpha = 1.0;
    point_t first_point = *iter;

    //TODO; handle empty line
    gdk_cairo_set_source_rgba(cairo, &color);
    cairo_move_to(cairo, first_point.x, first_point.y);
    ++iter;
    for(; iter != line->end(); ++iter){
        cairo_line_to(cairo, (*iter).x, (*iter).y);
        cairo_move_to(cairo, (*iter).x, (*iter).y);
    }
    cairo_stroke(cairo);
}

void on_button_press(GtkWidget* widget, GdkEvent* event){
    //app_context.coordinated
}

void on_draw(GtkWidget* widget, cairo_t* cairo, gpointer data){
    //TODO: draw shapes
    cout << shapes.size() << endl;

    for(list<line_t>::iterator iter = shapes.begin(); iter != shapes.end(); ++iter){
        line_t line = *iter;
        draw_line(cairo, &line);
    }
}

GtkWidget* transparent_window_new(){
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(window, visual);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_size_request(window, 600, 600);

    /* GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("suke_icon_500.png", NULL); */
    /* GdkPixbuf* pixbuf = gdk_pixbuf_new_from_inline(-1, suke_icon, FALSE, NULL); */
    /* GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf); */
    /* gtk_container_add(GTK_CONTAINER(window), image); */

    GtkWidget* area = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(on_draw), NULL);
    gtk_container_add(GTK_CONTAINER(window), area);
    return window;
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    //TODO: delete points
    line_t line;
    line.push_back(point_t(1, 2));
    line.push_back(point_t(100, 200));
    shapes.push_back(line);

    GtkWidget *window = transparent_window_new();
    g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
