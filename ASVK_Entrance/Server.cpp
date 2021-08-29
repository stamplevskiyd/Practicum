#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>

//сам сервер. работает с клиентами, пока не получит в терминале символ n после вопроса
//так сделано исключительно для удобства использования

#define ERROR_S "Server error: "
#define DEFAULT_PORT 1306
#define BUFFER_SIZE 1024

//обработка такая же, как в основной программе

class Tower;

std::vector<Tower *> List; //список указателей на все вышки сети

class Tower {
    bool Availability; //доступна (исправна) ли вышка в данный момент
    int User_Count; //число абонентов, подключенных именно к этой вышке
    std::vector<Tower *> Connections; //вектор указателей на вышки, с которыми физически связана данная вышка
    std::vector<Tower *> Accessible;  //вектор указателей на вышки, к которым у данной есть доступ (логическая свзяанность)
    long int Error_value; //Показатель суммарной мобильной связанности при поломке именно этой вышки
    std::string Name;
    bool Counted; //проверили ли уже этот элемент при подсчете
public:
    Tower(int Count = 0, std::string N = "");

    bool Connect_Physical(Tower &B); //создание физического соединения между вышками
    bool Connect_Logical(Tower &B);

    bool Is_Connected(Tower &B); //проверка на то, есть ли физическое соединение у исходной вышки с указанной
    void Turn_On() { Availability = true; } //включение вышки обратно в сеть
    void Turn_Off() { Availability = false; } //отключение вышки
    bool Connect_All(Tower &B); //подключить вышку в сеть через канал с B. То есть логически связать со всеми вышками, с которыми связана B
    void Reconfigure();
    void Set_User_Count(int V) { User_Count = V; }
    int Get_Users_Count() const { return User_Count; }
    bool Is_Available(Tower &B) const { return B.Availability; }
    void Count_Error_Value();
    long int Get_Error_Value() { return Error_value; }
    std::string Get_Name() { return Name; }
};

bool Tower::Is_Connected(Tower &B) {
    int i;
    bool Answer = false;
    for (i = 0; i < Connections.size(); i++) //подключение симметричное, все равно, по чьей таблице смотреть
        if (Connections[i] == &B)
            Answer = true;
    return Answer;
}

bool Tower::Connect_Physical(Tower &B) //true-если подключение удалось
{
    if (&B != this) //подключение вышки к самой себе не несет пользы, а только забивает вектор
        if (!Is_Connected(B)) //чтобы не дублировать в векторе, если подключение уже есть
        {
            if ((Is_Available(*this)) &&
                (Is_Available(B))) //также если какая-то из вышек недоступна, подключать их не надо
            {
                Connections.push_back(&B);
                B.Connections.push_back(this);
                Connect_Logical(B); //если связаны физически, то связаны и логически
                return true;
            }
        }
    return false;
}

bool
Tower::Connect_All(Tower &B) { //true-если удалось хоть одно подключение. то есть-если найдены новые логические связи
    int i;
    bool Result = false;
    Connect_Logical(B); //а если не подключены изгачатьно?
    for (i = 0; i < B.Accessible.size(); i++)
        Result = Result || Connect_Logical(*B.Accessible[i]); //все равно там есть проверка необходимости подключения
    if (Result) { //если на этом этапе подключена хоть одна новая вышка, значит, могут быть новые логические связи. иначе-нет, искать бессмысленно
        for (i = 0; i < B.Accessible.size(); i++) {
            Connect_All(*B.Accessible[i]); //все равно там есть проверка необходимости подключения
            B.Accessible[i]->Connect_All(*this); //для остальных тоже пересчитываем
        }
    }
    return Result;
}

void Tower::Reconfigure() {
    int i, j;
    for (i = 0; i < List.size(); i++) //очищаем все логические соединения всех вышек, чтобы создать новые
        List[i]->Accessible.clear();
    for (i = 0; i < List.size(); i++)
        for (j = 0; j <
                    List.size(); j++) //проверка на доступность, как и отсутствие соединения, тоже не даст зайти куда не надо
            if ((i != j) && (Is_Available(*List[i])) && (Is_Available(*List[j])) && (List[i]->Is_Connected(*List[j])))
                List[i]->Connect_All(
                        *List[j]); //подключает ко всем связанным только если: это не та же самая вышка, новая вышка доступна и подключена физически
}

