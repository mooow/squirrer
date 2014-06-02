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

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listdir.h"

#define TRUE 1
#define FALSE 0

int filter_images(const struct dirent *entry)
{
	if ( strcmp(entry->d_name, ".") == 0 || 
			strcmp(entry->d_name, "..") == 0)
	{
		return 0;
	}
	
	char *ptr = strrchr(entry->d_name, '.');
	if (ptr != NULL && (
		strcmp(ptr, ".png")==0 ||
		strcmp(ptr, ".PNG")==0 ||
		strcmp(ptr, ".bmp")==0 ||
		strcmp(ptr, ".BMP")==0 ||
		strcmp(ptr, ".jpg")==0 ||
		strcmp(ptr, ".JPG")==0 ))
	{
		return 1;
	}
	
	return 0;
}

char ** list_images(const char *path, int *count)
{
	int i;
	struct dirent **d;
	
	//int alphasort();
	
	*count = scandir(path, &d, filter_images, alphasort);
	
	if (*count <= 0) {
		//puts("no files");
		return NULL;
	}
	
	char **ptr = malloc(sizeof(char *) * *count);
	
	for (i =0; i < *count; i++ )
	{
		ptr[i]=malloc(strlen(d[i]->d_name)+1);
		strcpy(ptr[i], d[i]->d_name);
	}
	
	return ptr;
}
