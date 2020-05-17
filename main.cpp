#include <cctype>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <immintrin.h>
#include "HashFuncs.h"
#include "opt_func.h"

const int LIST_SIZE = 128;      //Размер списка
const int NUM_SIZE = 3;         //Макс количество разрядов в числе операций со списком
const int SERVICE = 118;        //Служебная константа (для построения графов)
const int HT_SIZE = 256 * 16 * 16;
const int BUF_EXTRA_SIZE = 1;
const char *FILEPATH = "../inputs/input_100000.txt";
const char *UNKWPATH = "../inputs/input.txt";

char *buf_to_free = nullptr;
size_t words_num = 0;

//Операции со списком
List_t ListInit (size_t size);                                      //Конструктор
void ListDestruct (List_t *list);                                   //Деструктор
void ListOK (List_t *list);                                         //Верификатор
size_t ListIdxSearch (List_t *list, size_t idx);                    //Поиск элемента по логическому номеру
void ListPushFront (List_t *list, Elem_t val);                      //Вставить элемент в начало списка
void ListPushBack (List_t *list, Elem_t val);                       //Вставить элемент в конец списка
void ListPushBefore (List_t *list, Elem_t val_s, Elem_t val_i);     //Вставить элемент перед заданным
void ListPushAfter (List_t *list, Elem_t val_s, Elem_t val_i);      //Вставить элемен  после заданного
void ListDelete (List_t *list, Elem_t val);                         //Удалить элемент
void ListClear (List_t *list);                                      //Очистить список
void ListPrint (List_t *list);                                      //Вывод содержимого списка подряд
void ListDump (List_t *list);                                       //Дамп списка
void ListSort (List_t *list);                                       //Пересчет индексов элементов

//Операции с таблицей
HT_t HTInit (size_t size, __uint64_t (*HashFunc) (char *str, int len));
void HTDestruct (HT_t *ht);
void HTInsert (HT_t *ht, char *str, int len = 15);

void ReadData (HT_t *ht, FILE *readfile);
char *GetWordsNum (FILE *user_input, long size);
void PrintStart (FILE *writefile);
void PrintData (HT_t *ht, FILE *writefile, char *name);

unsigned long int crc_table[256] = {0};

int main () {
    getCRC32Table ();

    HT_t ht = HTInit (HT_SIZE, ALIGN_CRC32_SSE);

    FILE *readfile = fopen (FILEPATH, "rb");
	if (!readfile) {
		printf ("Error opening file 1\n");
		return 0;
	}
    /*
    ReadData (&ht, readfile, JSHash);
    ReadData (&ht, readfile, RSHash);
    ReadData (&ht, readfile, BesouIdentity);
    ReadData (&ht, readfile, Sum);
    ReadData (&ht, readfile, Len);
    ReadData (&ht, readfile, FirstLetter);
    ReadData (&ht, readfile, MURMUR);
    ReadData (&ht, readfile, EQ1);
    ReadData (&ht, readfile, RolHash);
    ReadData (&ht, readfile, HashLy);
    */
    ReadData (&ht, readfile);
    fclose (readfile);
	printf ("Words number: %zu\n", words_num);
	/*
    FILE *writefile = fopen ("../output.csv", "w");
    PrintStart (writefile);
    PrintData (&ht, writefile, "JSHash");
    fclose (writefile);
    */
    //free (buf_to_free);
    //----------------------------------
    words_num = 0;
    HT_t ht2 = HTInit (HT_SIZE, ALIGN_CRC32_SSE);

    readfile = fopen (UNKWPATH, "rb");
    if (!readfile) {
        printf ("Error opening file 2\n");
        return 0;
    }
    /*
    ReadData (&ht, readfile, JSHash);
    ReadData (&ht, readfile, RSHash);
    ReadData (&ht, readfile, BesouIdentity);
    ReadData (&ht, readfile, Sum);
    ReadData (&ht, readfile, Len);
    ReadData (&ht, readfile, FirstLetter);
    ReadData (&ht, readfile, MURMUR);
    ReadData (&ht, readfile, EQ1);
    ReadData (&ht, readfile, RolHash);
    ReadData (&ht, readfile, HashLy);
    */
    ReadData (&ht2, readfile);
    fclose (readfile);
    printf ("Words number: %zu\n", words_num);
    /*
    FILE *writefile = fopen ("../output.csv", "w");
    PrintStart (writefile);
    PrintData (&ht, writefile, "JSHash");
    fclose (writefile);
    */


    size_t counter = 0;
    int mnoj = 256;

    for (int w = 0; w < mnoj; w++)
        for (size_t i = 1; i < HT_SIZE; i++) {
            //size_t size = ht2.lists[i].size;
            auto lists = ht2.lists[i];
            for (size_t j = lists.head; j != 0; j = lists.items[j].next)
                if (HT_HTSearch (&ht, lists.items[j].data) >= 0)
                    counter++;
        }

    counter /= mnoj;

	printf ("\nCounter: %zu\n", counter);
    HTDestruct (&ht);
    HTDestruct (&ht2);
    return 0;
}

