#include <stdio.h>
#include <stdlib.h>

//Beta-2. Есть ввод-вывод, умножение на число, сложение (работоспособность не проверена) и удаление матрицы.
//Удаление матрицы работает и потерь памяти нет, но есть лишний free. Начата работа с пользовательским интерфейсом и switch
//Библиотеки пока нет.

//Beta-3. Начато исправление проблем с памятью. Реже крашится при разных размерах матриц (5*1, 5*2 теперь работают)
//Внимательно следить за размером в malloc
//Похоже, починено. Память не теряется, число free сходится

//Beta-4. Добавлена функция умножения матриц. Работает. Добавлена проверка на некорректно переданную матрицу (CheckEmpty)
// и в результате чуть упрощен код. Добавлена функция подсчета определителя. Пока не работает, но задумка расписана и база есть

//Beta-5. Функция подсчета определителя поправлена. Работает, но: надо очень внмательно проверить перестановку строк
//и работу с вырожденными матрицами. Есть лишний free при вычислении определителя пустой матрицы

//Beta-6. поправлен и доведен до рабочего состояния switch, попралена или добавлена обработка крайних ситуаций в функциях
//обработка ошибочных ситцаций работает. Да, программа в случае некорректных данных забрасывает пользователя ошибками
//но это лучше, чем портить универсальность и самостоятельность функций
//Полностью рабочий вариант программы, но без библиотек
//с valgrind не дружит. Судя по всему, есть проблемы при копировании мартицы: при создании New
//в определителе тоже беды с памятью. что-то тут не так

//Beta-7. поправлена общая проблема с valgrind (было неправильно указано максимальное значение счетчика-перепутаны строки со столбцами)
//Поправлена проблема с valgrind при подсчете определителя. Больше проблем не обнаружено. Можно переписывать под библиотеку

struct Matrix
{
    int StringCount;
    int ColumnCount;
    float **Data;
};

typedef struct Matrix sm;

void InitMatrix(sm **Result, int StringCount, int ColumnCount);
void OutputMatrix(sm *Matrix);
void DeleteMatrix(sm **Source);
int CheckEmpty(sm *Matrix);
sm *MultyplyNumber(sm* Source, float Number);
sm *AddMatrix(sm *Matrix1, sm *Matrix2);
sm *MultyplyMatrix(sm *Matrix1, sm *Matrix2);
float Determinatn(sm *Matrix);

void InitMatrix(sm **Result, int StringCount, int ColumnCount)  //ввод матрицы. на вход-число строк и столбцов, выход-указатель на матрицу-структуру. Две звездочки-чтобы можно было менять "первый элемент"-то есть если указатель-NULL
{
    int i=0,j=0;
    if ((StringCount<=0)||(ColumnCount<=0))
    {
        printf("Incorrect data: Count of strings or columns is zero or less\n"); //в таком случае переданную "матрицу" не меняем никак
        return;
    }

    *Result=(sm*)malloc(sizeof(sm));  //переопределяем, что бы там ни находилось ранее. Теперь это-матрица
    if (*Result!=NULL)   //если все в порядке и память выделить удалось
    {
        (*Result)->StringCount=StringCount;
        (*Result)->ColumnCount=ColumnCount;
        float **Array=(float**)malloc(StringCount*sizeof(float*));
        for (i=0; i<StringCount; i++)
            Array[i]=(float*)malloc(ColumnCount*sizeof(float));

        for (i=0; i<StringCount; i++)
            for (j=0; j<ColumnCount; j++)
                scanf("%f", &Array[i][j]);
        (*Result)->Data=Array;
    }
    else printf("Error! No free memory!\n");
    return;
}

void OutputMatrix(sm *Matrix) //Вывод матрицы. Работает.
{
    if (CheckEmpty(Matrix))
    {
        printf("Matrix is empty. Probably, it may heve been impossible to built or the input data is incorrect\n");
        return;
    }

    for (int i=0; i<(Matrix->StringCount); i++)
    {
        for (int j=0; j<(Matrix->ColumnCount); j++)
            printf("%3.0f", (Matrix->Data)[i][j]); //3.0f- исключительно для читаемости текста
        printf("\n");
    }
    printf("\n");
    return;
}

