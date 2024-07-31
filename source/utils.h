/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2023, Freakler
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UTILSS_H__
#define __UTILSS_H__


int logPrintf(const char *text, ...);

int doesFileExist(const char* path);
int doesDirExist(const char* path);

int countFilesInFolder(const char *path);
int countFoldersInFolder(const char *path);

int setFolderModificationDateNow(const char* folder);
char *getFolderModificationDate(const char* folder);

int adrenalineCheck();

void clearICacheFor(u32 address);

void writeShort(uint8_t *address, short value);
void writeInteger(uint8_t *address, int value);
void writeFloat(uint8_t *address, float value);

char *strtok_r(char *s, const char *delim, char **save_ptr);
char *_fgets(char *s, int size, SceUID stream);

void makedirs(char *path);
int getHighMemBound();

int checkCoordinateInsideArea(float a, float b, float c, float x, float y, float z, float radius);
float distanceBetweenCoordinates3d(float x1, float y1, float z1, float x2, float y2, float z2);

void getSizeString(char string[16], uint64_t size);

int fileEndsWithExtension(char *filename, const char* extension);

#endif