const char *POISON = "POISON";  //Ядовитое значение

List_t ListInit (size_t size) {
    List_t list = {};
    //Выделение памяти для содержимого списка
    list.items = (List_it *) calloc (size, sizeof(List_t));
    assert (list.items);

    list.size = 0;
    list.head = list.tail = 0;
    list.free = 1;

    list.items[0].data = (char *)POISON; //Служебная ячейка
    list.items[0].next = list.items[0].prev = 0; //Указывает сама на себя

    //Заполнение списка пустых ячеек
    for (size_t idx = 1; idx < LIST_SIZE; ++idx) {
        list.items[idx].next = (idx + 1) % LIST_SIZE;
        list.items[idx].prev = (idx - 1) % LIST_SIZE;
    }
    ListOK (&list);
    return list;
}

void ListDestruct (List_t *list) {
    ListOK (list);
    list->size = 0;
    list->head = list->tail = 0;
    list->free = 1;

    //Освобождение памяти, выделенной под содержимое списка и обнуление указателя на содержмиое списка
    free (list->items);
    list->items = nullptr;
}

void ListOK (List_t *list) {
    return;
    if (list->free == 0)
        printf ("WARNING! List overflow!\n");

    //Проверка служебной ячейки
    assert (list->items[0].prev == 0);
    assert (list->items[0].next == 0);
    assert (list->items[0].data == POISON);

    assert (list->items != nullptr); //Проверка указателя на данные
    assert (list->head != list->free); //Проверка head
    assert (list->items[list->head].prev == 0);
    assert (list->tail != list->free); //Проверка tail
    assert (list->items[list->tail].next == 0);
    assert (list->dump_count > 0); //Проверка счётчика дампов

    //Проверка длины списка в прямом направлении
    size_t size = 0;
    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next, ++size);
    assert (list->size == size);

    //Проверка длины списка в обратном направлении
    size = 0;
    for (size_t idx = list->tail; idx != 0; idx = list->items[idx].prev, ++size);
    assert (list->size == size);

    //Дамп списка
    //ListDump (list);
}

size_t ListIdxSearch (List_t *list, size_t idx_s) {
    size_t idx = list->head;

    for (size_t num = 0 ; num != idx_s; idx = list->items[idx].next, ++num) {
        if (idx == 0) //Если весь список пройден и элемент не найден
            return 0;
    }
    return idx; //Возвращение логического номера элемента
}

void ListPushBefore (List_t *list, Elem_t val_s, Elem_t val_i) {
    ListOK (list);
    //Помещение значения в первую доступную свободную ячейку
    size_t cur = list->free;
    list->items[cur].data = val_i;
    list->free = list->items[cur].next;
    ++list->size; //Увеличиваем размер списка
    //Поиск следующего за данным элемента
    size_t idx_after = ListValSearch(list, val_s);
    if (!idx_after) {
        printf("Element %s not found\n", val_s);
        exit(1);
    }
    //Если вставили в самое начало
    if (idx_after == list->head)
        list->head = cur;
    //Если вставили не в самое начало, то изменяем указатель на след. элемент у предыдущего
    if (list->items[idx_after].prev != 0)
        list->items[list->items[idx_after].prev].next = cur;
    //Обработка текущего и следующего элементов
    list->items[cur].prev = list->items[idx_after].prev;
    list->items[idx_after].prev = cur;
    list->items[cur].next = idx_after;
    ListOK (list);
}

