/*
 * squirrer - a simple image viewer for reading comics
 * Copyright (C) 2014 Lorenzo Mureu
 * 
 * This file is part of squirrer.
 *
 * squirrer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * squirrer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with squirrer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>                // this is for GTK
#include <gdk/gdkkeysyms.h>         // this is for Keyboard's codes
#include <unistd.h>                 // POSIX symbols and constants
#include <stdio.h>                  // C standard lib for I/O
#include <stdlib.h>                 // C standard lib
#include <string.h>                 // strcmp()
#include "futils.h"                 // this is my lib for file utils
#include "listdir.h"                // this is my lib for scanning directories
#include "helpdialog.h"
#include "squirrer.h"               // global objects

// -- global variables
GtkWidget *windowMain, *folderdialog;
GtkLabel *labelStatus;
GtkImage *imageMain;
GdkCursor *oldcursor;
char **folder;
int folder_i, folder_max;
int fullscreen=FALSE;
float zoom;
// -- end of global variables

// -- function declarations --
gboolean windowMain_keypress (GtkWidget *widget, GdkEvent  *event, gpointer   user_data);
GtkAdjustment* get_vscroll_adjustment();
void load_image(const char *filename);
void load_next(void);
void toggle_fullscreen(void);
void vscroll_top(void);
void vscroll_up(void);
void vscroll_down(void);
void folder_free(void);
void load_folder_file(const char *folderpath, const char *filename);
void load_folder(char *f);
void onclose(void);
void load_previous(void);
void update_label(void);
void choose_folder(void);
void reload_image(void);
void zoom_in(void);
void zoom_out(void);
void zoom_reset(void);

// -- end of function declarations --

gboolean windowMain_keypress (GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
{  
  switch(event->key.keyval) {
    case GDK_KEY_plus:
    case GDK_KEY_KP_Add:
        zoom_in();
        break;
    case GDK_KEY_minus:
    case GDK_KEY_KP_Subtract:
        zoom_out();
        break;
    case GDK_KEY_p:
    case GDK_KEY_o:
    //case GDK_KEY_P:
        choose_folder();
        break;
    case GDK_KEY_Left:
    case GDK_KEY_comma:
        load_previous();
        break;
    case GDK_KEY_h:
        helpdialog_show();
        break;
    case GDK_KEY_Right:
    case GDK_KEY_period:
        load_next();
        break;
    case GDK_KEY_F11:
    case GDK_KEY_Return:
    //case GDK_KEY_q:
        toggle_fullscreen();
        break;
    case GDK_KEY_Up:
        vscroll_up();
        break;
    case GDK_KEY_Down:
        vscroll_down();
        break;
    case GDK_KEY_Page_Up:
        vscroll_top();
        break;
    case GDK_KEY_Escape:
        onclose();
        break;
  }
  return TRUE;
}

void update_label()
{
    char text[50];
    snprintf(text, 50, "%s\n[%d/%d]", 
            basename(folder[folder_i]) ,folder_i+1,folder_max);
    gtk_label_set_text(labelStatus, text);
}

void onclose()
{
    folder_free();
    gtk_main_quit();
}

void load_previous()
{
    if (folder != NULL && folder_i-1 >= 0) {
        zoom_reset();
        load_image(folder[--folder_i]);
    } else {
        choose_folder();
    }
}

GtkAdjustment* get_vscroll_adjustment()
{
    GtkScrollable *viewport;
    viewport = GTK_SCROLLABLE(gtk_builder_get_object(builder, "viewportMain"));
    return gtk_scrollable_get_vadjustment (viewport);
}

void vscroll_top() 
{
    GtkAdjustment *adjustment = get_vscroll_adjustment();
    gtk_adjustment_set_value (adjustment, 0);
}

void vscroll_up()
{
    GtkAdjustment *adjustment = get_vscroll_adjustment();
    gdouble value = gtk_adjustment_get_value (adjustment);
    value-=gtk_adjustment_get_step_increment(adjustment);
    gtk_adjustment_set_value (adjustment,value);
}

void vscroll_down()
{
    GtkAdjustment *adjustment = get_vscroll_adjustment();
    gdouble value = gtk_adjustment_get_value (adjustment);
    value+=gtk_adjustment_get_step_increment(adjustment);
    gtk_adjustment_set_value (adjustment,value);
}

void hide_cursor()
{
    GdkWindow *w;
    GdkCursor *cur;
    
    w = gtk_widget_get_window (GTK_WIDGET(windowMain));
    oldcursor = gdk_window_get_cursor(w);
    cur = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_BLANK_CURSOR);
    gdk_window_set_cursor(w, cur);
    
    //w = GDK_WINDOW(windowMain);
    
}

void unhide_cursor()
{
    GdkWindow *w;
    //GdkCursor *cur;
    
    w = gtk_widget_get_window (GTK_WIDGET(windowMain));
    //cur = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_ARROW);
    gdk_window_set_cursor(w, oldcursor);
    
    //w = GDK_WINDOW(windowMain);
}

gboolean on_eventboxImage_button_press_event (GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
{
    printf("Button pressed: %d\n", event->button.button);
    switch(event->button.button)
    {
        case 1: // left click
            load_previous();
            break;
        case 2: // middle click
            zoom_reset();
            break;
        case 3: // right click
            load_next();
            break;
    }
    return TRUE;
}


void toggle_fullscreen()
{
    if(fullscreen) {
        gtk_window_unfullscreen(GTK_WINDOW(windowMain));
        fullscreen=FALSE;
        unhide_cursor();
    } else {
        gtk_window_fullscreen(GTK_WINDOW(windowMain));
        fullscreen=TRUE;
        hide_cursor();
    }
    reload_image();
}

void folder_free()
{
    if ( folder == NULL) {
        return;
    }
    
    for (folder_i=0; folder_i<folder_max; folder_i++) {
        free(folder[folder_i]);
    }
    free(folder);
    folder=NULL;
    
}

void reload_image()
{
    if (folder != NULL && folder_i >= 0 && folder_i < folder_max) {
        load_image(folder[folder_i]);
    }
}

void choose_folder()
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Open Folder", GTK_WINDOW(windowMain),
                                action, "_Cancel", GTK_RESPONSE_CANCEL,
                                "_Open",   GTK_RESPONSE_ACCEPT, NULL);
    
    //char *cwd = get_current_dir_name();
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), ".");
    //free(cwd);
    
    if(fullscreen) {
        unhide_cursor();
    }
    
    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
      {
        char *f;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        f= gtk_file_chooser_get_filename (chooser);
        
        load_folder(f);
        
        g_free (f);
      }
    gtk_widget_destroy (dialog);
    
    if(fullscreen) {
        hide_cursor();
    }
}

void load_next()
{
    if (folder != NULL && folder_i+1 < folder_max) {
        zoom_reset();
        load_image(folder[++folder_i]);
    } else {
        choose_folder();
    }
}

void zoom_in()
{
    zoom+=0.1;
    reload_image();
}

void zoom_out()
{
    zoom-=0.1;
    reload_image();
}

void zoom_reset()
{
    zoom=1.0;
    reload_image();
}


void load_folder(char *f)
{
    load_folder_file(f, NULL);
}

//~ gboolean folderdialog_buttonOK_clicked (GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
//~ {
//~ 
    //~ char *f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(folderdialog));
    //~ load_folder(f);
    //~ gtk_widget_set_visible(folderdialog, FALSE);
    //~ return TRUE;
//~ }

//~ gboolean folderdialog_buttonCancel_clicked (GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
//~ {
    //~ gtk_widget_set_visible(folderdialog, FALSE);
    //~ return TRUE;
//~ }

void load_image_null()
{
    GError *e = NULL;   // used to handle errors
    GdkPixbuf *pixbuf;
    int w,h;
    
    w=gtk_widget_get_allocated_width(GTK_WIDGET(imageMain));
    h=gtk_widget_get_allocated_height(GTK_WIDGET(imageMain));
    
    pixbuf=gdk_pixbuf_new_from_resource_at_scale ("/tk/lmureu/squirrer/img/squirrer.png", w, h, TRUE, &e);
    if(pixbuf!=NULL) {
        gtk_image_set_from_pixbuf(imageMain, pixbuf);
    } else {
        puts(e->message);
        g_error_free(e);
    }
}

void load_image(const char *filename)
{
    GError *e = NULL;   // used to handle errors
    GdkPixbuf *pixbuf;  // GdkPixbuf which represent the image
    int width,          // image's original width
        height,         // image's original heigth
        swidth,         // actual width of the GtkImage
        wscale;         // image's scaled width

    // get image size
    gdk_pixbuf_get_file_info (filename, &width, &height);
    // get GtkImage object
    //    GtkImage *image = GTK_IMAGE(gtk_builder_get_object(builder, "imageMain"));
    // get GtkImage width
    swidth= gtk_widget_get_allocated_width(GTK_WIDGET(imageMain));
    // calculate wscale
    wscale = (width > swidth) ? swidth : width;
    //printf("wscale (non zoomed): %d\n", wscale);
    //printf("zoom: %f\n", zoom);
    wscale = (int)(wscale*zoom);
    //printf("wscale (zoomed): %d\n\n", wscale);
    // create scaled pixbuf 
    pixbuf=gdk_pixbuf_new_from_file_at_size (filename, wscale , -1, &e); 
    
    /*
     * DEBUG:
     * printf("Image size: %d\nWidget size: %d\nPixbuf computed size: %d\n\n",width,swidth,q);
     */
    
    if (pixbuf != NULL) {
        gtk_image_set_from_pixbuf(imageMain, pixbuf);
        vscroll_top();
        update_label();
    }else{
        puts(e->message);
        g_error_free(e);
    }
}

