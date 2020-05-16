//
// Created by tester on 14.05.2020.
//

#ifndef INC_6_HASHTABLE_HASHFUNCS_H
#define INC_6_HASHTABLE_HASHFUNCS_H
unsigned long int EQ1 (char *str) ;

unsigned long int FirstLetter (char *str);

unsigned long int BesouIdentity (char *str);

unsigned long int RSHash (char *str);

unsigned long int JSHash (char *str);

unsigned long int Len (char *str);

unsigned long int HashLy(char *str);

unsigned long int Sum (char *str);

unsigned long int GNU_HASH (char *str);

unsigned long int MURMUR (char *str);

unsigned long int rol (unsigned long int v, unsigned long shift);

unsigned long int RolHash (char *str);

unsigned long int CRC32 (char *str, int len);

unsigned long int CRC32_SSE (char *word, int len);

unsigned long int CRC32_opt1 (char *str);

unsigned long *getCRC32Table ();

struct crc32 {

    unsigned long *crc_table;

    crc32 ();
    unsigned long int CRC32 (char *buf);
};

//--------------------------------------------------------------------
char *_readfile (const char *path, long *size);

void _writefile (char *path, char *data, long size);

long _getSizeFile (FILE *file);
#endif //INC_6_HASHTABLE_HASHFUNCS_H