int CheckEmpty(sm *Matrix) //проверка матрицы на пустоту или неправильно переданные размеры. Для экономии места и упрощения читаемости кода
{
    if (Matrix==NULL) //порядок не менять. В случае, если число строк или столбцов 0 или меньше, на указатель даже не смотрим
        return 1;
    else
    {
        if ((Matrix->StringCount<=0)||(Matrix->ColumnCount<=0))
            return 1;
        else return 0;
    }
}

sm *MultyplyNumber(sm* Source, float Number)  //умножение матрицы на число. Создает новую матрицу, не портя старую, в новом месте памяти
{
    if (CheckEmpty(Source))
    {
        printf("Incorrect data: Matrix is empty or  its sizes are wrong\n");
        return NULL;
    }

    int i,j;
    sm *New=(sm*)malloc(sizeof(sm));
    New->StringCount=Source->StringCount;  //сперва копируем старую матрицу в N, затем умножаем на число, затем возвращаем N. Чтобы не портить M.
    New->ColumnCount=Source->ColumnCount;
    float **Array=(float**)malloc(New->StringCount*sizeof(float*));
    for (i=0; i<New->StringCount; i++)
        Array[i]=(float*)malloc(New->ColumnCount*sizeof(float));

    for (i=0; i<New->StringCount; i++)
        for (j=0; j<New->ColumnCount; j++)
            Array[i][j]=(Source->Data)[i][j];

    New->Data=Array;
    for (i=0; i<(New->StringCount); i++)
    {
        for (j=0; j<(New->ColumnCount); j++)
            (New->Data)[i][j]*=Number;
    }
    return New;
}

sm *AddMatrix(sm *Matrix1, sm *Matrix2)  //сложение матриц. Создает новую матрицу, не портя старую, в новом месте памяти. Сделано из функции умножения на число
{
    if ((Matrix1==NULL)||(Matrix1->ColumnCount<=0)||(Matrix1->StringCount<=0))
    {
        printf("Incorrect data: first matrix is empty or its sizes are incorrect\n");
        return NULL;
    }

    if ((Matrix2==NULL)||(Matrix2->ColumnCount<=0)||(Matrix2->StringCount<=0))
    {
        printf("Incorrect data: second matrix is empty or its sizes are incorrect\n");
        return NULL;
    }

    if ((Matrix1->ColumnCount!=Matrix2->ColumnCount)||(Matrix1->StringCount!=Matrix2->StringCount))
    {
        printf("Addition is impossible: matrices have different sizes\n");
        return NULL;
    }

    int i,j;
    sm *New=(sm*)malloc(sizeof(sm));
    New->StringCount=Matrix1->StringCount;  //сперва копируем старую матрицу в New и пишем уже туда. Чтобы не портить Matrix1
    New->ColumnCount=Matrix1->ColumnCount;

    float **Array=(float**)malloc(New->StringCount*sizeof(float*));
    for (i=0; i<New->StringCount; i++)
        Array[i]=(float*)malloc(New->ColumnCount*sizeof(float));

    for (i=0; i<New->StringCount; i++)
        for (j=0; j<New->ColumnCount; j++)
            Array[i][j]=(Matrix1->Data)[i][j];
    New->Data=Array;

    for (i=0; i<(New->StringCount); i++)
    {
        for (j=0; j<(New->ColumnCount); j++)
            (New->Data)[i][j]+=(Matrix2->Data)[i][j];
    }
    return New;
}

void DeleteMatrix(sm **Source)
{
    if (*Source!=NULL)
    {
        for (int i=0; i<(*Source)->StringCount;i++) //освобождение памяти
            free((*Source)->Data[i]);
        free((*Source)->Data);
        free(*Source);   //более-менее работает, но: крашится, если строк больше, чем столбцов (???) и valgrind выдает один лишний free. Потерь памяти нет
    }
    return;
}

