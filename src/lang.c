/*
 *  CheatDevice Remastered
 *  Copyright (C) 2017-, Freakler
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "lang.h"
#include <pspiofilemgr.h>
#include <pspsysmem.h>
#include <malloc.h>
#include "putils.h"
#include "logs.h"
#include "minIni.h"

#ifdef LANG

extern u8string folder_translations;
extern const u8string basefolder;

static LangHashTable *langTableCreate()
{
  LangHashTable *ht = (LangHashTable *)malloc(sizeof(LangHashTable));
  if ( !ht ) return NULL;

  // Initialize table to NULL
  memset(ht->table, 0, sizeof(ht->table));
  return ht;
}

static func_stat langTableInsert(LangHashTable *ht, const u8string original_string, const u8string trans_string)
{
  if ( !ht || !main_file_table || !original_string || !trans_string ) return FUNC_ERROR;

  uint32_t index = hash(original_string, strlen(original_string), MURMURMASH_3_SEED) % TABLE_SIZE;

  string_lang *new_kv = (string_lang *)malloc(sizeof(string_lang));
  if ( !new_kv ) return FUNC_ERROR;

  new_kv->original_string = strdup(original_string);
  if ( !new_kv->original_string )
  {
    free(new_kv);
    return FUNC_ERROR;
  }

  new_kv->trans_string = strdup(trans_string);
  if ( !new_kv->trans_string )
  {
    free(new_kv->original_string);
    free(new_kv);
    return FUNC_ERROR;
  }

  new_kv->next = ht->table[index]; // Point to the current list at index
  ht->table[index] = new_kv; // Insert new_kv at the beginning

  return FUNC_OK;
}

static u8string langTableSearch(LangHashTable *ht, const u8string original_string)
{
  if ( !ht || !original_string ) return (u8string)original_string;

  array_uidx index = (array_uidx)(hash(original_string, strlen(original_string), MURMURMASH_3_SEED) % TABLE_SIZE);
  
  string_lang *current = ht->table[index];

  while (current) {
    if ( !strcmp(current->original_string, original_string) )
      return current->trans_string;

    current = current->next;
  }

  // If not found, return the original string
  return (u8string)original_string;
}

void langTableFree(LangHashTable *ht)
{
  if ( !ht ) return;

  for (array_uidx i = 0; i < TABLE_SIZE; i++)
  {
    string_lang *current = ht->table[i];
    while (current)
    {
      string_lang *tmp = current;
      current = current->next;
      free(tmp->original_string);
      free(tmp->trans_string);
      free(tmp);
    }
  }
  free(ht);
  ht = NULL;

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Language table has been freed!");
  #endif
}

LangHashTable *main_lang_table = NULL;
LangFileTable *main_file_table = NULL;

// Initialize the LangFileTable
static LangFileTable *langFileTableInit()
{
  LangFileTable *table = (LangFileTable *)malloc(sizeof(LangFileTable));
  if ( !table ) return NULL;

  memset(table, 0, sizeof(LangFileTable));
  return table;
}

LangID currLanguageID = LANG_ENGLISH;

// Append a LanguageFile to the LangFileTable
static void langFileTableAppend(LangFileTable *table, const u8string version, const u8string author, const u8string language, const u8string filename)
{
  if ( !table || !filename || table->size >= LANG_FILES_LIMIT ) return;

  LanguageFile *new_lf = NULL;

  new_lf = (LanguageFile *)malloc(sizeof(LanguageFile));
  if ( !new_lf ) return;

  new_lf->version = strdup(version);
  if ( !new_lf->version )
  {
    free(new_lf);
    return;
  }

  new_lf->author_name = strdup(author);
  if ( !new_lf->author_name )
  {
    free(new_lf->version);
    free(new_lf);
    return;
  }

  new_lf->lang_name = strdup(language);
  if ( !new_lf->lang_name )
  {
    free(new_lf->author_name);
    free(new_lf->version);
    free(new_lf);
    return;
  }

  new_lf->path = strdup(filename);
  if ( !new_lf->path )
  {
    free(new_lf->lang_name);
    free(new_lf->author_name);
    free(new_lf->version);
    free(new_lf);
    return;
  }

  DEBUG_LOG("Adding '%s' (%s)", filename, language);

  // Append new LanguageFile to table
  table->lang_files[table->size] = new_lf;
  table->size++;
}

// Free memory allocated for LangFileTable
void langFileTableFree(LangFileTable *table)
{
  if ( !table ) return;

  for (array_uidx i = 0; i < table->size; i++)
  {
    LanguageFile *currLangFile = table->lang_files[i];
    if ( !currLangFile ) continue;

    free(currLangFile->author_name);
    free(currLangFile->lang_name);
    free(currLangFile->path);
    free(currLangFile->version);
    free(currLangFile);
  }

  free(table);
  table = NULL;

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Language file table has been freed!");
  #endif
}

static void langFileTableSearch()
{
  main_file_table = langFileTableInit();
  if ( !main_file_table ) return;

  // Create an english default
  langFileTableAppend(main_file_table, VERSION, "Freakler", "English (United States)", "");

  // Open Directory
  u8char buffer[128];
  snprintf(buffer, sizeof(buffer), "%s%s", basefolder, folder_translations);
  SceUID dir = sceIoDopen(buffer);
  if ( dir < 0 ) return;

  SceIoDirent dirent;

  // Initialize dirent
  memset(&dirent, 0, sizeof(SceIoDirent));

  while ( sceIoDread(dir, &dirent) > 0 )
  {
    if ( FIO_SO_ISREG(dirent.d_stat.st_attr) )
    {
      // Suffix to ignore an .ini file (add "_ignore.ini" to the filename)
      if ( stringEndsWith(dirent.d_name, "_ignore.ini") ) continue;

      // If file is an .ini and is the 'sample.ini' file, get info from it (to add to menu selections)
      if ( stringEndsWith(dirent.d_name, ".ini") && strcasecmp(dirent.d_name, "sample.ini") != 0 )
        GetLangINIInfo(main_file_table, dirent.d_name);
    }

    // Clear dirent
    memset(&dirent, 0, sizeof(SceIoDirent));
  }

  sceIoDclose(dir);

  return;
}

void GetLangINIInfo(LangFileTable *table, const u8string filename)
{
  if ( !table || !filename ) return;

  u8char version[8];
  u8char author[32];
  u8char lang[32];
  u8char path[256];

  snprintf(path, sizeof(path), "%s%s%s", basefolder, folder_translations, filename);

  ini_gets("INFO", "Translate Version", VERSION, version, sizeof(version), path);
  ini_gets("INFO", "Translate Author", "unknown", author, sizeof(author), path);
  ini_gets("INFO", "Translate Language", "unknown", lang, sizeof(lang), path);

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Info from file '%s': Version '%s', Author '%s', Language '%s'", filename, version, author, lang);
  #endif

  langFileTableAppend(table, version, author, lang, filename);
}

static usize translatedStringsLength = 0;
static int ReadTranslationEntry(const char *section, const char *key, const char *value, void *user_data)
{
  if ( !section || !key || !value || !user_data ) return 0; // Invalid args

  TranslationBrowseData *data = (TranslationBrowseData *)user_data;
  if ( translatedStringsLength >= TRANSLATED_STRINGS_LIMIT ) return 0; // We've reached the limit!
  if ( data->section[0] != section[0] ) return 1; // Not needed
  
  // Empty or english string
  if ( STR_EMPTY(value) || !strcmp(key, value) ) return 1;
  
  langTableInsert(data->table, (const u8string)key, (const u8string)value);
  translatedStringsLength++;
  return 1;
}

static void ReadTranslationsFromINI(LangHashTable *table, const u8string INISection, array_uidx index)
{
  if ( !table || !main_file_table || !INISection || *INISection == '\0' || index >= main_file_table->size ) return;

  #ifdef LANG_DEBUG
  SceInt64 curr_time = sceKernelGetSystemTimeWide();
  (void)curr_time; // Dumb compiler
  #endif
  
  u8char lang_path[128];
  snprintf(lang_path, sizeof(lang_path), "%s%s%s", basefolder, folder_translations, main_file_table->lang_files[index]->path);

  #ifdef LANG_DEBUG
  DEBUG_LOG("Reading Section '%s' from INI file '%s'", INISection, lang_path);
  #endif

  ini_browse(ReadTranslationEntry, &(TranslationBrowseData){ 
      .table = table,
      .section = INISection,
    },
    lang_path
  );

  #ifdef LANG_DEBUG
  DEBUG_LOG("Num of Translated Strings -> %u", translatedStringsLength);
  DEBUG_LOG("Time needed for read -> %.2f seconds", (float)(sceKernelGetSystemTimeWide() - curr_time) / 1000000.0f);
  #endif
}

void langTableUpdate(LangID langID)
{
  translatedStringsLength = 0;

  if (main_lang_table) {
    // Avoid mem leaks
    langTableFree(main_lang_table);
    main_lang_table = NULL;
  }

  if (main_file_table) {
    // Avoid mem leaks
    langFileTableFree(main_file_table);
    main_file_table = NULL;
  }

  langTableSetup(langID);
}

void langTableSetup(LangID langID)
{
  currLanguageID = langID;

  main_lang_table = langTableCreate();
  if ( !main_lang_table ) return;

  langFileTableSearch();
  if ( !main_file_table || main_file_table->size == 1 ) return;

  // If current language is English (lang and file tables still need to be created)
  if ( langID == LANG_ENGLISH ) return;

  ReadTranslationsFromINI(main_lang_table, "GENERAL", langID);
  ReadTranslationsFromINI(main_lang_table, LCS ? "LCS" : "VCS", langID);
}

#endif

// Get translated string from .ini file
u8string _t(const u8string string)
{
  #ifdef LANG
  if ( currLanguageID != 0 ) return langTableSearch(main_lang_table, string);
  #endif

  return (u8string)string;
}