Tower::Tower(int Count, std::string N) {
    Availability = true;
    User_Count = Count;
    Name = N;
}

bool Tower::Connect_Logical(Tower &B) {
    std::vector<Tower *>::iterator finder;
    if ((finder = find(Accessible.begin(), Accessible.end(), &B)) ==
        Accessible.end()) //если не нашли - то есть если доступа пока нет
    {
        if (&B != this) {
            Accessible.push_back(&B); //добавляем в список и возвращаем истину
            B.Accessible.push_back(this);
            return true;
        }
    }
    return false;
}

void Tower::Count_Error_Value() { //считает мобильную связанность системы при отключении именно этой вышки
    int i, j, Users, Value = 0;
    Turn_Off();
    Reconfigure();
    for (i = 0; i <
                List.size(); i++) //пока ничего и никого не посчитали. Каждая вышка должна считаться только один раз, поэтому и нужна такая проверка
        List[i]->Counted = false;
    for (i = 0; i < List.size(); i++) //смотрим статичтику для i-й вышки при отключении исходной
    {
        if (!List[i]->Counted) {
            if (List[i] == this)
                Value += User_Count; //для отключенной вышки число связей равно числу пользователей-связаться можно только с собой
            else {
                Users = 0;
                if (List[i]->Accessible.empty())
                    Users = List[i]->Get_Users_Count();
                else {
                    for (j = 0; j < List[i]->Accessible.size(); j++) //по j проходим по всем вышкам, доступным из i-й
                        if (!List[i]->Accessible[j]->Counted) {
                            Users += List[i]->Accessible[j]->Get_Users_Count(); //получаем число пользователей по этому виртуальному подключению. См. формулу
                            List[i]->Accessible[j]->Counted = true;
                        }
                    Users += List[i]->Get_Users_Count(); //вышка считается не подключенной к себе
                }
                Value += Users * Users;
            }
            List[i]->Counted = true;
        }
    }
    Error_value = Value;
    Turn_On(); //возвращаем все на место
    Reconfigure();
}

char Skip(FILE *Text) //получает первый значащий символ, идущий СРАЗУ ПОСЛЕ предыдущего считанного
{
    char c = fgetc(Text);
    while ((c == ' ') || (c == '\n') || (c == '\t'))
        c = fgetc(Text);
    return c;
}

std::string Get_Tower_Name(FILE *Text) //получает из файла имя вышки
{
    char c = fgetc(Text); //первый символ получить нужно обязательно
    std::string One_Name;
    if (c != '\'')
        c = Skip(Text);
    if (c != '\'') { //получение первого имени
        std::cout
                << "Error! Name of tower is lost\n"; //на эту ошибку можно наткнуться только если дойти до конца файла, то есть вообще не закрыть строку
        return "";
    } //сейчас смотрим на одинарные кавычки. значит, следующий символ-первый в строке
    c = fgetc(Text);
    if (c == '\'') {
        std::cout << "Error! Empty name\n";
    }
    while (c != '\'') {
        One_Name += c;
        c = fgetc(Text);
        if (c == EOF) {
            std::cout << "Error! String (tower's name) was not closed\n";
            return "";
        }
    } //считывание закончили, сейчас "смотрим" на ', следующим считаем символ сразу после
    return One_Name;
}

