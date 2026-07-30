/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2025, Freakler
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

#ifndef __PUTILS_H__
#define __PUTILS_H__

#include <psptypes.h>
#include <pspkerneltypes.h>

extern int doesFileExist(const char* path);
extern int doesDirExist(const char* path);

extern int countFilesInFolder(const char *path);
extern int countFoldersInFolder(const char *path);

extern int setFolderModificationDateNow(const char* folder);
extern char *getFolderModificationDate(const char* folder, char *out, size_t size);

extern int adrenalineCheck();

extern void clearICacheFor(u32 address);

extern void writeShort(uint8_t *address, short value);
extern void writeInteger(uint8_t *address, int value);
extern void writeFloat(uint8_t *address, float value);

extern char *_fgets(char *s, int size, SceUID stream);

extern void makedirs(const char *path);
extern int getHighMemBound();

extern int checkCoordinateInsideArea(float a, float b, float c, float x, float y, float z, float radius);
extern float distanceBetweenCoordinates3d(float x1, float y1, float z1, float x2, float y2, float z2);

extern void getSizeString(char *string, uint64_t size);

extern int fileEndsWithExtension(const char *path, const char* extension);
extern int getExtensionLength(const char* path);

extern uint32_t hash(const char *key, uint32_t len, uint32_t seed);

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof(x[0]) )

#endif /* __PUTILS_H__ */