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
#include <assert.h>
#include "helpdialog.h"
#include "squirrer.h"

GtkWidget *helpdialog;

gboolean on_dialogHelp_buttonClose_clicked(GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
{
	assert(helpdialog != NULL);
	gtk_widget_hide(helpdialog);
	
	return TRUE;
}

void helpdialog_show(void) {
	assert(builder != NULL);
	helpdialog = GTK_WIDGET(gtk_builder_get_object(builder, "dialogHelp"));
	assert(helpdialog != NULL);
	gtk_widget_show(helpdialog);
}