bool Init(FILE *Text) { //в любом случае, независимо от корректности, закрывает файл
    int Index1, Index2, i;
    Tower *T1;
    std::string Name1, Name2;
    char c = fgetc(Text);
    if (c != '{')
        c = Skip(Text);
    if (c != '{') {
        std::cout << "Error! Opening bracket is lost\n";
        return false;
    }
    c = fgetc(Text); //считываем символ, идущий после открывающей скобки
    if (c != '[')
        c = Skip(Text);
    if (c != '[') {
        std::cout << "Error! Opening bracket is lost\n";
        return false;
    }
    c = fgetc(Text); //переходим на символ после скобки
    bool Closed = false;
    while (!Closed) {
        if (c != '[')
            c = Skip(Text);
        if (c != '[') {
            std::cout << "Error! Opening bracket is lost\n";
            return false;
        }
        Name1 = Get_Tower_Name(Text);
        c = fgetc(Text);
        if (c != ',')
            c = Skip(Text);
        if (c != ',') { //между двумя именами может идти только запятая
            std::cout << "Error! No comma between tower names\n";
            return false;
        }
        Name2 = Get_Tower_Name(Text);
        bool Added1 = false, Added2 = false;
        if (Name1 != "")
            for (i = 0; i < List.size(); i++)
                if (List[i]->Get_Name() == Name1) {
                    Added1 = true;
                    Index1 = i;
                }
        if (!Added1) {
            T1 = new Tower(0, Name1);
            List.push_back(T1);
            Index1 = List.size() - 1;//добавили в список последним
        }
        if (Name2 != "") //можно было бы обойтись одним циклом, но тогда не сработает, если оба имени одинаковые
            for (i = 0; i < List.size(); i++)
                if (List[i]->Get_Name() == Name2) {
                    Added2 = true;
                    Index2 = i;
                }
        if ((Name1 == "") ||
            (Name2 == "")) //пустые строки означают ошибку. а какую ошибку-выводит функция получения имени
            return false;;
        if (!Added2) {
            T1 = new Tower(0, Name2);
            List.push_back(T1);
            Index2 = List.size() - 1;
        }
        List[Index1]->Connect_Physical(
                *List[Index2]); //просто подключаем физически по уже полученным индексам. Если были подключены-не проблема, у функции есть проверка
        if (c != ']')
            c = Skip(Text);
        if (c != ']') {
            std::cout << "Error! Closing bracket is lost\n";
            return false;
        }
        c = Skip(Text);
        if (c == ']')
            Closed = true;
        else if (c != ',') {
            c = Skip(Text);
            if (c != ',') {
                std::cout << "Error! No comma between connections\n";
                return false;
            }
        }
    }

    //конец первого ряда данных с заданием вышек и связей между ними

    if (c != ',')
        c = Skip(Text);
    if (c != ',') {
        std::cout << "Error! No comma between sections\n";
        return false;
    }
    if (c != '{')
        c = Skip(Text);
    if (c != '{') {
        std::cout << "Error! No bracket in second section\n";
        return false;
    }
    while (c != '}') {
        std::string Name;
        int Num = 0, Index = 0;
        bool In_List = false;
        Name = Get_Tower_Name(Text);
        for (i = 0; i < List.size(); i++) {
            if (List[i]->Get_Name() == Name) {
                In_List = true;
                Index = i; //понадобится чуть позже, при записи числа подключений
            }
        }
        if (!In_List) {
            T1 = new Tower(0,
                           Name); //этой вышки нет в списке соединений. Она ни к чему не подключена, но в сети то есть. Ноль изменим чуть позже
            List.push_back(T1);
            Index = List.size() - 1;
        }
        if (c != ':')
            c = Skip(Text);
        if (c != ':') //если у вышки не указано число пользователей, будем считать, что оно равно нулю
            List[Index]->Set_User_Count(0);
        else {
            c = Skip(Text);
            if (c == '-') {
                std::cout << "Wrong input: number of users can't be negative\n";
                return false;
            }
            while ((c >= '0') && (c <= '9')) {
                Num *= 10;
                Num += c - '0';
                c = fgetc(Text);
            }
            List[Index]->Set_User_Count(Num);
        }
        if ((c != ',') && (c != '}'))
            c = Skip(Text);
        if ((c != ',') && (c != '}')) { //если запятой все-таки нет
            std::cout << "Wrong input: no comma between sections\n";
            return false;
        }
    }
    c = fgetc(Text); //считали предыдущую закрывающую скобку, теперь смотрим финальную
    if (c != '}')
        c = Skip(Text);
    if (c != '}') {
        std::cout << "Error! No final bracket!\n";
        return false;
    }
    return true;
}

void Free_Memory() {
    for (int i = 0; i < List.size(); i++)
        delete (List[i]);
}

bool Got_end_Symbol(const char *msg) {
    for (int i = 0; i < BUFFER_SIZE; i++)
        if (msg[i] == '#')
            return true;
    return false;
}

void Clear_Buffer(char *buf) {
    buf[0] = '\0';
}

