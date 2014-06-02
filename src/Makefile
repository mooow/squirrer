#~ 
#~     squirrer - a simple image viewer for reading comics
#~     Copyright (C) 2014 Lorenzo Mureu
#~ 
#~     This file is part of squirrer.
#~  
#~ 	squirrer is free software: you can redistribute it and/or modify
#~ 	it under the terms of the GNU General Public License as published by
#~ 	the Free Software Foundation, either version 3 of the License, or
#~ 	(at your option) any later version.
#~ 
#~ 	squirrer is distributed in the hope that it will be useful,
#~ 	but WITHOUT ANY WARRANTY; without even the implied warranty of
#~ 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#~ 	GNU General Public License for more details.
#~ 
#~ 	You should have received a copy of the GNU General Public License
#~ 	along with squirrer.  If not, see <http://www.gnu.org/licenses/>.
#~ 

CC=gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -std=gnu99  -g -Wall -Werror -Wpedantic -Wl,--export-dynamic 
CLIBS = `pkg-config --libs gtk+-3.0`
program = squirrer
objects = main.o listdir.o futils.o helpdialog.o gresources.o

all: $(objects)
	$(CC) $(CFLAGS) -o $(program) $(CLIBS) $(objects)
gresources.c: resources.gresource.xml squirrer.glade
	glib-compile-resources --generate-source --target=gresources.c resources.gresource.xml
%.o:  %.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean :
	rm -f $(program) $(objects)

