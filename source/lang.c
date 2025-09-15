/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-2024, Freakler
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

#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "minIni.h"
#include "main.h"
#include "lang.h"
#include <pspiofilemgr.h>
#include "utils.h"

#ifdef LANG

extern char folder_translations[];
extern const char *basefolder;

static int translated_strings_left = TRANSLATED_STRINGS_LIMIT;

LangHashTable *create_lang_table() {
    LangHashTable *ht = (LangHashTable *)malloc(sizeof(LangHashTable));
    if (!ht) {
        return NULL;
    }
    memset(ht->table, 0, sizeof(string_lang *) * TABLE_SIZE); // Initialize table to NULL
    return ht;
}

// MurmurHash3 hash function
static uint32_t hash(const char *key, uint32_t len, uint32_t seed) {
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n = 0xe6546b64;
    uint32_t h = seed;
    uint32_t k = 0;
    const uint8_t *d = (const uint8_t *)key;
    const uint32_t *chunks = (const uint32_t *)(d);
    const uint8_t *tail = (const uint8_t *)(d + (len / 4) * 4);

    int l = len / 4;

    // Process the body (4-byte chunks)
    int i;
    for (i = 0; i < l; ++i) {
        k = chunks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;
    }

    k = 0;

    // Process the tail (remaining bytes)
    switch (len & 3) {
        case 3: k ^= (tail[2] << 16);
        case 2: k ^= (tail[1] << 8);
        case 1: k ^= tail[0];
                k *= c1;
                k = (k << r1) | (k >> (32 - r1));
                k *= c2;
                h ^= k;
    }

    h ^= len;
    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);

    return h % TABLE_SIZE;
}

void lang_table_insert(LangHashTable *ht, const char *original_string, const char *trans_string) {
    uint32_t index = hash(original_string, strlen(original_string), MURMURMASH_3_SEED);
    
    string_lang *new_kv = (string_lang *)malloc(sizeof(string_lang));
    if (!new_kv) {
        return;
    }

    new_kv->original_string = strdup(original_string);
    new_kv->trans_string = strdup(trans_string);
    new_kv->next = ht->table[index]; // Point to the current list at index
    ht->table[index] = new_kv; // Insert new_kv at the beginning
}

char *lang_table_search(LangHashTable *ht, const char *original_string) {

    if (ht == NULL || main_file_table == NULL || main_file_table->size==1) goto RET_ORIGINAL_STRING;

    uint32_t index = hash(original_string, strlen(original_string), MURMURMASH_3_SEED);
    string_lang *current = ht->table[index];
    
    while (current) {
        if (strcmp(current->original_string, original_string) == 0) {
            return current->trans_string;
        }
        current = current->next;
    }

    RET_ORIGINAL_STRING:
    // If not found, return the original string
    return (char*)original_string;
}

void free_table(LangHashTable *ht) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        string_lang *current = ht->table[i];
        while (current) {
            string_lang *tmp = current;
            current = current->next;
            free(tmp->original_string);
            free(tmp->trans_string);
            free(tmp);
        }
    }
    free(ht);
}

LangHashTable *main_lang_table;
LangFileTable *main_file_table;

// Initialize the LangFileTable
LangFileTable *initLangFileTable() {
    LangFileTable *table = (LangFileTable *)malloc(sizeof(LangFileTable));
    if (!table) {
        return NULL;
    }
    int i;
    for (i = 0; i < LANG_FILES_LIMIT; i++) {
        table->lang_files[i] = NULL; // Initialize all pointers to NULL
    }

    table->size = 0;
    return table;
}

int CurrentLanguageID = 0;

// Append a LanguageFile to the LangFileTable
void LangFileAppend(LangFileTable *table, const char *version, const char *author, const char *language, const char *filename) {
    if (table->size >= LANG_FILES_LIMIT) {
        return;
    }

    // Allocate memory for new LanguageFile
    LanguageFile *new_lf = (LanguageFile *)malloc(sizeof(LanguageFile));
    if (!new_lf) {
        return;
    }

    // Duplicate strings and assign to new LanguageFile
    new_lf->Language = strdup(language);
    new_lf->Author = strdup(author);
    new_lf->Version = strdup(version);
    new_lf->FileName = strdup(filename);

    // Append new LanguageFile to table
    table->lang_files[table->size] = new_lf;
    table->size++;
}

// Free memory allocated for LangFileTable
void freeLangFileTable(LangFileTable *table) {
    if (!table) return;
    int i;
    for (i = 0; i < table->size; i++) {
        free(table->lang_files[i]->Language);
        free(table->lang_files[i]->Author);
        free(table->lang_files[i]->Version);
        free(table->lang_files[i]->FileName);
        free(table->lang_files[i]);
    }

    free(table);
}