void ListPushAfter (List_t *list, Elem_t val_s, Elem_t val_i) {
    ListOK (list);
    //Помещение значения в первую доступную свободную ячейку
    size_t cur = list->free;
    list->items[cur].data = val_i;
    list->free = list->items[cur].next;
    ++list->size; //Увеличиваем размер списка
    //Поиск предыдущего элемента
    size_t idx_before = ListValSearch(list, val_s);
    if (!idx_before) {
        printf("Element %s not found\n", val_s);
        exit(1);
    }
    //Если вставили в самый конец
    if (idx_before == list->tail)
        list->tail = cur;
    //Если вставили не в самый конец, то изменяем указатель на пред. элемент у следующего
    if (list->items[idx_before].next != 0)
        list->items[list->items[idx_before].next].prev = cur;
    //Обработка текущего и предыдущего элементов
    list->items[cur].next = list->items[idx_before].next;
    list->items[idx_before].next = cur;
    list->items[cur].prev = idx_before;
    ListOK (list);
}

void ListDelete (List_t *list, Elem_t val) {
    ListOK (list);
    //Поиск удаляемого элемента
    size_t cur = ListValSearch(list, val);
    //Перезаписываем следующий элемент у предыдущего
    if (list->items[cur].prev != 0)
        list->items[list->items[cur].prev].next = list->items[cur].next;
    //Перезаписываем предыдущий элемент у следующего
    if (list->items[cur].next != 0)
        list->items[list->items[cur].next].prev = list->items[cur].prev;
    --list->size; //Уменьшаем размер списка

    //Перемещаем голову и хвост
    if (list->head == cur)
        list->head = list->items[cur].next;
    else if (list->tail == cur)
        list->tail = list->items[cur].prev;

    //Вносим только что удалённый элемент в начало списка пустых элементов
    list->items[cur].next = list->free;
    list->free = cur;
    ListOK (list);
}

void ListClear (List_t *list) {
    ListOK (list);
    //Временно сохраняем значение счётчика дампов
    size_t dump_count = list->dump_count;
    //Уничтожаем и заново создаем список
    ListDestruct (list);
    *list = ListInit (LIST_SIZE);
    //Записываем предыдущее значение счётчика дампов
    list->dump_count = dump_count;
    ListOK (list);
}

void ListPushFront (List_t *list, Elem_t val) {
    ListOK (list);
    //Помещение значения в первую доступную свободную ячейку
    size_t cur = list->free;
    list->items[cur].data = val;
    list->free = list->items[cur].next;
    ++list->size;
    //Обработка первого элемента
    list->items[cur].prev = 0;
    list->items[cur].next = list->head;
    //Если до этого список был непуст
    if (list->head != 0)
        list->items[list->head].prev = cur;
    else //Если head == 0, то и tail == 0
        list->tail = cur;
    list->head = cur;
    ListOK (list);
}

void ListPushBack (List_t *list, Elem_t val) {
    //ListOK (list);
    //Помещение значения в первую доступную свободную ячейку
    size_t cur = list->free;
    list->items[cur].data = val;
    list->free = list->items[cur].next;
    ++list->size;
    //Обработка последнего элемента
    list->items[cur].next = 0;
    list->items[cur].prev = list->tail;
    //Если до этого список был непуст
    if (list->tail != 0)
        list->items[list->tail].next = cur;
    else //Если tail == 0, то и head == 0
        list->head = cur;
    list->tail = cur;
    //ListOK (list);
}

void ListPrint(List_t *list) {
    ListOK (list);
    printf ("This is ListPrint:\n"); //Служебный вывод содержимого списка в консоль
    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next) {
        printf ("%s ", list->items[idx].data);
    }
    printf ("\n");
}

