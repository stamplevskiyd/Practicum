#include <iostream>

//разобраться со static и const в функциях

//Beta-1. Работает добавление элемента и вывод

typedef int base;

struct link{
    base Data; //поле данных, пока нулевое
    link *Next; //поле-указатель на следующий элемент
};

void Deletelist(link *First)
{
    link *Additional = First;
    while (Additional->Next->Next)
        Additional = Additional->Next;
    delete Additional->Next;
    Additional->Next = 0;
    Deletelist(First);
}

class Pack{
    link *Root; //в private содержится заголовок списка, а значит, только из класса есть доступ к списку
public:
    void Out();
    Pack() //конструктор умолчания. Можно потом вынести из класса
    {
        link *Element = new link;
        Element->Data = 0;
        Element->Next = 0;
        Root = Element;
    }
    void AddElement(base Value);
    ~Pack() {Deletelist(Root);} //реализовано так потому, что удаление рекурсивное, а рекурсивно вызывать деструктор-ну такое
};

void Pack::AddElement(base Value)
{
    link *Element = new link;
    Element->Data = Value;
    Element->Next = 0;
    link *Additional = Root; //создаем дополнительную ссылку и идем по ней, пока не найдем нужное место
    while (Additional->Next)
        Additional = Additional->Next;
    Additional->Next = Element;
}

void Pack::Out()
{
    link *Additional = Root;
    while (Additional) {
        std::cout << Additional->Data << std::endl;
        Additional = Additional->Next;
    };
}
int main() {
    std::cout << "Hello, World!" << std::endl;
    Pack A;
    for (int i = 1; i < 10; i++)
        A.AddElement(i);
    A.Out();
    return 0;
}