LangFileTable *SearchLangFiles() {
    main_file_table = initLangFileTable();
    if (!main_file_table) {
        return NULL;
    }

    // Create an english default 
    LangFileAppend(main_file_table, VERSION, "Freakler", "English (United States)", "");

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%s%s", basefolder, folder_translations);
    SceUID dir = sceIoDopen(buffer);
    if (dir < 0) {
        return main_file_table;
    }

    SceIoDirent dirent;

    // Initialize dirent
    memset(&dirent, 0, sizeof(SceIoDirent));

    while (sceIoDread(dir, &dirent) > 0) {
        if (FIO_SO_ISREG(dirent.d_stat.st_attr)) {
            // Suffix to ignore an .ini file (add "_ignore.ini" to the filename)
            if (fileEndsWithExtension(dirent.d_name, "_ignore.ini")) continue;
            if (fileEndsWithExtension(dirent.d_name, ".ini") && strcmp(dirent.d_name, "sample.ini") != 0) {
                GetINIInfo(main_file_table, dirent.d_name);
            }
        }

        // Clear dirent
        memset(&dirent, 0, sizeof(SceIoDirent));
    }

    sceIoDclose(dir);

    return main_file_table;
}

void GetINIInfo(LangFileTable *table, const char *filename) {
    char Version[8];
    char Author[32];
    char Language[32];
    char filepath[256];
	
    snprintf(filepath, sizeof(filepath), "%s%s%s", basefolder, folder_translations, filename);

    ini_gets("INFO", "Translate Version", "None", Version, sizeof(Version), filepath);
    ini_gets("INFO", "Translate Author", "None", Author, sizeof(Author), filepath);
    ini_gets("INFO", "Translate Language", "None", Language, sizeof(Language), filepath);

#if defined(LOG) && defined(LANG_DEBUG)
    logPrintf("Info from file '%s': Version '%s', Author '%s', Language '%s'", filename, Version, Author, Language);
#endif

    LangFileAppend(table, Version, Author, Language, filename);
}

#if defined(LOG) && defined(LANG_DEBUG)
u64 curr_time, after_time;
char bufDebug[64];
#endif

void ReadTranslationsFromINI(LangHashTable *table, const char* INISection, int index) 
{
    char fileread[FILE_SIZE_LIMIT];
    char original_string[256];
    char translated_string[256];
    char lang_path[128];
    SceUID fp;

#if defined(LOG) && defined(LANG_DEBUG)
    curr_time = sceKernelGetSystemTimeWide();
#endif

    snprintf(lang_path, sizeof(lang_path), "%s%s%s", basefolder, folder_translations, main_file_table->lang_files[index]->FileName);

#if defined(LOG) && defined(LANG_DEBUG)
    logPrintf("Reading Section '%s' from INI file '%s'", INISection, lang_path);
#endif

    if (!ini_openread(lang_path, &fp)) {
        return;
    }
    
    if (!sceIoRead(fp, &fileread, sizeof(fileread))) {
        goto CLOSE_INI;
    }

    int i;
    int temp_trans_strings_left = translated_strings_left;
    for (i = 0; i < temp_trans_strings_left; i++) {
        // Get original string (key) from index
        if (ini_getkeyfromstring(INISection, i, original_string, sizeof(original_string), fileread) == 0)
            break;

        // Get (from key) the translation
        ini_getsfromstring(INISection, original_string, original_string, translated_string, sizeof(translated_string), fileread);

        // Don't load any english or empty string (it defaults to english)
        if (strcmp(original_string, translated_string) == 0 || strlen(translated_string) == 0) {
            continue;
        }
        
        // Insert on table
        lang_table_insert(table, original_string, translated_string);
        translated_strings_left--;
    }

    CLOSE_INI:
    ini_close(&fp);

#if defined(LOG) && defined(LANG_DEBUG)
    after_time = sceKernelGetSystemTimeWide();
    logPrintf("Translated Strings Left -> %d", translated_strings_left);
    logPrintf("Time needed for read -> %.2f seconds", (float)(after_time-curr_time)/1000000.0f);
#endif
}

void update_lang(int langIndex) {
    // Avoid mem leaks
    translated_strings_left = TRANSLATED_STRINGS_LIMIT;
    if (main_lang_table != NULL) {
        free_table(main_lang_table);
    }

    if (main_file_table != NULL) {
        freeLangFileTable(main_file_table);
    }
    setup_lang(langIndex);
}

void setup_lang(int langIndex) {

    main_lang_table = create_lang_table();
    main_file_table = SearchLangFiles();

    // If language is english
    if (langIndex == 0) {
        CurrentLanguageID = langIndex;
        return;
    } 

    if (main_lang_table == NULL || main_file_table == NULL || main_file_table->size==1) return;

    ReadTranslationsFromINI(main_lang_table, "GENERAL", langIndex);

    if (LCS)
        ReadTranslationsFromINI(main_lang_table, "LCS", langIndex);
    else
        ReadTranslationsFromINI(main_lang_table, "VCS", langIndex);

    CurrentLanguageID = langIndex;
}

#endif

char* t_string(const char* string) {
    #ifdef LANG
    if (CurrentLanguageID != 0)
        return lang_table_search(main_lang_table, string);
    #endif
    return (char*)string;
}