sm *MultyplyMatrix(sm *Matrix1, sm *Matrix2) //умножение матриц. вроде работает.
{
    if (CheckEmpty((Matrix1))||CheckEmpty(Matrix2))
    {
        printf("Error! Empty matrix\n");
        return NULL;
    }

    if (Matrix1->ColumnCount!=Matrix2->StringCount)
    {
        printf("These matrices can't be multiplied because their sizes are incompatible\n");
        return NULL;
    }

    int i,j,k;
    sm *New=(sm*)malloc(sizeof(sm));

    float **Array=(float**)malloc((Matrix1->StringCount)*sizeof(float*));
    for (i=0; i<Matrix1->StringCount; i++)
        Array[i]=(float*)malloc((Matrix2->ColumnCount)*sizeof(float));

    for (i=0; i<Matrix1->StringCount; i++)  //обнуляем матрицу, чтобы можно было в нее складывать
        for (j=0; j<Matrix2->ColumnCount; j++)
            Array[i][j]=0;

    for (i=0; i<Matrix1->StringCount; i++)
        for (j=0; j<Matrix2->ColumnCount; j++)
            for (k=0; k<Matrix1->ColumnCount; k++)
                Array[i][j]+=((Matrix1->Data)[i][k]*(Matrix2->Data)[k][j]);

    New->StringCount=Matrix1->StringCount;
    New->ColumnCount=Matrix2->ColumnCount;
    New->Data=Array;
    return New;
}

float Determinant(sm *Matrix)
{
    if (CheckEmpty(Matrix))
    {
        printf("Error: Matrix is empty\n");
        return 0;
    }

    if (Matrix->ColumnCount!=Matrix->StringCount)
    {
        printf("Error: Matrix is not square\n");
        return 0;
    }

    float Result=1,Index=0;
    int i=0,j=0,k=0, p=0, Sign=1;
    float **Backup=(float**)malloc(Matrix->StringCount*sizeof(float*)); //создадим копию матрицы, чтобы не портить исходную при подсчете определителя
    for (i=0;i<Matrix->StringCount; i++)
        Backup[i]=(float*)malloc(Matrix->ColumnCount*sizeof(float));

    for (i=0; i<Matrix->StringCount; i++)
        for (j=0; j<Matrix->ColumnCount; j++)
            Backup[i][j]=(Matrix->Data)[i][j];

    for (i=0; i<Matrix->StringCount; i++) //пытаемся привести матрицу к верхнедиагональному виду
    {
        k=i;
        while ((k<Matrix->StringCount)&&(Backup[k][i]==0)) ++k; //ищем самый ранний ненулевой элемент в i-м столбце. Порядок менять нельзя, т.к короткая логика. Если k=числу строк и при первом условии не выйдем, получим ошибку
        if (k>=Matrix->StringCount) //если это случилось, значит, выбрать ненулевой элемент нельзя и значит, определитель равен 0.
        {
            for (i=0; i<Matrix->StringCount; i++) //выходим из функции и удаляем уже сохданный массив backup
                free(Backup[i]);
            free(Backup);
            return 0;
        }
        if (Backup[i][i]==0) //если не вышли из цикла, но на (i,i) ноль, то переставляем строки
        {
            float *Exchanger=NULL;
            Exchanger=Backup[i];  //переставляем строки так, чтобы первый ненулевой элемент оказался на месте (i,i)
            Backup[i]=Backup[k];
            Backup[k]=Exchanger;
            Sign*=(-1); //при перестановке строк знак определителя меняется
        }

        for (j=i+1; j<Matrix->StringCount; j++)
        {
            Index=Backup[j][i]/Backup[i][i]; //сохраняем, так как нужно при изменении матрицы, но в цикле зануляется, так как зануляется "первый" элемент изменяемой строки
            for (p=0; p<Matrix->StringCount; p++) //p=i, так как при правильной работе алгоритма первые элементы уже должны были обнулиться
                Backup[j][p]=Backup[j][p]-Backup[i][p]*Index; //вычитаем строку с соответствующим коэфициентом, чтобы занулить ее
        }
    }

    for (i=0; i<Matrix->StringCount; i++)
        Result*=Backup[i][i];
    Result*=Sign;
    for (i=0; i<Matrix->StringCount; i++)
        free(Backup[i]);
    free(Backup);
    return Result;
}