void ListDump (List_t *list) {
    //Дамп в консоль
    printf ("-------");

    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("----------");
    printf ("\n");

    printf ("HEAD: %zu, TAIL: %zu, SIZE: %zu, FREE: %zu\n", list->head, list->tail, list->size, list->free);

    printf ("-------");

    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("----------");
    printf ("\n");

    printf (" idx: ");
    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("%10zu", idx);
    printf ("\n");

    printf ("-------");

    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("----------");
    printf ("\n");

    printf ("prev: ");
    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("%10zu", list->items[idx].prev);
    printf ("\n");

    printf ("data: ");
    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("%10s", list->items[idx].data);
    printf ("\n");

    printf ("next: ");
    for (size_t idx = 0; idx < LIST_SIZE; ++idx)
        printf ("%10zu", list->items[idx].next);
    printf ("\n");

    printf ("\n");
    //Окончание дампа в консоль

    //Запись в dot файл (инициализация графа)
    FILE *Dot = fopen ("../temp.dot", "w");
    fprintf (Dot, "digraph {\nnode [shape=\"box\"]\n"
                  "edge [style=\"dashed\" dir=\"both\"]\n"
                  "dirtype = \"both\"\n"
                  "rankdir=\"LR\"\n");

    //Запись всех элементов списка в dot файл
    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next) {
        fprintf (Dot, "\telem_%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"3\">\n"
                      "\t\t<TR><TD COLSPAN=\"2\" BGCOLOR=\"white\"> idx: %zu </TD></TR>\n"
                      "\t\t<TR><TD COLSPAN=\"2\" BGCOLOR=\"white\"> data: %s </TD></TR>\n"
                      "\t\t<TR><TD BGCOLOR=\"salmon\" PORT=\"prev\"> prev: %lu </TD>\n"
                      "\t\t<TD BGCOLOR=\"lawngreen\" PORT=\"next\"> next: %lu </TD></TR>\n"
                      "\t</TABLE>>]\n", idx, idx, list->items[idx].data,
                 list->items[idx].prev == 0 ? -1 : list->items[idx].prev,
                 list->items[idx].next == 0 ? -1 : list->items[idx].next);
    }
    //Если список непуст
    if (list->head != 0)
        fprintf (Dot, "\"data\" -> elem_%zu\n", list->head);
    else
        fprintf (Dot, "\"data\" -> \"empty\"\n");
    for (size_t idx = list->head; list->items[idx].next != 0; idx = list->items[idx].next)
        fprintf (Dot, "elem_%zu -> elem_%zu\n", idx, list->items[idx].next);

    //Запись свободных ячеек в dot файл
    for (size_t idx = list->free; idx != 0; idx = list->items[idx].next) {
        fprintf (Dot, "\telem_free_%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"3\">\n"
                      "\t\t<TR><TD COLSPAN=\"2\" BGCOLOR=\"aquamarine\"> idx: %zu </TD></TR>\n"
                      "\t\t<TR><TD COLSPAN=\"2\" BGCOLOR=\"aquamarine\"> data: %s </TD></TR>\n"
                      "\t\t<TR><TD BGCOLOR=\"salmon\" PORT=\"prev\"> prev: %lu </TD>\n"
                      "\t\t<TD BGCOLOR=\"lawngreen\" PORT=\"next\"> next: %lu </TD></TR>\n"
                      "\t</TABLE>>]\n", idx, idx, list->items[idx].data,
                 list->items[idx].prev == 0 ? -1 : list->items[idx].prev,
                 list->items[idx].next == 0 ? -1 : list->items[idx].next);
    }
    //Если список не заполнен
    if (list->free != 0)
        fprintf (Dot, "\"free\" -> elem_free_%zu\n", list->free);
    else
        fprintf (Dot, "\"free\" -> \"empty\"\n");
    for (size_t idx = list->free; list->items[idx].next != 0; idx = list->items[idx].next)
        fprintf (Dot, "elem_free_%zu -> elem_free_%zu\n", idx, list->items[idx].next);

    fprintf (Dot, "}\n");
    fclose (Dot);
    //Окончание ввода в dot файл

    //Создание gif изображения
    char *command = (char *) calloc (NUM_SIZE + SERVICE, sizeof (char));
    sprintf (command, "dot -Tpng /home/biscuitslayer/CLionProjects/6_HashTable/temp.dot "
                      "-o/home/biscuitslayer/CLionProjects/6_HashTable/images/temp%zu.png", list->dump_count);

    //Если программа работает заново, папка с изображениями очищается
    if (list->dump_count == 1)
        system ("rm /home/biscuitslayer/CLionProjects/6_HashTable/images/*");
    system (command);

    //Освобождение памяти по команду и увеличение счётчика дампа
    free (command);
    ++list->dump_count;
}