void load_folder_file(const char *folderpath, const char *filename)
{
    load_image_null();
    
    if(chdir(folderpath)==0){
        folder_free();
        folder= list_images(folderpath, &folder_max);
        
        if( filename == NULL && folder_max >= 1) {
            zoom_reset();
            folder_i=0;
            load_image(folder[folder_i]);
            return;
        }
        
        for (folder_i=0; folder_i<folder_max; folder_i++)
        {
            if (strcmp(filename,folder[folder_i]) == 0) {
                zoom_reset();
                load_image(folder[folder_i]);
                break;
            }
        }
    }
}

int main (int argc, char *argv[])
{
    // initialize gtk
    gtk_init (&argc, &argv); 
    // load interface
    builder=gtk_builder_new_from_resource("/tk/lmureu/squirrer/squirrer.glade");
    // bind signals handlers
    gtk_builder_connect_signals(builder,NULL);
    // make some widely-used widgets globally visible
    windowMain=GTK_WIDGET(gtk_builder_get_object(builder, "windowMain"));
    labelStatus = GTK_LABEL(gtk_builder_get_object(builder, "labelStatus"));
    imageMain = GTK_IMAGE( gtk_builder_get_object(builder, "imageMain"));
    
    load_image_null();
    // eventually load directory given as argv[1]
    if (argc == 2 ) {
        
        if (is_directory(argv[0])) {
            load_folder(argv[0]);
        } else {
            char *d = dirname(argv[1]);
            char *f = basename(argv[1]);
            load_folder_file(d,f);
        }
    }
    // main loop
    gtk_main ();
    return 0;
}


/*
 * TODO:
 * 1) on window resize: reload image
 * 2) on end of folder reached (both ways) change directory (++ or dialog?) [done]
 * 4) add status (file # / total files) like IrfanView [done]
 */