int main() {
    int client, server;
    char End_Symbol;
    char Symbol;
    struct sockaddr_in server_address; //информация о адресе сервера
    client = socket(AF_INET, SOCK_STREAM, 0); //создание сокета
    if (client < 0) { //то есть если функция вернула -1 -то есть ошибка
        std::cout << ERROR_S << "Establishing socket error\n";
        exit(0);
    }
    std::cout << "SERVER: socket was successfully created\n";
    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    int ret;

    ret = bind(client, reinterpret_cast<struct sockaddr *>(&server_address),
               sizeof(server_address)); //соединение сокета с информацией о сети, в которой он запускается
    if (ret < 0) {
        std::cout << ERROR_S << "binding error\n";
        return -1;
    }

    socklen_t size = sizeof(server_address);
    std::cout << "Server is listening\n";

    listen(client, 5); //слушаем клиента. втророй аргумент-размер очереди
    while (true) {
        std::cout << "New client!\n";
        bool This_user_has_finished = false;
        server = accept(client, reinterpret_cast<struct sockaddr *>(&server_address),
                        &size);
        if (server < 0) {
            std::cout << ERROR_S << "Can't accept client\n";
            exit(0);
        }
        bool Is_Exit = false;
        FILE *Additional;
        int Received;
        while (!This_user_has_finished) {
            Additional = fopen("1a2q3s4w5d.txt",
                               "wt"); //просто текстовый файл со случайным названием для хранения данных
            while (Symbol != '!') {
                Received = recv(server, &Symbol, 1, 0);
                if (Received)
                    fwrite(&Symbol, sizeof(char), Received,
                           Additional); //да, в файл запишется лишний символ. Но структура основной программы такова, что он все равно проигнорируется, так что даже нет смысла пытаться его убрать, мнеее эффективно будет
            }

            std::cout << "File is received\n";
            //обработка файла
            bool Correctness; //указывает, надо ли выводить данные или была ошибка
            int i;
            long int Min;
            fclose(Additional);
            if ((Additional = fopen("1a2q3s4w5d.txt", "rt")) == NULL) {
                char *buffer;
                strcpy(buffer, "Error on server\n");
                send(server, buffer, BUFFER_SIZE, 0);
                std::cout << "Error! Unable to open additional file\n";
                Correctness = false;
            } else {
                bool Data_Is_Correct;
                char *buffer;
                Data_Is_Correct = Init(Additional);
                fclose(Additional);
                truncate("1a2q3s4w5d.txt", 0);
                if (!Data_Is_Correct){
                    char Error[] = "Data is incorrect";
                    for (int i = 0; i < sizeof(Error); i++) //посылаем клиенту сообщение об ошибке
                        send(server, &Error[i], 1, 0);
                    Symbol = '%';
                    send(server, &Symbol, 1, 0); //посылаем код завершения
                }

                List[0]->Count_Error_Value(); //при корректном вводе хоть один элемент списка есть всегда.
                Min = List[0]->Get_Error_Value(); //пусть минимумом будет первое посчитанное значение, чтобы было с чем сравнивать
                for (i = 1; i < List.size(); i++)//нулевой уже посчитали
                {
                    List[i]->Count_Error_Value();
                    if (List[i]->Get_Error_Value() < Min)
                        Min = List[i]->Get_Error_Value();
                }
                std::cout << "File processing is finished\n";
                Correctness = true;
            }

            End_Symbol = '*'; //послали граничный символ
            send(server, &End_Symbol, 1, 0);
            if (Correctness) {
                for (i = 0; i < List.size(); i++)
                    if (List[i]->Get_Error_Value() == Min) {
                        Symbol = '[';
                        send(server, &Symbol, 1, 0);
                        for (int j = 0; j < List[i]->Get_Name().size(); j++) {
                            Symbol = List[i]->Get_Name()[j];
                            send(server, &Symbol, 1, 0);
                        }
                        Symbol = ']';
                        send(server, &Symbol, 1, 0);
                    }
                Free_Memory();
            }
            End_Symbol = '%';
            send(server, &End_Symbol, 1, 0);
            recv(server, &Symbol, 1, MSG_PEEK); //получим сообщение для проверки без потери пакетов благодаря MSG_PEEK
            if (Symbol == '#')
                This_user_has_finished = true;
        }
        std::cout << "Do you want to finish? y/n\n"; //сделано исключительно для удобства
        char c;
        std::cin >> c;
        if (c == 'n')
            break;
    }
    std::cout << "End of session\n";
    return 0;
}
