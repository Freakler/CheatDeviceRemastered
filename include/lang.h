/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2026, Freakler
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

#ifndef __LANG_H__
#define __LANG_H__

#define LANG_DEBUG

#define TABLE_SIZE 823 // Prime number for hash table size
#define LANG_FILES_LIMIT 8
#define TRANSLATED_STRINGS_LIMIT TABLE_SIZE
#define FILE_SIZE_LIMIT 0xFFFF
#define MURMURMASH_3_SEED 0x947473


#define LOC_STRING_MAX_SIZE 124

typedef struct string_lang
{
  struct string_lang *next; // Pointer to the next node (for chaining)
  char original_string[LOC_STRING_MAX_SIZE];
  char trans_string[LOC_STRING_MAX_SIZE];
} string_lang;

typedef struct LangHashTable
{
  string_lang *table[TABLE_SIZE]; // Array of pointers to string_lang nodes
} LangHashTable;

void langTableFree(LangHashTable* ht);

typedef struct
{
  char lang_name[32];
  char author_name[16];
  char version[16];
  char path[128];
} LanguageFile;

typedef struct
{
  LanguageFile *lang_files[LANG_FILES_LIMIT];
  int size;
} LangFileTable;

char *_t(const char *string);

void langTableSetup(int langIndex);
void langTableUpdate(int langIndex);

void langFileTableFree(LangFileTable *table);

void GetLangINIInfo(LangFileTable *table, const char *filename);

extern LangHashTable *main_lang_table;
extern LangFileTable *main_file_table;

extern int currLanguageID;

#endif