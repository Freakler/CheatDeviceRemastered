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

#include <pspkernel.h>
#include <pspdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minIni.h"
#include "main.h"
#include "lang.h"
#include <pspiofilemgr.h>
#include <pspsysmem.h>
#include "utils.h"
#include "logs.h"

#ifdef LANG

extern char folder_translations[];
extern const char *basefolder;

static int translated_strings_left = TRANSLATED_STRINGS_LIMIT;

SceUID langVpl = -1;

static LangHashTable *langTableCreate()
{
  LangHashTable *ht;
  int vplRet = sceKernelTryAllocateVpl(langVpl, sizeof(LangHashTable), (void**)&ht);

  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", sizeof(LangHashTable), vplRet);
    return NULL;
  }

  // Initialize table to NULL
  memset(ht->table, 0, sizeof(ht->table));
  return ht;
}

static void langTableInsert(LangHashTable *ht, const char *original_string, const char *trans_string)
{
  if ( !ht || !main_file_table || !original_string || !trans_string ) return;

  uint32_t index = hash(original_string, strlen(original_string), MURMURMASH_3_SEED) & TABLE_SIZE;

  string_lang *new_kv = NULL;
  int vplRet = sceKernelTryAllocateVpl(langVpl, sizeof(string_lang), (void**)&new_kv);

  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", sizeof(string_lang), vplRet);
    return;
  }

  SceSize ostring_len = strlen(original_string);
  SceSize tstring_len = strlen(trans_string);

  vplRet = sceKernelTryAllocateVpl(langVpl, ostring_len + 1, (void**)&new_kv->original_string);

  if ( vplRet < 0 )
  {
    ERROR_LOG("v(%u) failed with error 0x%08X", ostring_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_kv);
    return;
  }

  vplRet = sceKernelTryAllocateVpl(langVpl, tstring_len + 1, (void**)&new_kv->trans_string);

  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", tstring_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_kv->original_string);
    sceKernelFreeVpl(langVpl, new_kv);
    return;
  }

  strncpy(new_kv->original_string, original_string, ostring_len);
  new_kv->original_string[ostring_len] = '\0';

  strncpy(new_kv->trans_string, trans_string, tstring_len);
  new_kv->trans_string[tstring_len] = '\0';

  new_kv->next = ht->table[index]; // Point to the current list at index
  ht->table[index] = new_kv; // Insert new_kv at the beginning
}

static char *langTableSearch(LangHashTable *ht, const char *original_string)
{
  if ( !ht || !original_string ) return (char *)original_string;

  uint32_t index = hash(original_string, strlen(original_string), MURMURMASH_3_SEED) & TABLE_SIZE;
  string_lang *current = ht->table[index];

  while (current) {
    if ( !strcmp(current->original_string, original_string) )
      return current->trans_string;

    current = current->next;
  }

  // If not found, return the original string
  return (char *)original_string;
}

void langTableFree(LangHashTable *ht)
{
  if ( !ht ) return;

  int i;
  for (i = 0; i < TABLE_SIZE; i++)
  {
    string_lang *current = ht->table[i];
    while (current)
    {
      string_lang *tmp = current;
      current = current->next;
      sceKernelFreeVpl(langVpl, tmp->original_string);
      sceKernelFreeVpl(langVpl, tmp->trans_string);
      sceKernelFreeVpl(langVpl, tmp);
    }
  }
  sceKernelFreeVpl(langVpl, ht);
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
  LangFileTable *table = NULL;
  int vplRet = sceKernelTryAllocateVpl(langVpl, sizeof(LangFileTable), (void**)&table);

  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", sizeof(LangFileTable), vplRet);
    return NULL;
  }

  memset(table, 0, sizeof(LangFileTable));
  return table;
}

int currLanguageID = 0;

