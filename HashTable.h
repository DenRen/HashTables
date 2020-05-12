//
// Created by tester on 12.05.2020.
//

#ifndef INC_6_HASHTABLE_HASHTABLE_H
#define INC_6_HASHTABLE_HASHTABLE_H

#include <cctype>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

const int LIST_SIZE = 1000; //?????? ??????
const int NUM_SIZE = 3;   //???? ?????????? ???????? ? ????? ???????? ?? ???????
const int SERVICE = 118;  //????????? ????????? (??? ?????????? ??????)
const int HT_SIZE = 201;
const int BUF_EXTRA_SIZE = 1;
const char *FILEPATH = "inputs/input.txt";

//??? ??????????? ??????
typedef char * Elem_t;

//????????? ???????? ??????
typedef struct {
    Elem_t data = nullptr;
    size_t prev = 0, next = 0;
} List_it;

//????????? c????? ??????
struct List_t {
    List_it *items = nullptr;
    size_t size = 0;
    size_t head = 0, tail = 0;
    size_t free = 1;
    size_t dump_count = 1;
};

typedef struct {
    List_t *lists = nullptr;
    size_t *sizes = nullptr;
} HT_t;

char *buf_to_free = nullptr;
size_t words_num = 0;

//???????? ?? ???????
List_t ListInit (size_t size);                                      //???????????
void ListDestruct (List_t *list);                                   //??????????
void ListOK (List_t *list);                                         //???????????
size_t ListValSearch (List_t *list, Elem_t val);                    //????? ???????? ?? ????????
size_t ListIdxSearch (List_t *list, size_t idx);                    //????? ???????? ?? ??????????? ??????
void ListPushFront (List_t *list, Elem_t val);                      //???????? ??????? ? ?????? ??????
void ListPushBack (List_t *list, Elem_t val);                       //???????? ??????? ? ????? ??????
void ListPushBefore (List_t *list, Elem_t val_s, Elem_t val_i);     //???????? ??????? ????? ????????
void ListPushAfter (List_t *list, Elem_t val_s, Elem_t val_i);      //???????? ??????  ????? ?????????
void ListDelete (List_t *list, Elem_t val);                         //??????? ???????
void ListClear (List_t *list);                                      //???????? ??????
void ListPrint (List_t *list);                                      //????? ??????????? ?????? ??????
void ListDump (List_t *list);                                       //???? ??????
void ListSort (List_t *list);                                       //???????? ???????? ?????????

//???????? ? ????????
HT_t HTInit (size_t size);
void HTDestruct (HT_t *ht);
void HTInsert (HT_t *ht, unsigned long int (*HF) (char *str), char *str);
bool HTSearch (HT_t *ht, char *str);
/*
unsigned long int EQ1 (char *str);
unsigned long int MURMUR (char *str);
unsigned long int FirstLetter (char *str);
unsigned long int Len (char *str);
unsigned long int Sum (char *str);
unsigned long int BesouIdentity (char *str);
unsigned long int JSHash (char *str);
unsigned long int RSHash (char *str);
unsigned long int RolHash (char *str);
unsigned long int HashLy (char *str);
unsigned long int GNU_HASH (char *str);
*/
unsigned long int CRC32(char *str);

unsigned long crc_table_opt1[256] = { 0, 1996959894, 3993919788, 2567524794, 124634137, 1886057615, 3915621685, 2657392035, 249268274, 2044508324, 3772115230, 2547177864, 162941995, 2125561021, 3887607047, 2428444049, 498536548, 1789927666, 4089016648, 2227061214, 450548861, 1843258603, 4107580753, 2211677639, 325883990, 1684777152, 4251122042, 2321926636, 335633487, 1661365465, 4195302755, 2366115317, 997073096, 1281953886, 3579855332, 2724688242, 1006888145, 1258607687, 3524101629, 2768942443, 901097722, 1119000684, 3686517206, 2898065728, 853044451, 1172266101, 3705015759, 2882616665, 651767980, 1373503546, 3369554304, 3218104598, 565507253, 1454621731, 3485111705, 3099436303, 671266974, 1594198024, 3322730930, 2970347812, 795835527, 1483230225, 3244367275, 3060149565, 1994146192, 31158534, 2563907772, 4023717930, 1907459465, 112637215, 2680153253, 3904427059, 2013776290, 251722036, 2517215374, 3775830040, 2137656763, 141376813, 2439277719, 3865271297, 1802195444, 476864866, 2238001368, 4066508878, 1812370925, 453092731, 2181625025, 4111451223, 1706088902, 314042704, 2344532202, 4240017532, 1658658271, 366619977, 2362670323, 4224994405, 1303535960, 984961486, 2747007092, 3569037538, 1256170817, 1037604311, 2765210733, 3554079995, 1131014506, 879679996, 2909243462, 3663771856, 1141124467, 855842277, 2852801631, 3708648649, 1342533948, 654459306, 3188396048, 3373015174, 1466479909, 544179635, 3110523913, 3462522015, 1591671054, 702138776, 2966460450, 3352799412, 1504918807, 783551873, 3082640443, 3233442989, 3988292384, 2596254646, 62317068, 1957810842, 3939845945, 2647816111, 81470997, 1943803523, 3814918930, 2489596804, 225274430, 2053790376, 3826175755, 2466906013, 167816743, 2097651377, 4027552580, 2265490386, 503444072, 1762050814, 4150417245, 2154129355, 426522225, 1852507879, 4275313526, 2312317920, 282753626, 1742555852, 4189708143, 2394877945, 397917763, 1622183637, 3604390888, 2714866558, 953729732, 1340076626, 3518719985, 2797360999, 1068828381, 1219638859, 3624741850, 2936675148, 906185462, 1090812512, 3747672003, 2825379669, 829329135, 1181335161, 3412177804, 3160834842, 628085408, 1382605366, 3423369109, 3138078467, 570562233, 1426400815, 3317316542, 2998733608, 733239954, 1555261956, 3268935591, 3050360625, 752459403, 1541320221, 2607071920, 3965973030, 1969922972, 40735498, 2617837225, 3943577151, 1913087877, 83908371, 2512341634, 3803740692, 2075208622, 213261112, 2463272603, 3855990285, 2094854071, 198958881, 2262029012, 4057260610, 1759359992, 534414190, 2176718541, 4139329115, 1873836001, 414664567, 2282248934, 4279200368, 1711684554, 285281116, 2405801727, 4167216745, 1634467795, 376229701, 2685067896, 3608007406, 1308918612, 956543938, 2808555105, 3495958263, 1231636301, 1047427035, 2932959818, 3654703836, 1088359270, 936918000, 2847714899, 3736837829, 1202900863, 817233897, 3183342108, 3401237130, 1404277552, 615818150, 3134207493, 3453421203, 1423857449, 601450431, 3009837614, 3294710456, 1567103746, 711928724, 3020668471, 3272380065, 1510334235, 755167117 };
void CRC32_tablefill();
unsigned long int CRC32_opt1(char *str);

void ReadData (HT_t *ht, FILE *readfile, unsigned long int (*HF) (char *str));
char *GetWordsNum (FILE *user_input, struct stat file_info);
void PrintStart (FILE *writefile);
void PrintData (HT_t *ht, FILE *writefile, char *name);

#endif //INC_6_HASHTABLE_HASHTABLE_H