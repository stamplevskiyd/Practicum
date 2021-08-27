#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Beta-9. Окончательный вариант, в таком виде и была сдана

struct List
{
    long int Offset; //в байтах, для корректной работы
    int Length; //в символах, для наглядности. все равно только между собой сравниваются. символ конца строки не считается
    struct List *Next;
};

typedef struct List sl;

sl *InitList(FILE *Source)    //просто добавляет элементы-строки в список, используя функцию GetStr, которая получает из файла строчки по одной
{
    sl *List = (sl*)malloc(sizeof(sl));
    sl *Pointer = List;
    Pointer->Length = 0;
    Pointer->Offset = ftell(Source);
    Pointer->Next = NULL;
    int Character;
    while ( (Character = fgetc(Source)) != EOF ) //и двигаемся по файлу к новой строке и считаем длину старой (в символах)
    {
        if (Character != '\n')
            Pointer->Length++;
        else
        {
            Character = fgetc(Source);
            if (Character != EOF)  //чтобы не создавать звено после последней строки, в котором ничего не будет
            {
                fseek(Source, -1, SEEK_CUR); //если не конец, возвращаемся назад на символ
                sl *New = (sl *) malloc(sizeof(sl));  //создаем новое звено
                if (New == NULL) {
                    printf("Error! Unable to allocate memory!\n");
                    fclose(Source);
                    return NULL;
                }
                New->Offset = ftell(Source);
                New->Next = NULL;
                New->Length = 0;
                Pointer->Next = New;
                Pointer = Pointer->Next;
            }
        }
    }
    return List;
}