// Append a LanguageFile to the LangFileTable
static void langFileTableAppend(LangFileTable *table, const char *version, const char *author, const char *language, const char *filename)
{
  if ( !table || !filename || table->size >= LANG_FILES_LIMIT ) return;

  SceSize version_len = strlen(version);
  SceSize author_len = strlen(author);
  SceSize language_len = strlen(language);
  SceSize filename_len = strlen(filename);

  LanguageFile *new_lf = NULL;

  int vplRet = sceKernelTryAllocateVpl(langVpl, sizeof(LanguageFile), (void**)&new_lf);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", sizeof(LanguageFile), vplRet);
    return;
  }

  vplRet = sceKernelTryAllocateVpl(langVpl, version_len + 1, (void**)&new_lf->version);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", version_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_lf);
    return;
  }

  vplRet = sceKernelTryAllocateVpl(langVpl, author_len + 1, (void**)&new_lf->author_name);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", author_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_lf);
    sceKernelFreeVpl(langVpl, new_lf->version);
    return;
  }

  vplRet = sceKernelTryAllocateVpl(langVpl, language_len + 1, (void**)&new_lf->lang_name);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", language_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_lf);
    sceKernelFreeVpl(langVpl, new_lf->version);
    sceKernelFreeVpl(langVpl, new_lf->author_name);
    return;
  }

  vplRet = sceKernelTryAllocateVpl(langVpl, filename_len + 1, (void**)&new_lf->path);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", filename_len + 1, vplRet);
    sceKernelFreeVpl(langVpl, new_lf);
    sceKernelFreeVpl(langVpl, new_lf->version);
    sceKernelFreeVpl(langVpl, new_lf->author_name);
    sceKernelFreeVpl(langVpl, new_lf->lang_name);
    return;
  }

  strncpy(new_lf->lang_name, language, language_len);
  new_lf->lang_name[language_len] = '\0';

  strncpy(new_lf->author_name, author, author_len);
  new_lf->author_name[author_len] = '\0';

  strncpy(new_lf->version, version, version_len);
  new_lf->version[version_len] = '\0';

  strncpy(new_lf->path, filename, filename_len);
  new_lf->path[filename_len] = '\0';

  // Append new LanguageFile to table
  table->lang_files[table->size] = new_lf;
  table->size++;
}

// Free memory allocated for LangFileTable
void langFileTableFree(LangFileTable *table)
{
  if ( !table ) return;

  int i;
  for (i = 0; i < table->size; i++)
  {
    LanguageFile *currLangFile = table->lang_files[i];
    if ( !currLangFile ) continue;

    sceKernelFreeVpl(langVpl, currLangFile->author_name);
    sceKernelFreeVpl(langVpl, currLangFile->lang_name);
    sceKernelFreeVpl(langVpl, currLangFile->path);
    sceKernelFreeVpl(langVpl, currLangFile->version);
    sceKernelFreeVpl(langVpl, currLangFile);
  }

  sceKernelFreeVpl(langVpl, table);
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
  char buffer[128];
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
      if ( fileEndsWithExtension(dirent.d_name, "_ignore.ini") ) continue;

      // If file is an .ini and is the 'sample.ini' file, get info from it (to add to menu selections)
      if ( fileEndsWithExtension(dirent.d_name, ".ini") && strcasecmp(dirent.d_name, "sample.ini") != 0 )
        GetLangINIInfo(main_file_table, dirent.d_name);
    }

    // Clear dirent
    memset(&dirent, 0, sizeof(SceIoDirent));
  }

  sceIoDclose(dir);

  return;
}

void GetLangINIInfo(LangFileTable *table, const char *filename)
{
  if ( !table || !filename ) return;

  char version[8];
  char author[32];
  char lang[32];
  char path[256];

  snprintf(path, sizeof(path), "%s%s%s", basefolder, folder_translations, filename);

  ini_gets("INFO", "Translate Version", VERSION, version, sizeof(version), path);
  ini_gets("INFO", "Translate Author", "unknown", author, sizeof(author), path);
  ini_gets("INFO", "Translate Language", "unknown", lang, sizeof(lang), path);

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Info from file '%s': Version '%s', Author '%s', Language '%s'", filename, version, author, lang);
  #endif

  langFileTableAppend(table, version, author, lang, filename);
}

