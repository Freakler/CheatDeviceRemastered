
#ifndef __LANG_H__
#define __LANG_H__

#define TABLE_SIZE 2579 // Prime number for hash table size
#define LANG_FILES_LIMIT 8
#define TRANSLATED_STRINGS_LIMIT TABLE_SIZE
#define FILE_SIZE_LIMIT 0xFFFF
#define MURMURMASH_3_SEED 0x947473

char* translate_string(const char* string);
void setup_lang(int langIndex);
void update_lang(int langIndex);

typedef struct string_lang {
    char *original_string;
    char *trans_string;
    struct string_lang *next; // Pointer to the next node (for chaining)
} string_lang;

typedef struct LangHashTable {
    string_lang *table[TABLE_SIZE]; // Array of pointers to string_lang nodes
} LangHashTable;

typedef struct {
    char* Language;
    char* Author;
    char* Version;
    char* FileName;
} LanguageFile;

typedef struct {
    LanguageFile *lang_files[LANG_FILES_LIMIT];
    int size;
} LangFileTable;

void GetINIInfo(LangFileTable *table, char* filename);

extern LangHashTable *main_lang_table;
extern LangFileTable *main_file_table;

extern int LanguageConfigStart;

#endif