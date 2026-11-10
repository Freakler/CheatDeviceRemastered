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

#ifndef __LANG_H__
#define __LANG_H__

#define LANG_DEBUG

#define TABLE_SIZE                823 // Prime number for hash table size (Mersenne primes won't work here unforunately)
#define LANG_FILES_LIMIT          8
#define TRANSLATED_STRINGS_LIMIT  TABLE_SIZE
#define MURMURMASH_3_SEED         0x947473

typedef int LangID;

enum
{
  LANG_ENGLISH =  0,
  LANG_UNKNOWN = -1
};

typedef struct string_lang
{
  struct string_lang *next; // Pointer to the next node (for chaining)
  u8string original_string;
  u8string trans_string;
} string_lang;

typedef struct LangHashTable
{
  string_lang *table[TABLE_SIZE]; // Array of pointers to string_lang nodes
} LangHashTable;

void langTableFree(LangHashTable* ht);

typedef struct
{
  u8string lang_name;
  u8string author_name;
  u8string version;
  u8string path;
} LanguageFile;

typedef struct
{
  LanguageFile *lang_files[LANG_FILES_LIMIT];
  usize size;
} LangFileTable;

typedef struct
{
  LangHashTable *table;
  const u8string section;
} TranslationBrowseData;

extern u8string _t(const u8string string);

extern void langTableSetup(LangID langID);
extern void langTableUpdate(LangID langID);

extern void langFileTableFree(LangFileTable *table);

extern void GetLangINIInfo(LangFileTable *table, const u8string filename);

extern LangHashTable *main_lang_table;
extern LangFileTable *main_file_table;

extern LangID currLanguageID;

#endif