#include <cctype>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "opt_func.h"

const int LIST_SIZE = 100;     //Размер списка
const int NUM_SIZE = 3;         //Макс количество разрядов в числе операций со списком
const int SERVICE = 118;        //Служебная константа (для построения графов)
const int HT_SIZE = 256 * 2 * 4;
const int BUF_EXTRA_SIZE = 1;
const char *FILEPATH = "../inputs/input_100000.txt";

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
HT_t HTInit (size_t size);
void HTDestruct (HT_t *ht);
void HTInsert (HT_t *ht, unsigned long int (*HF) (char *str), char *str);

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

int main () {
    HT_t ht = HTInit (HT_SIZE);

    FILE *readfile = fopen (FILEPATH, "rb");
	if (!readfile) {
		printf ("Error opening file\n");
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
    ReadData (&ht, readfile, CRC32_opt1);
    fclose (readfile);
	printf ("Words number: %zu\n", words_num);
	/*
    FILE *writefile = fopen ("../output.csv", "w");
    PrintStart (writefile);
    PrintData (&ht, writefile, "JSHash");
    fclose (writefile);
    */

    size_t counter = 0;

    for (size_t i = 0; i < HT_SIZE; i++)
        for (size_t j = 0; j < ht.sizes[i]; j++)
            if (HTSearch(&ht, ht.lists[i].items[j].data))
                counter++;

	printf ("%zu\n", counter);
	printf ("\n%zu - %zu = %zu == HT_SIZE", words_num, counter, words_num - counter);
    HTDestruct (&ht);
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

HT_t HTInit (size_t size) {
    HT_t ht = {};
    ht.lists = (List_t *) calloc (size, sizeof (List_t));
    ht.sizes = (size_t *) calloc (size, sizeof (size_t));
    return ht;
}

void HTDestruct (HT_t *ht) {
    for (int i = 0; i < HT_SIZE; ++i)
        ListDestruct (&ht->lists[i]);
    free (buf_to_free);
    free (ht->sizes);
    free (ht->lists);
}

void ReadData (HT_t *ht, FILE *readfile, unsigned long int (*HF) (char *str)) {
    struct stat file_info = {};
    stat (FILEPATH, &file_info);
    char *buf = GetWordsNum (readfile, file_info);
    buf_to_free = buf;
    for (size_t idx = 0; idx < words_num; ++idx) {
        HTInsert (ht, HF, buf);
        while (*buf != '\0')
            ++buf;
        if (idx == words_num - 1)
            break;
        ++buf;
        while (isspace (*buf))
            ++buf;
    }
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

char *GetWordsNum (FILE *user_input, struct stat file_info) {
    char *buf = (char *) calloc (file_info.st_size + BUF_EXTRA_SIZE, sizeof (char)); //Буфер из файла ввода пользователя
    fread (buf, sizeof(char), file_info.st_size, user_input); //Загрузка ввода пользователя в буфер

    size_t i = 0; //Индекс в буфере

    //Пропуск пробелов до первой команды
    while (isspace (buf[i])) ++i;

    //Обработка ввода пользователя
    for ( ; ( (i < file_info.st_size) && (buf[i] != EOF) ); ++i) {
        //Новая строка в программе
        if (buf[i] == '\n' || isspace (buf[i]))
            ++words_num, buf[i] = '\0';

        //Пропуск пробелов в начале новой строки
        while ((isspace (buf[i]) || buf[i] == '\0') && (i < file_info.st_size - 1))
            ++i;

        //Если достигли конца ввода
        if (buf[i] == EOF) {
            ++words_num;
            break;
        }
        if ((i == file_info.st_size - 1) && (buf[i] != '\n')) {
            ++words_num;
        }
    }
    return buf;
}

void HTInsert (HT_t *ht, unsigned long int (*HF) (char *str), char *str) {
    unsigned long int code = HF (str);
    code %= HT_SIZE;
    List_t *list = &(ht->lists[code]);
    if (list->items == nullptr)
        *list = ListInit (LIST_SIZE);
    ListPushBack (list, str);
    ++ht->sizes[code];
}

unsigned long int EQ1 (char *str) {
    //Возвращает единицу
    return 1;
}

unsigned long int FirstLetter (char *str) {
    //Возвращает ASCII код первого символа
    return (unsigned long int) tolower (str[0]);
}

unsigned long int BesouIdentity (char *str) {
    //Простые числа
    const int A = 54059, B = 76963, C = 86969, FIRSTH = 37;
    char *s = str;
    unsigned h = FIRSTH;
    while (*s) {
        h = (h * A) ^ (s[0] * B);
        s++;
    }
    return h % C;
}

unsigned long int RSHash (char *str) {
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;

    for(std::size_t i = 0; i < strlen(str); i++)
    {
        hash = hash * a + str[i];
        a    = a * b;
    }

    return (hash & 0x7FFFFFFF);
}

unsigned long int JSHash (char *str) {
    unsigned int hash = 1315423911;
    for(size_t i = 0; i < strlen(str); i++)
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    return (hash & 0x7FFFFFFF);
}

unsigned long int Len (char *str) {
    //Возвращает длину строки
    return strlen (str);
}

unsigned long int HashLy(char *str) {
    unsigned int hash = 0;

    for(; *str; str++)
        hash = (hash * 1664525) + (unsigned char)(*str) + 1013904223;

    return hash;
}

unsigned long int Sum (char *str) {
    unsigned long int code = 0;
    char *c = str;
    while (*c++ != '\0')
        code += (int) *c;
    return code;
}

unsigned long int GNU_HASH (char *str)
{
    unsigned long int h = 5381;

    for (unsigned char c = *str; c != '\0'; c = *++str)
        h = h * 33 + c;

    return h;
}

unsigned long int MURMUR (char *str) {
    char *key = str;
    unsigned int len = strlen (str);

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char * data = (const unsigned char *) key;
    unsigned int k;

    while (len >= 4) {
        k  = data [0];
        k |= data [1] << 8;
        k |= data [2] << 16;
        k |= data [3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3:
            h ^= data [2] << 16;
        case 2:
            h ^= data [1] << 8;
        case 1:
            h ^= data [0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

unsigned long int rol (unsigned long int v, unsigned long shift) {
    unsigned long s =  shift >=  0 ? shift % 32 : - ((-shift) % 32);
    return (v << s) | (v >> (32 - s) );
}

unsigned long int RolHash (char *str) {
    unsigned long int code = 0;
    char *c = str;
    while (*c++ != '\0') {
        code = rol (code, 1);
        code ^= (unsigned long int) *c;
    }
    return code;
}

unsigned long int CRC32 (char *str) {

    unsigned long len = strlen (str);
    unsigned long crc_table[256] = {};
    unsigned long crc = 0;
    for (int i = 0; i < 256; i++)
    {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table[(crc ^ *str++) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

unsigned long int CRC32_opt1 (char *str) {

    unsigned long len = strlen (str);
    unsigned long crc_table[256] = {};
    unsigned long crc = 0;

    //CRC32_tablefill();

    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table_opt1[(crc ^ *str++) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

void CRC32_tablefill () {
    unsigned long crc = 0;

    for (int i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table_opt1[i] = crc;
    };
}