int main()
{
    int StringCount1=0,ColumnCount1=0, StringCount2=0, ColumnCount2=0, Mode=0, Repeat=1;
    sm *Matrix1=NULL, *Matrix2=NULL, *ResultMatrix=NULL;
    float Number=0;
    while(Repeat!=0)
    {
        printf("Chose program Mode:\n 1: Multyply matrix by number\n 2: Add two matrices\n 3: Multyply two matrices\n 4: Calculate the determinant of a matrix\n");
        scanf("%d", &Mode);
        switch(Mode)
        {
             case 1:
                      printf("Input count of strings\n");
                      scanf("%d",&StringCount1);
                      printf("Input count of columns\n");
                      scanf("%d",&ColumnCount1);

                      printf("Input Matrix\n");
                      InitMatrix(&Matrix1, StringCount1, ColumnCount1);
                      printf("Initial matrix:\n");
                      OutputMatrix(Matrix1);

                      printf("Input number, you want to multylpy matrix by\n");
                      scanf("%f",&Number);

                      ResultMatrix=MultyplyNumber(Matrix1, Number);
                      printf("Result:\n");
                      OutputMatrix(ResultMatrix);

                      DeleteMatrix(&ResultMatrix);
                      DeleteMatrix(&Matrix1);
                      break;
             case 2:
                      printf("Input count of strings of matrices\n"); //для сложения матрицы обязаны быть одинакового размера. Обработка матриц разных размеров есть в самой функции. Так что здесь можно не тратить время на ввод размеров второй матрицы
                      scanf("%d",&StringCount1);
                      printf("Input count of columns of matrices\n");
                      scanf("%d",&ColumnCount1);

                      printf("Input first matrix\n");
                      InitMatrix(&Matrix1, StringCount1, ColumnCount1);
                      printf("First matrix:\n");
                      OutputMatrix(Matrix1);

                      printf("Input second matrix\n");
                      InitMatrix(&Matrix2, StringCount1, ColumnCount1);
                      printf("Second matrix:\n");
                      OutputMatrix(Matrix2);

                      ResultMatrix=AddMatrix(Matrix1, Matrix2);
                      printf("Result:\n");
                      OutputMatrix(ResultMatrix);

                      DeleteMatrix(&Matrix1);
                      DeleteMatrix(&Matrix2);
                      DeleteMatrix(&ResultMatrix);
                      break;
             case 3:
                      printf("Input count of strings of first matrix\n");
                      scanf("%d",&StringCount1);
                      printf("Input count of columns of first matrix\n");
                      scanf("%d",&ColumnCount1);

                      printf("Input first matrix\n");
                      InitMatrix(&Matrix1, StringCount1, ColumnCount1);
                      printf("First matrix:\n");
                      OutputMatrix(Matrix1);

                      printf("Input count of strings of second matrix\n");
                      scanf("%d",&StringCount2);
                      printf("Input count of columns of second matrix\n");
                      scanf("%d",&ColumnCount2);

                      printf("Input second matrix\n");
                      InitMatrix(&Matrix2, StringCount2, ColumnCount2);
                      printf("Second matrix:\n");
                      OutputMatrix(Matrix2);

                      ResultMatrix=MultyplyMatrix(Matrix1, Matrix2);
                      printf("Result:\n");
                      OutputMatrix(ResultMatrix);

                      DeleteMatrix(&Matrix1);
                      DeleteMatrix(&Matrix2);
                      DeleteMatrix(&ResultMatrix);
                      break;
             case 4:
                      printf("Input count of strings\n");
                      scanf("%d",&StringCount1);
                      printf("Input count of columns\n");
                      scanf("%d",&ColumnCount1);

                      printf("Input Matrix\n");
                      InitMatrix(&Matrix1, StringCount1, ColumnCount1);
                      printf("Initial matrix:\n");
                      OutputMatrix(Matrix1);

                      printf("Determinant: %3.0f\n", Determinant(Matrix1));                       //при неквадратной или "пустой" матрице выводит 0. Как это поменять, непонятно, функция числовая и всегда возврящает число

                      DeleteMatrix(&Matrix1);
                      break;
        }
        printf("Do you want to continue? No-0, Yes-everything else\n");
        scanf("%d", &Repeat);
    }
    return 0;
}