#if defined(LANG_DEBUG)
u64 curr_time, after_time;
char bufDebug[64];
#endif

static void ReadTranslationsFromINI(LangHashTable *table, const char *INISection, int index)
{
  if ( !table || !INISection || *INISection == '\0' || index < 0 ) return;

  char original_string[256];
  char translated_string[256];
  char lang_path[128];
  SceUID fp;
  SceIoStat stat;

  #if defined(LANG_DEBUG)
    curr_time = sceKernelGetSystemTimeWide();
  #endif

  snprintf(lang_path, sizeof(lang_path), "%s%s%s", basefolder, folder_translations, main_file_table->lang_files[index]->path);

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Reading Section '%s' from INI file '%s'", INISection, lang_path);
  #endif

  // Get file size (to allocate)
  memset(&stat, 0, sizeof(SceIoStat));
  if ( sceIoGetstat(lang_path, &stat) < 0 ) return;
  if ( stat.st_size <= 0 ) return; // Invalid file size

  SceSize alloc_size = stat.st_size + 1;

  char *fileread = NULL;
  int vplRet = sceKernelTryAllocateVpl(langVpl, alloc_size, (void**)&fileread);
  if ( vplRet < 0 )
  {
    ERROR_LOG("sceKernelTryAllocateVpl(%u) failed with error 0x%08X", alloc_size, vplRet);
    return;
  }

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Allocated %u bytes for file", alloc_size);
  #endif

  if ( !ini_openread(lang_path, &fp) ) goto END_READING_TRANSLATIONS;

  if ( sceIoRead(fp, fileread, stat.st_size) < 0 ) goto END_READING_TRANSLATIONS;

  fileread[stat.st_size] = '\0'; // Terminate string

  int i;
  int temp_trans_strings_left = translated_strings_left;
  for ( i = 0; i < temp_trans_strings_left; i++ )
  {
    // Get original string (key) from index
    if ( !ini_getkeyfromstring(INISection, i, original_string, sizeof(original_string), fileread) )
      break;

    // Get (from key) the translation
    ini_getsfromstring(INISection, original_string, original_string, translated_string, sizeof(translated_string), fileread);

    // Don't load any English or empty string (it defaults to English)
    if ( translated_string[0] == '\0' || !strcmp(original_string, translated_string) )
      continue;

    // Insert on table
    langTableInsert(table, original_string, translated_string);
    translated_strings_left--;
  }

  END_READING_TRANSLATIONS:
  sceKernelFreeVpl(langVpl, fileread); // Free allocated memory

  #if defined(LANG_DEBUG)
    DEBUG_LOG("Freed %u bytes", alloc_size);
  #endif

  ini_close(&fp);

  #if defined(LANG_DEBUG)
    after_time = sceKernelGetSystemTimeWide();
    DEBUG_LOG("Translated Strings Left -> %d", translated_strings_left);
    DEBUG_LOG("Time needed for read -> %.2f seconds", (float)(after_time-curr_time)/1000000.0f);
  #endif
}

void langTableUpdate(int langIndex)
{
  // Avoid mem leaks
  translated_strings_left = TRANSLATED_STRINGS_LIMIT;

  if (main_lang_table) langTableFree(main_lang_table);
  if (main_file_table) langFileTableFree(main_file_table);

  langTableSetup(langIndex);
}

void langTableSetup(int langIndex)
{
  currLanguageID = langIndex;

  main_lang_table = langTableCreate();
  if ( !main_lang_table ) return;

  langFileTableSearch();
  if ( !main_file_table || main_file_table->size == 1 ) return;

  // If current language is English (lang and file tables still need to be created)
  if (langIndex == 0) return;

  ReadTranslationsFromINI(main_lang_table, "GENERAL", langIndex);
  ReadTranslationsFromINI(main_lang_table, LCS ? "LCS" : "VCS", langIndex);
}

#endif

// Get translated string from .ini file
char *_t(const char *string)
{
  #ifdef LANG
    if ( currLanguageID != 0 ) return langTableSearch(main_lang_table, string);
  #endif

  return (char *)string;
}