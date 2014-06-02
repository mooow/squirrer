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

#include <sys/types.h>              // this defines POSIX types
#include <sys/stat.h>               // this defines stat
#include <fcntl.h>                  // POSIX file control constants
#include <string.h>                 // utilities for strings (rindex)
#include <stdlib.h> 				// malloc

int is_directory(const char *filename)
{
	struct stat buffer;
	int         status;
	status = stat(filename, &buffer);
	if (status == 0){
		return S_ISDIR(buffer.st_mode);
	} else {
		return 0;
	}
}

char * basename(char *path)
{
	char *ptr =  strrchr(path, '/');
	if (ptr != NULL) {
		return &ptr[1];
	}
	return path;
}

char * dirname(char *path)
{
	if(is_directory(path)) {
		return path;
	} else {
		char *tmp = malloc(strlen(path)*(sizeof (char)));
		strcpy(tmp,path);
		char *ptr = rindex(tmp, '/');
		ptr[1]='\0';
		return dirname(tmp);
	}
}
