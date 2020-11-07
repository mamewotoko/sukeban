#include <gtk/gtk.h>
#include "img.h"
#include <iostream>
#include <list>
#include <cmdline/cmdline.h>

using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

class point_t {
public:
    point_t(int x, int y){
        this->x = x;
        this->y = y;
    }
    ~point_t(){

    }
    int x;
    int y;
};

class line_t {
public:
    line_t(int width){
        this->width = width;
    }
    ~line_t(){
        while(0 < data.size()){
            point_t* p = data.front();
            delete p;
            data.pop_front();
        }
    }
    list<point_t*> data;
    int width;
};

class app_context_t {
public:
    app_context_t(bool is_transparent,
                  int line_width)
    {
        this->is_transparent = is_transparent;
        this->line_width = line_width;
        current_line = new line_t(this->line_width);

        this->is_pressing = false;
    }
    ~app_context_t(){
        while(0 < shapes.size()){
            line_t* line = shapes.front();
            delete line;
            shapes.pop_front();
        }
        delete current_line;
        current_line = NULL;
    }
    list<line_t*> shapes;
    line_t* current_line;
    bool is_pressing;
    bool is_transparent;
    int line_width;
};

app_context_t* app_context;

void draw_line(cairo_t* cairo, line_t* line){
    list<point_t*>::iterator iter = line->data.begin();
    GdkRGBA color;
    color.red = 0.0;
    color.green = 0.0;
    color.blue = 0.0;
    color.alpha = 1.0;
    point_t* first_point = *iter;

    if(line->data.empty()){
        return;
    }
    //TODO; handle empty line
    gdk_cairo_set_source_rgba(cairo, &color);
    cairo_set_line_width(cairo, line->width);
    cairo_move_to(cairo, first_point->x, first_point->y);
    ++iter;
    for(; iter != line->data.end(); ++iter){
        cairo_line_to(cairo, (*iter)->x, (*iter)->y);
        cairo_move_to(cairo, (*iter)->x, (*iter)->y);
    }
    cairo_stroke(cairo);
}

void on_button_press(GtkWidget* widget, GdkEventButton* event){
    //app_context->coordinated
    if(event->button != 1){
        return;
    }
    app_context->is_pressing = true;
    app_context->current_line->data.push_back(new point_t(event->x, event->y));
}

void on_button_release(GtkWidget* widget, GdkEventButton* event){
    //app_context->coordinated
    if(event->button != 1){
        return;
    }
    app_context->current_line->data.push_back(new point_t(event->x, event->y));
    app_context->shapes.push_back(app_context->current_line);
    app_context->current_line = new line_t(app_context->line_width);
    gtk_widget_queue_draw(widget);
    app_context->is_pressing = false;
}

void on_move(GtkWidget* widget, GdkEventMotion* event){
    if(!app_context->is_pressing){
        return;
    }
    app_context->current_line->data.push_back(new point_t(event->x, event->y));
    gtk_widget_queue_draw(widget);
}

void on_draw(GtkWidget* widget, cairo_t* cairo, gpointer data){
    //tmp
    if(!app_context->is_transparent){
        GdkRGBA color;
        color.red = 1.0;
        color.green = 1.0;
        color.blue = 1.0;
        color.alpha = 1.0;
        //TODO; handle empty line
        //gdk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, color);
        gdk_cairo_set_source_rgba(cairo, &color);
        cairo_rectangle(cairo, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        cairo_fill(cairo);
    }

    for(list<line_t*>::iterator iter = app_context->shapes.begin();
        iter != app_context->shapes.end();
        ++iter){
        line_t* line = *iter;
        draw_line(cairo, line);
    }
    draw_line(cairo, app_context->current_line);
}

GtkWidget* transparent_window_new(){
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(window, visual);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("suke_icon_500.png", NULL); */
    /* GdkPixbuf* pixbuf = gdk_pixbuf_new_from_inline(-1, suke_icon, FALSE, NULL); */
    /* GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf); */
    /* gtk_container_add(GTK_CONTAINER(window), image); */

    GtkWidget* area = gtk_drawing_area_new();
    gtk_widget_set_events(area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK);
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(G_OBJECT(area), "button_press_event", G_CALLBACK(on_button_press), NULL);
    g_signal_connect(G_OBJECT(area), "button_release_event", G_CALLBACK(on_button_release), NULL);
    g_signal_connect(G_OBJECT(area), "motion_notify_event", G_CALLBACK(on_move), NULL);

    gtk_container_add(GTK_CONTAINER(window), area);
    return window;
}

int main(int argc, char **argv)
{
    cmdline::parser a;

    a.add("opaque", 'o', "opaque mode");
    a.add<int>("line_width", 'w', "line width mode", false, 4);
    a.parse_check(argc, argv);
    app_context = new app_context_t(!a.exist("opaque"),
                                    a.get<int>("line_width"));
    gtk_init(&argc, &argv);

    GtkWidget *window = transparent_window_new();
    g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

    gtk_widget_show_all(window);
    gtk_main();
    delete app_context;
    cout << "exit app" << endl;
    return 0;
}
