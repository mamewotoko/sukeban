#include <gtk/gtk.h>
#include "img.h"
#include "icon.h"
#include <iostream>
#include <list>
#include <map>
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
    line_t(int width, GdkRGBA* color){
        this->width = width;
        this->color = *color;
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
    GdkRGBA color;
};

class color_pallet_t {
public:
    color_pallet_t(){
        const char* colors[][2] = {
            { "red", "#FF0000" },
            { "navy", "#00008D" },
            { "green", "#00FF00" },
            { "black", "#000000" },
            { "pink", "#FF1493" },
            { "yellow", "#FFFF00" },
            { "murasaki", "#800080" },
            { "white", "#FFFFFF" },
            { "aqua", "#00FFFF" },
            { "orange", "#FFA500" },
            { "blue", "#0000FF" },
            { "gray", "#808080" },
            { "purple", "#800080" },
            { "fuchsia", "#FF00FF" }
        };
        int size = sizeof(colors)/sizeof(colors[0]);

        for(int i = 0; i < size; i++){
            const char* name = colors[i][0];
            const char* color_str = colors[i][1];
            GdkRGBA* c = new GdkRGBA;
            gdk_rgba_parse(c, color_str);
            data[name] = c;
        }
    }

    ~color_pallet_t(){
        //TODO: iterate keys
        delete data["red"];
        delete data["navy"];
        delete data["green"];
    }
    map<string, GdkRGBA*> data;
};

class app_context_t {
public:
    app_context_t(bool is_transparent,
                  int line_width,
                  bool is_demo)
    {
        this->is_transparent = is_transparent;
        this->line_width = line_width;
        this->is_demo = is_demo;

        this->icon = gdk_pixbuf_new_from_inline(-1, suke_icon, FALSE, NULL);
        this->win_icon = gdk_pixbuf_new_from_inline(-1, suke_icon_mini, FALSE, NULL);
        current_line = NULL;
        line_color_name = "green";
        this->is_pressing = false;
        this->pallet = new color_pallet_t();
    }

    ~app_context_t(){
        while(0 < shapes.size()){
            line_t* line = shapes.front();
            delete line;
            shapes.pop_front();
        }
        delete current_line;
        current_line = NULL;
        g_object_unref(this->win_icon);
        this->win_icon = NULL;
        g_object_unref(this->icon);
        this->icon = NULL;
    }

    GdkRGBA* get_line_color(){
        map<string, GdkRGBA*>::iterator iter = this->pallet->data.find(this->line_color_name);
        if(iter == this->pallet->data.end()){
            //default
            iter = this->pallet->data.find("green");
        }
        return iter->second;
    }

    list<line_t*> shapes;
    line_t* current_line;
    bool is_pressing;
    bool is_transparent;
    bool is_demo;
    GdkPixbuf* icon;
    GdkPixbuf* win_icon;
    int line_width;
    string line_color_name;
    color_pallet_t* pallet;
};

app_context_t* app_context;

void draw_line(cairo_t* cairo, line_t* line){
    list<point_t*>::iterator iter = line->data.begin();

    GdkRGBA* color = &line->color;
    point_t* first_point = *iter;

    if(line->data.empty()){
        return;
    }
    //TODO; handle empty line
    gdk_cairo_set_source_rgba(cairo, color);
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
    GdkRGBA* color = app_context->get_line_color();
    app_context->current_line = new line_t(app_context->line_width, color);
    app_context->current_line->data.push_back(new point_t(event->x, event->y));
}

void on_button_release(GtkWidget* widget, GdkEventButton* event){
    //app_context->coordinated
    if(event->button != 1){
        return;
    }
    app_context->current_line->data.push_back(new point_t(event->x, event->y));
    app_context->shapes.push_back(app_context->current_line);
    app_context->current_line = NULL;
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
    if(app_context->is_demo){
        gdk_cairo_set_source_pixbuf(cairo,
                                    app_context->icon,
                                    (WINDOW_WIDTH - gdk_pixbuf_get_width(app_context->icon))/2,
                                    (WINDOW_HEIGHT - gdk_pixbuf_get_height(app_context->icon))/2);
        cairo_paint(cairo);
    }
    for(list<line_t*>::iterator iter = app_context->shapes.begin();
        iter != app_context->shapes.end();
        ++iter){
        line_t* line = *iter;
        draw_line(cairo, line);
    }
    if(app_context->current_line != NULL){
        draw_line(cairo, app_context->current_line);
    }
}

void undo(GtkWidget* widget){
    if(app_context->shapes.empty()){
      return;
    }
    delete app_context->shapes.back();
    app_context->shapes.pop_back();
    gtk_widget_queue_draw(widget); 
}

void on_key_press(GtkWidget* widget, GdkEventKey* event, gpointer user_data){
    GdkModifierType modifiers;

    modifiers = gtk_accelerator_get_default_mod_mask ();
    switch(event->keyval){
    case GDK_KEY_G:
        app_context->line_color_name = "green";
        break;
    case GDK_KEY_N:
        app_context->line_color_name = "navy";
        break;
    case GDK_KEY_R:
        app_context->line_color_name = "red";
        break;
    case GDK_KEY_B:
        app_context->line_color_name = "black";
        break;
    case GDK_KEY_Y:
        app_context->line_color_name = "yellow";
        break;
    case GDK_KEY_M:
        app_context->line_color_name = "murasaki";
        break;
    case GDK_KEY_W:
        app_context->line_color_name = "white";
        break;
    case GDK_KEY_A:
        app_context->line_color_name = "aqua";
        break;
    case GDK_KEY_O:
        app_context->line_color_name = "orange";
        break;
    case GDK_KEY_P:
        app_context->line_color_name = "pink";
        break;
    case GDK_KEY_z:
	if((event->state & modifiers) == GDK_CONTROL_MASK){
	    undo(widget);
	}
        break;
    default:
        break;
    }
}

GtkWidget* transparent_window_new(){
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
    gtk_window_set_icon(GTK_WINDOW(window), app_context->win_icon);
    gtk_widget_set_visual(window, visual);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_size_request(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    GtkWidget* area = gtk_drawing_area_new();
    gtk_widget_set_events(area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK);
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(on_key_press), NULL);
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
    a.add("demo", '\0', "draw icon");
    a.parse_check(argc, argv);
    app_context = new app_context_t(!a.exist("opaque"),
                                    a.get<int>("line_width"),
                                    a.exist("demo"));
    gtk_init(&argc, &argv);

    GtkWidget *window = transparent_window_new();
    g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

    gtk_widget_show_all(window);
    gtk_main();
    delete app_context;
    return 0;
}