void ListSort (List_t *list) {
    ListOK(list);
    //Создание массива с отсортированным содержимым
    Elem_t *sorted_list_data = (Elem_t *) calloc(list->size, sizeof(Elem_t));

    //Копирование отсортированного списка в массив
    for (size_t idx = list->head, sorted_idx = 0; idx != 0; idx = list->items[idx].next, ++sorted_idx)
        sorted_list_data[sorted_idx] = list->items[idx].data;

    //Перезапись индексов и содержимого списка
    for (size_t idx = 1; idx < LIST_SIZE; ++idx) {
        list->items[idx].next = (idx + 1) % LIST_SIZE;
        list->items[idx].prev = (idx - 1) % LIST_SIZE;
        if (idx <= list->size)
            list->items[idx].data = sorted_list_data[idx - 1];
        else
            list->items[idx].data = 0;
    }

    //Освождение памяти
    free(sorted_list_data);

    //Обработка отсортированного списка
    list->head = 1;
    list->tail = list->size;
    list->items[list->size].next = 0;
    list->free = list->size + 1;
    ListOK(list);
}

HT_t HTInit (size_t size, unsigned long int (*HashFunc) (char *str, int len)) {
    HT_t ht = {};

    ht.lists = (List_t *) calloc (size, sizeof (List_t));
    ht.sizes = (size_t *) calloc (size, sizeof (size_t));

    ht.HashFunc = HashFunc;

    return ht;
}

void HTDestruct (HT_t *ht) {
    for (int i = 0; i < HT_SIZE; ++i)
        ListDestruct (&ht->lists[i]);
    free (buf_to_free);
    free (ht->sizes);
    free (ht->lists);
}

long getSizeFile(FILE *file) {
    if (file == nullptr)
        return -1;

    long size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

void ReadData (HT_t *ht, FILE *readfile) {
    // Only for words with a length equal to 15
    char *temp_buf = GetWordsNum (readfile, getSizeFile (readfile));

    __m128i *buf = aligned_16_AVX (temp_buf, words_num);

    free (temp_buf);
    ht->buf = buf;

    for (size_t id = 0; id < words_num; id++)
        HTInsert(ht, (char *) &buf[id], strlen ((char *) &buf[id]));
}

void PrintStart (FILE *writefile) {
    fprintf (writefile, "LIST_NUM");
    for (size_t idx = 0; idx < HT_SIZE; ++idx)
        fprintf (writefile, ",%zu", idx);
    fprintf (writefile, "\n");

}

void PrintData (HT_t *ht, FILE *writefile, char *name) {
    fprintf (writefile, "%s", name);
    for (size_t idx = 0; idx < HT_SIZE; ++idx)
        fprintf (writefile, ",%zu", ht->sizes[idx]);
    fprintf (writefile, "\n");
}

char *GetWordsNum (FILE *user_input, long size) {
    char *buf = (char *) calloc (size + BUF_EXTRA_SIZE, sizeof (char));     //Буфер из файла ввода пользователя
    fread (buf, sizeof(char), size, user_input);                            //Загрузка ввода пользователя в буфер

    size_t i = 0;   //Индекс в буфере
    words_num = 0;  //Счётчик слов

    //Пропуск пробелов до первой команды
    while (isspace (buf[i])) ++i;

    //Обработка ввода пользователя
    for ( ; ( (i < size) && (buf[i] != EOF) ); ++i) {
        //Новая строка в программе
        if (buf[i] == '\n' || isspace (buf[i]))
            ++words_num, buf[i] = '\0';

        //Пропуск пробелов в начале новой строки
        while ((isspace (buf[i]) || buf[i] == '\0') && (i < size - 1))
            ++i;

        //Если достигли конца ввода
        if (buf[i] == EOF) {
            ++words_num;
            break;
        }
        if ((i == size - 1) && (buf[i] != '\n')) {
            ++words_num;
        }
    }

    return buf;
}

void HTInsert (HT_t *ht, char *str, int len) {
    unsigned long int code = ht->HashFunc (str, len);
    code %= HT_SIZE;
    List_t *list = &(ht->lists[code]);
    if (list->items == nullptr)
        *list = ListInit (LIST_SIZE);
    ListPushBack (list, str);
    ++ht->sizes[code];
}