void Exchange (sl **List)  //двигает первый элемент вправо до тех пор, пока не встретит строку длиннее нее. (то есть ставит на нужное место).работает.
{
    if (List != NULL)
    {
        if ((*List)->Next != NULL)
        {
            sl *Pointer = (*List)->Next;
            if (Pointer->Length < (*List)->Length) //если первые две строки не упорядочены, переставляем их и идем дальше
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


void SortList(sl **List)  //сортировка элементов списка по длинне, от самого короткого к самому длинному. Если одинаковые- идут в первоначальном порядке
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
        DeleteList(List->Next);
    free(List);
    return;
}

int CompareString(FILE *Source, long int Offset1, long int Offset2)
{
    int Letter1, Letter2, Sign = 0;

    fseek(Source, Offset1, SEEK_SET); //устанавливаем смещение на начало первой строки
    Letter1 = fgetc(Source);
    Offset1 += sizeof(char); //чтобы в дальнейшем делать сдвиг на следующую букву
    fseek(Source, Offset2, SEEK_SET);
    Letter2 = fgetc(Source);
    Offset2 += sizeof(char); //чтобы в дальнейшем делать сдвиг на следующую букву

    if (Letter1 > Letter2) //зачем идти в цикл, если они отличаются в первом же символе
        Sign = 1;
    if (Letter1 < Letter2)
        Sign = -1;
    while ( (Sign == 0) && (Letter1 != '\n') && (Letter1 != EOF) && (Letter2 != '\n') && (Letter2 != EOF) )
    {
        if (Letter1 > Letter2) //зачем идти дальше в цикл, если они отличаются в первом же символе
            Sign = 1;
        if (Letter1 < Letter2)
            Sign = -1;

        fseek(Source, Offset1, SEEK_SET); //устанавливаем смещение на начало первой строки
        Letter1 = fgetc(Source);
        Offset1 += sizeof(char); //чтобы в дальнейшем делать сдвиг на следующую букву
        fseek(Source, Offset2, SEEK_SET);
        Letter2 = fgetc(Source);
        Offset2 += sizeof(char); //чтобы в дальнейшем делать сдвиг на следующую букву
    }
    if (Sign == 0)
    {
        if ( ((Letter1 == EOF)||(Letter1 == '\n')) && ((Letter2 == EOF)||(Letter2 == '\n')) ) //EOF и '\n' будем считать равнозначными
            Sign = 0;
        else
        {
            if (Letter1 > Letter2) //else-значит, символ конца строки или файла-только один из двух. а значит, строки не одинаковые
                Sign = 1;
            if (Letter1 < Letter2)
                Sign = -1;
        }
    }
    return Sign;
}

long int MostFrequentString(FILE *Source, sl *List) //ищем в открытом файле самую частую строку. возвращает ее смещение от начала файла или -1 при пустом файле
{
    //очень много утечек по памяти
    if (List == NULL)  //если файл пустой, искать нечего
        return (-1);
    if (List->Next == NULL) //если в файле одна строка, ее не с чем сравнивать, она самая частая
        return 0;
    int Count, MFCount = 1, Sign;
    long int MFOffset = 0;
    sl *Pointer;
    while (List != NULL) //сравниваем первую строку со всем списком. потом-вторую. и так далее.
        //если первая строка встречается чаще других, у ее клонов число будет меньше. ну и что, все равно они одинаковые,
        //считаем "от первого вхождения строки"
    {
        Pointer = List;
        Count = 1;
        while (Pointer->Next != NULL) //просматриваем весь остальной список
        {
            Pointer = Pointer->Next;
            Sign = CompareString(Source, List->Offset, Pointer->Offset);
            if (Sign == 0) //если равны, увеличиваем счетчик
                Count++;
        }
        if (Count > MFCount) //сохраняем информацию о самой частой строке
        {
            MFCount = Count;
            MFOffset = List->Offset;
        }
        List = List->Next;
    }
    return MFOffset;
}

void PrintString(FILE *Source, long int Offset, FILE *Output)
{
    fseek(Source, Offset, SEEK_SET);
    int Letter;
    while ( ((Letter = fgetc(Source)) != EOF) && (Letter != '\n') )
        fprintf(Output, "%c", Letter);
    fprintf(Output, "%c", '\n');
    return;
}

void WriteFromList(FILE *Source, FILE *Destination, sl *List)
{
    while (List != NULL)
    {
        PrintString(Source, List->Offset, Destination);
        List = List->Next;
    }
    return;
}

int main(int argc, char **argv)
{
    char *FileName;
    if (argc != 2)
    {
        printf("Error! Wrong count of parametrs\n");
        exit(1);
    }
    FileName = argv[1];
    FILE *Source;
    if ( (Source = fopen(FileName, "r")) == NULL)
    {
        printf("Error! Unable to open file\n");
        exit(1);
    }
    sl *FileList;
    FileList = InitList(Source); //создали список, содержащий "указатели" на строки файла
    if (FileList == NULL)
    {
        printf("File is empty\n");
        exit(1);
    }

    char Mode = '4';
    printf("Chose program mode:\n 1-print most frequent string of file,\n 2-sort text\n");
    scanf("%c", &Mode);
    while ( (Mode != '1') && (Mode != '2') && (Mode != '\0') )
    {
        Mode = '\0'; //чтобы программа не циклилась при нажатии ctrl+d
        scanf("%c", &Mode);
    }


    if (Mode == '1')
    {
        long int Offset = MostFrequentString(Source, FileList);
        printf("Most frequent string:\n");
        PrintString(Source, Offset, stdout);
        DeleteList(FileList);
        fclose(Source);
    }
    if (Mode == '2')
    {
        fseek(Source, 0, SEEK_SET);
        FILE *Additional = fopen ("Additional", "w"); //создаем (открываем) дополнительный файл для записи сортированного исходного в него
        if (Additional == NULL)
        {
            printf("Error! Unable to create additional file\n");
            exit(1);
        }
        SortList(&FileList); //сортируем этот список по длине строк
        WriteFromList(Source, Additional, FileList);
        DeleteList(FileList);
        fclose(Source);
        fclose(Additional);
        if ( (Source = fopen(FileName, "w")) == NULL) //при открытии обнуляется, все в порядке
        {
            printf("Error! Unable to open file\n");
            exit(1);
        }
        if ( (Additional = fopen("Additional", "r")) == NULL)
        {
            printf("Error! Unable to open file\n");
            exit(1);
        }
        int Character;
        while ( (Character = fgetc(Additional)) != EOF)
            fprintf(Source, "%c", Character);
        fclose(Source);
        fclose(Additional);
        truncate("Additional",0); //очищаем дополнительный файл
    }
    return 0;
}