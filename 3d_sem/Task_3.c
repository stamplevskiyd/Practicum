#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define L 4

//Beta-1. Рабочий ввод строки, добавление элемента и построение списка, а также вывод. Может считать текст и построчно его вывести

//Beta-2. добавлено удаление списка. С valgrind проблем не обнаружено

//Beta-3. Добавление дополнительных функций для перестановки элементов

//Beta-4. SortLite-полноценно рабочая сортировка

//Beta-5. Изменен признак конца текста, добавлена работа "по кругу". Исправлена проблема с valgrind, исправлен баг с попаданием
// \n в список при втором запуске. Программа полностью готова и протестирована

struct List
{
    char *String;
    struct List *Next;
};

typedef struct List sl;

char *InitString(void)           //ввод строки. Проверить, нет ли лишних free. Скорее всего free здесь и не нужны
{
    int Size = L, Position = 0, Length;
    char *String = NULL;
    if ( (String = (char*)malloc( Size*sizeof(char)) ) )                              //если возникли проблемы с выделением памяти
    {
        while (*(fgets(String+Position, L, stdin)) != '!')  //останавливаем ввод, когда считаем строку, начинающуюся с !
        {
            Length = strlen(String);
            if (String[Length-1] != '\n')          //считали не всю строку
            {
                Position=Length;
                Size += L;
                String = (char*)realloc(String,Size*sizeof(char));
                if (String == NULL)
                {
                    printf("Error! Unable to allocate memory!\n");
                    return NULL;
                }
            }
            else
            {
                String[Length-1] = '\0';
                return(String);
            }
        }
        free(String);
        return NULL;
    }
    printf("Error! Unable to allocate memory!\n");
    return NULL;
}

void AddLink (sl **List, char *String)  //добавление звена в конец списка
{
    sl **Pointer = List;   //элемент-указатель, с помощью которого ищем тот адрес, на который указывает последнее звено списка
    while ((*Pointer) != NULL)  //идем по списку, пока не найдем конец
        Pointer = &((*Pointer)->Next);
    sl *New=(sl*)malloc(sizeof(sl));  //создаем новое звено
    if (New == NULL)
    {
        printf("Error! Unable to allocate memory!\n");
        return;
    }
    New->String = String;
    New->Next = NULL;
    (*Pointer) = New;  //приписываем адресу из последнего элемента адрес нового звена
    return;
}

sl *InitList(void)    //просто добавляет элементы-строки, используя другую функцию
{
    sl *List = NULL;
    char *String = NULL;
    while (String = InitString())
        AddLink(&List, String);
    return List;
}

void PrintList(sl *List) //печать всех строк списка
{
    if (List != NULL)
    {
        printf("%s\n", List->String);
        PrintList(List->Next);
    }
    return;
}

void Exchange (sl **List)  //двигает первый элемент вправо до тех пор, пока не встретит элемент больше него. (то есть ставит на нужное место).работает.
{
    if (List != NULL)
    {
        if ((*List)->Next != NULL)
        {
            sl *Pointer = (*List)->Next;
            if (strcmp(Pointer->String, (*List)->String) < 0) //если первые две строки не упорядочены, переставляем их и идем дальше
            {
                (*List)->Next = Pointer->Next;
                Pointer->Next = *List;
                *List = Pointer;
            }
            Exchange(&(*List)->Next);
        }
    }
    return;
}


void SortList(sl **List)  //смотри программу из тетрадки: вставка звена в упорядоченный список
{
    if (*List != NULL)
    {
        if ((*List)->Next != NULL)
        {
            SortList(&((*List)->Next));
            Exchange(List);
        }
    }
    return;
}

void DeleteList(sl *List)  //удаление списка
{
    if (List != NULL)
    {
        DeleteList(List->Next);
        free(List->String);
    }
    free(List);
    return;
}

int main()
{
    sl *List = NULL;
    char Repeat= '1';
    while (Repeat != '0')
    {
        printf("Enter text\n");
        List = InitList();
        printf("Initial list:\n");
        PrintList(List);
        printf("\n");
        SortList(&List);
        printf("Sorted list:\n");
        PrintList(List);
        printf("\n");
        DeleteList(List);
        printf("Do you want to repeat program? No-0, Yes-anything else\n");
        scanf("%c", &Repeat);
        while (Repeat == '\n' )
            scanf("%c", &Repeat);
        getchar();  //после ввода Repeat нажимаем enter и вводим \n, который потом попадает в список, чего быть не должно
    }
    return 0;
}

