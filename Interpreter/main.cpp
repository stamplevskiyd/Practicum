#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include <stack>

//Version 2.1.1
//Почему-то заработал if с else и без. Причины не совсем понятны

using namespace std;

FILE *Text;

void Output_Error_String() { //вызывается только при обнаружении ошибки, так что все равно, куда в файле мы перейдем
    char c = '1';
    while ((c != '\n') && (ftell(Text) != 0)) {
        if ((ftell(Text) > 1) && (c != '\n') && (c != EOF))
            fseek(Text, -2 * sizeof(char), SEEK_CUR); //переходим на начало ошибочной строки
        else
            break; //в таком случае в c уже считан первый символ первой строки
        c = fgetc(Text);
    }
    while ((c == '\n') || (c == '\t') || (c == ' '))
        c = fgetc(Text);
    while ((c != '\n') && (c != EOF)) {
        cout << c;
        c = fgetc(Text);
    }
    cout << endl;
}

enum Lex_Type {
    LEX_NULL, LEX_AND, LEX_BEGIN, LEX_ID, LEX_BOOL,
    LEX_READ, LEX_TRUE, LEX_FALSE, LEX_WRITE,
    LEX_STRING, LEX_FIN, LEX_NUM, LEX_DIVIDE, LEX_MULTIPLY,
    LEX_ADD, LEX_SUBTRACT, LEX_OR, LEX_THEN, LEX_ELSE,
    LEX_EQ, LEX_NEQ, LEX_LEQ, LEX_GEQ, LEX_L, LEX_G,
    LEX_ASSIGN, LEX_END, LEX_IF, LEX_INT, LEX_NOT, LEX_WHILE,
    LEX_SEMICOLON, LEX_OPEN_BRACKET, LEX_CLOSE_BRACKET,
    LEX_OPEN_BRACE, LEX_CLOSE_BRACE, LEX_COMMA, LEX_TEXT, LEX_MOD,
    LEX_BREAK, LEX_DO, LEX_LOGIC, POLIZ_LABEL, POLIZ_ADDRESS, POLIZ_GO,
    POLIZ_FGO, POLIZ_BREAK
};

struct Error {
    enum Error_type {
        WRONG_IDENTIFIER, UNKNOWN_ERROR,
        UNCLOSED_COMMENT, WRONG_CHARACTER,
        UNOPENED_COMMENT, UNCLOSED_STRING
    };
    Error(Error_type T, string str) : Type(T), Wrong_Lex(str) {};
    Error(Error_type T, char C1 = '0') : Type(T), c(C1) {};
    Error_type Type;
    string Wrong_Lex;
    char c;
};

class Ident {
    string Name;
    bool Declare;  //обЪявлен или нет
    Lex_Type Type;
    bool Assign; //присвоено значение или нет
    int Value;
public:
    Ident() {
        Declare = false;
        Assign = false;
        Type = LEX_NULL;
        Value = 0;
    }
    Ident(const string &N) {
        Type = LEX_NULL;
        Value = 0;
        Name = N;
        Assign = false;
        Declare = false;
    }
    bool operator==(const string &s) const { return (Name == s); } //именно его и не хватало для работы put и find
    string Get_Name() const { return Name; }
    bool Get_Declare() const { return Declare; }
    bool Get_Assign() const { return Assign; }
    void Make_Declared() { Declare = true; }
    void Make_Assigned() { Assign = true; }
    int Get_Value() const { return Value; }
    void Set_Value(int V) { Value = V; }
    Lex_Type Get_Type() const { return Type; }
    void Set_Type(Lex_Type LT) { Type = LT; }
};

class Lex {
    Lex_Type Type;
    int Lex_Value; //номер в таблице для лексем-строк и переменных
public:
    Lex(Lex_Type T = LEX_NULL, int V = 0) {
        Type = T;
        Lex_Value = V;
    }
    Lex_Type Get_Lex_Type() const { return Type; }
    int Get_Lex_Value() const { return Lex_Value; }
    void Set_Lex_Value (int v) {Lex_Value = v;}
    friend ostream &operator<<(ostream &out, const Lex &L); //перегрузка вывода лексемы. Нужно хотя бы для проверки
    static Lex Lex_TW(string N); //перевод строки из таблицы в лексему. возвращает без значения и строки, вроде логично
};

class Scanner {
    char c;
    bool Finish;
    void gc() { c = fgetc(Text); }
    enum State {
        Beginning, Ident, Number, Comment,
        String, Comparsion, Comment_Division,
        Division
    };
    State CurrentState;
public:
    friend void Output_Error_String();
    Scanner(const char *Name = "Test1.txt") //в конструкторе сразу открываем файл
    {
        CurrentState = Beginning;
        Finish = false; //если наткнулись на символ конца файла-собачку
        if ((Text = fopen(Name, "r")) == NULL) {
            cout << "Unable to open file!\n";
            throw "File_Error";
        }
    }
    ~Scanner() { if (Text != NULL) fclose(Text); }
    Lex Get_Lex();
    static string TW[], TD[];    //TW - таблица служебных слов, TD- таблица ограничителей
    bool Belongs_to_TD(char c);
};

vector<Ident> TID; //таблица идентификаторов
vector<string> TS; //таблица строк. чтобы получить строку, обращаемся к ее индексу в таблице
string Scanner::TD[] = {"", ";", "@", ",", ":", "=", "(", ")", "<", ">", "+",
                        "-", "*", "/", "<=", ">=", "!=", "{", "}", "%",
                        "END_OF_ARRAY"}; //END_OF_ARRAY- для более простой реализации поиска
string Scanner::TW[] = {"", "and", "boolean", "else", "if", "false", "int", "not", "or",
                        "program", "read", "true", "while", "write", "string", "break", "do", "END_OF_ARRAY"};

class Parser {
    Lex Current_Lex;
    Lex_Type Current_Type;
    int Current_Value;
    Scanner Scan;
    bool In_Cycle = false;
    bool In_If = false;
    bool Recursive_Assignment = false;
    int Recursion = 0;
    stack<Lex_Type> Lex_Stack; //стек с типами выражений. без него не получается
    void Program();
    void B();
    void S();
    void E();
    void E1();
    void T();
    void F();
    void Initialisation();
    void eq_bool();
    void check_op();
    void check_id();
    void check_not();
    void eq_type();
    void check_id_in_read();
    void Get_Lexeme() {
        Current_Lex = Scan.Get_Lex();
        Current_Type = Current_Lex.Get_Lex_Type();
        Current_Value = Current_Lex.Get_Lex_Value();
    };
public:
    Parser(const char *Name) : Scan(Name) {} //конструктор по умолчанию
    void Analyse();
    vector <Lex> Poliz;
};

void Parser::Analyse() {
    Get_Lexeme();
    Program();
    if (Current_Type != LEX_FIN) {
        Output_Error_String();
        throw "Error! Not finished";
    }
    //for (Lex l: Poliz)
    //    cout << l;
}

void Parser::Initialisation() {
    int Number; //номер идентификатора в таблице
    bool Identified = true;
    while (Identified) { //если не удалось найти ни одного объявления переменной, переменная станет false и мы выйдем из цикла
        switch (Current_Type) {
            case LEX_INT:
                while (Current_Type != LEX_SEMICOLON) { //идем до конца описания целых переменных
                    Get_Lexeme();
                    if (Current_Type == LEX_ID) //получили лексему int, затем-лексему-идентификатор
                    {
                        Number = Current_Lex.Get_Lex_Value(); //получаем номер соответствующего ИДЕНТИФИКАТОРА в таблице TID
                        if (TID[Number].Get_Declare()){
                            cout << "Error! Second declaration is forbidden\n";
                            Output_Error_String();
                            throw Current_Lex;
                        }
                        TID[Number].Make_Declared(); //переменная уже объявлена
                        TID[Number].Set_Type(LEX_NUM); //указываем, что это-целое число
                        Get_Lexeme(); //лексема-идентификатор
                        if (Current_Type == LEX_ASSIGN) {
                            Get_Lexeme(); //значение, которое будет присвоено
                            if (Current_Type == LEX_NUM) { //лексема-целое значение
                                TID[Number].Set_Value(Current_Lex.Get_Lex_Value()); //пишем значение в таблицу
                                TID[Number].Make_Assigned(); //указываем, что он уже был присвоен
                            }
                        }
                        else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                            Output_Error_String();
                            throw "Wrong int initialisation";
                        } //в случае, если  при отсутствии значения идет что-то лишнее
                    }
                    else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                        Output_Error_String();
                        throw "Wrong int initialisation";
                    } //для случая int a, ...
                }
                break;
            case LEX_BOOL:
                while (Current_Type != LEX_SEMICOLON) {
                    Get_Lexeme();
                    if (Current_Type == LEX_ID) {
                        Number = Current_Lex.Get_Lex_Value();
                        if (TID[Number].Get_Declare()){
                            cout << "Error! Second declaration is forbidden\n";
                            Output_Error_String();
                            throw Current_Lex;
                        }
                        TID[Number].Make_Declared();
                        TID[Number].Set_Type(LEX_LOGIC);
                        Get_Lexeme();
                        if (Current_Type == LEX_ASSIGN) {
                            Get_Lexeme();
                            TID[Number].Make_Assigned();
                            if (Current_Type == LEX_TRUE)
                                TID[Number].Set_Value(1); //вообще-то это колхоз. надо бы переделать
                            else if (Current_Type == LEX_FALSE)
                                TID[Number].Set_Value(0);
                            else {
                                Output_Error_String();
                                throw "Wrong boolean initialisation";
                            }
                        }
                        else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                            Output_Error_String();
                            throw "Wrong boolean initialisation";
                        } //в случае, если  при отсутствии значения идет что-то лишнее
                    }
                    else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                        Output_Error_String();
                        throw "Wrong boolean initialisation";
                    }
                }
                break;
            case LEX_STRING:
                while (Current_Type != LEX_SEMICOLON) {
                    Get_Lexeme();
                    if (Current_Type == LEX_ID) {
                        Number = Current_Lex.Get_Lex_Value(); //пока получаем просто номер идентификатора, а не строки
                        if (TID[Number].Get_Declare()){
                            cout << "Error! Second declaration is forbidden\n";
                            Output_Error_String();
                            throw Current_Lex;
                        }
                        TID[Number].Make_Declared();
                        TID[Number].Set_Type(LEX_TEXT);
                        Get_Lexeme();
                        if (Current_Type == LEX_ASSIGN) {
                            Get_Lexeme();
                            TID[Number].Make_Assigned();
                            if (Current_Type == LEX_TEXT)
                                TID[Number].Set_Value(Current_Lex.Get_Lex_Value()); //у строковой лексемы номер отвечает за позицию в другой табблице. И обработка такая же, как и у int
                            else {
                                Output_Error_String();
                                throw "Wrong string initialisation";
                            }
                        } else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                            Output_Error_String();
                            throw "Wrong string initialisation";
                        } //в случае, если  при отсутствии значения идет что-то лишнее
                    } else if ((Current_Type != LEX_COMMA) && (Current_Type != LEX_SEMICOLON)) {
                        Output_Error_String();
                        throw "Wrong string initialisation";
                    };
                }
                break;
            default:
                Identified = false;
        }
        if (Identified) Get_Lexeme();
    }
}

void Parser::Program() {
    if (Current_Type == LEX_BEGIN)
        Get_Lexeme();
    else {
        Output_Error_String();
        throw "Error! Program begins with something wrong";
    }
    if (Current_Type == LEX_OPEN_BRACKET)
        Get_Lexeme();
    else {
        Output_Error_String();
        throw "Opening bracket is lost";
    }
    Initialisation(); //инициализировали все нужные переменные
    S();
    B();
    //for (Lex l: Poliz)
    //    cout << l;
    //cout << endl;
    //Poliz.clear();
    if (Current_Type == LEX_CLOSE_BRACKET)
        Get_Lexeme();
    else {
        Output_Error_String();
        throw "Error! Final bracket is lost";
    }
}

void Parser::B() {
    while ( (Current_Type == LEX_SEMICOLON) && (Current_Type != LEX_FIN) ){
        Get_Lexeme();
        S();
    }
}

void Parser::S() {
    int p10, p11, p12, p13;
    if (Current_Type == LEX_IF) {
        int Zero_Address = Poliz.size();
        In_If = true;
        Get_Lexeme();
        E();  //проверка выражения и проверка на то, явлается ли его результат логическим
        eq_bool();
        p12 = Poliz.size();
        Poliz.push_back(Lex()); //этот переход все равно изменим далее. пусть пока будет LEX_NULL
        Poliz.push_back(Lex(POLIZ_FGO));
        if (Current_Type == LEX_OPEN_BRACKET) {
            while ((Current_Type != LEX_CLOSE_BRACKET) && (Current_Type != LEX_FIN)) {
                Get_Lexeme();
                S();
            }
            if (Current_Type != LEX_CLOSE_BRACKET) {
                Output_Error_String();
                throw "Error! Unclosed bracket";
            }
        }
        else
            S(); //иначе-одна простая строка
        Get_Lexeme(); //переход на else
        p13 = Poliz.size();
        Poliz.push_back(Lex()); //тоже изментися
        Poliz.push_back(Lex(POLIZ_GO));
        Poliz[p12] = Lex(POLIZ_LABEL, Poliz.size());
        if (Current_Type == LEX_ELSE){
            Get_Lexeme(); //переходим на то, что после else
            if (Current_Type == LEX_OPEN_BRACKET) {
                while ((Current_Type != LEX_CLOSE_BRACKET) && (Current_Type != LEX_FIN)) {
                    Get_Lexeme();
                    S();
                }
                if (Current_Type != LEX_CLOSE_BRACKET) {
                    Output_Error_String();
                    throw "Error! Unclosed bracket";
                }
            }
            else
                S();
            Poliz[p13] = Lex(POLIZ_LABEL, Poliz.size());
        }
        else{
            Poliz.pop_back(); //если переходя по else нет, значит, надо убрать лишние элементы: адрес и символ перехода из полиза
            Poliz.pop_back();
            Poliz[p12].Set_Lex_Value(p13);
        }
        In_If = false;
        S(); //переход на следующее действие
    } //end if
    else if (Current_Type == LEX_WHILE){ //обработка ровно такая же, как и у if, только без else части
        In_Cycle = true;
        int Zero_Address = Poliz.size(); //чтобы при наличии нескольких while и нескольких break менять только относящиеся к этому циклу
        p10 = Poliz.size();
        Get_Lexeme();
        E();  //проверка выражения и проверка на то, явлается ли его результат логическим
        eq_bool();
        p11 = Poliz.size();
        Poliz.push_back(Lex(POLIZ_LABEL, Zero_Address));
        Poliz.push_back(Lex(POLIZ_FGO));
        if (Current_Type == LEX_OPEN_BRACKET) {
            while ((Current_Type != LEX_CLOSE_BRACKET) && (Current_Type != LEX_FIN)) {
                Get_Lexeme();
                S();
            }
            if (Current_Type != LEX_CLOSE_BRACKET) {
                Output_Error_String();
                throw "Error! Unclosed bracket";
            }
            else
                Get_Lexeme(); //переход на следующую лексему
        }
        else
            S(); //иначе-одна простая строка
        Poliz.push_back(Lex(POLIZ_LABEL, p10));
        Poliz.push_back(Lex(POLIZ_GO));
        Poliz[p11] = Lex(POLIZ_LABEL, Poliz.size());
        for (int i = Zero_Address; i < Poliz.size(); i++)
            if (Poliz[i].Get_Lex_Type() == POLIZ_BREAK)
                Poliz[i] = Lex(POLIZ_LABEL, Poliz.size()); //переход при break делается на конец текущей части стека
        S(); //после выполнения while переходим на следующее действие
        In_Cycle = false;
    } // end while
    else if (Current_Type == LEX_READ) {
        Get_Lexeme();
        if (Current_Type == LEX_OPEN_BRACE) {
            Get_Lexeme();
            if (Current_Type == LEX_ID) {
                check_id_in_read();
                Poliz.push_back(Lex(POLIZ_ADDRESS, Current_Value));
                Get_Lexeme();
            } else throw "Error! Reading not an identifier";
            if (Current_Type == LEX_CLOSE_BRACE){
                Get_Lexeme();
                Poliz.push_back(Lex(LEX_READ));
            }
            else {
                Output_Error_String();
                throw "Error! No close brace while reading";
            }
        } else {
            Output_Error_String();
            throw "Reading error";
        }
        if (Current_Type != LEX_SEMICOLON){
            Output_Error_String();
            cout << "Error! Semicolon is lost\n";
        }
    } //end read
    else if (Current_Type == LEX_WRITE) {
        Get_Lexeme();
        if (Current_Type == LEX_OPEN_BRACE) {
            Get_Lexeme();
            E();
            while  (Current_Type == LEX_COMMA){ //так как переменных может быть несколько, через запятую
                Get_Lexeme();
                E();
                if (Current_Type == LEX_FIN) {
                    Output_Error_String();
                    throw "Error! Unclosed write";
                }
            }
            if (Current_Type == LEX_CLOSE_BRACE) {
                Poliz.push_back(Lex(LEX_WRITE));
                Get_Lexeme();
            }
            else {
                Output_Error_String();
                throw "Error! Unclosed write";
            }
        }
        else {
            Output_Error_String();
            throw "Error! Unclosed write";
        }
        if (Current_Type != LEX_SEMICOLON){
            Output_Error_String();
            cout << "Error! Semicolon is lost\n";
        }
    } //end write
    else if (Current_Type == LEX_ID) {
        Recursive_Assignment = true;
        Lex Buffer = Current_Lex;
        check_id();
        Poliz.push_back(Lex(POLIZ_ADDRESS, Current_Value));
        Get_Lexeme();
        if (Current_Type == LEX_SEMICOLON){
            Poliz.pop_back(); //меняем адрес переменной на значение, если это-конец
            Poliz.push_back(Buffer);
        }
        else{
            if (Current_Type != LEX_ASSIGN) { //если это не присваивание, то рассматриваемая лексема-значение, а не величина. a = b + 5, b в таком выражении
                Poliz.pop_back(); //меняем адрес переменной на значение, если это-конец
                Poliz.push_back(Buffer);
                while ( (Current_Type != LEX_SEMICOLON) && (Current_Type != LEX_FIN) ){
                    Lex_Stack.push(Current_Type);
                    Get_Lexeme();
                    Recursion++;
                    E();
                    check_op();
                    Recursion--;
                }
                if (Current_Type == LEX_FIN){
                    Output_Error_String();
                    throw "Error! Semicolon is lost\n";
                }
            }
            else{
                Get_Lexeme();
                if (Current_Type != LEX_ID){ //выражение обрабатываем точно так же, как и раньше
                    Recursion++;
                    E(); //типы проверим самостоятельно, здесь
                    if (Lex_Stack.top() != TID[Buffer.Get_Lex_Value()].Get_Type()){
                        Output_Error_String();
                        throw "Error! Unassignable types\n";
                    }
                    Poliz.push_back(Lex(LEX_ASSIGN));
                    Recursion--;
                }
                else{ //если все-таки идетнификатор
                    Recursion++;
                    S(); //если типы все-таки совпали и это идентификатор. когда-нибудь из этой рекурсии выйдем
                    if (Lex_Stack.top() != TID[Buffer.Get_Lex_Value()].Get_Type()){
                        Output_Error_String();
                        throw "Error! Unassignable types\n";
                    }
                    Poliz.push_back((Lex(LEX_ASSIGN)));
                    Recursion--;
                }
            }
        }
        if (Recursion == 0) Poliz.push_back(Lex(LEX_SEMICOLON));
    } //assign end
    else if (Current_Type == LEX_BREAK){
        if (!In_Cycle){
            Output_Error_String();
            throw "Error! break is not allowed here";
        }
        else{
            Get_Lexeme();
            if (Current_Type != LEX_SEMICOLON){
                Output_Error_String();
                throw "Error! Semicolon is lost in break";
            }
            Poliz.push_back(Lex(POLIZ_BREAK)); //потом заменим на нормальную
            Poliz.push_back(Lex(POLIZ_GO)); //сюда надо вставить адрес. но сперва-просчитать его
        }
    }
    else if (Current_Type == LEX_SEMICOLON);
    else
        B();
}

void Parser::E() {
    E1();
    if ((Current_Type == LEX_EQ) || (Current_Type == LEX_NEQ) || (Current_Type == LEX_G) || (Current_Type == LEX_L) ||
        (Current_Type == LEX_LEQ) || (Current_Type == LEX_GEQ) || (Current_Type == LEX_ASSIGN)) {
        if (Current_Type == LEX_ASSIGN){
            Lex Buffer = Poliz.back();
            Poliz.pop_back();
            Poliz.push_back(Lex(POLIZ_ADDRESS, Buffer.Get_Lex_Value()));
        }
        Lex_Stack.push(Current_Type); //запоминаем текцщий тип в стеке. Для адекватной проверки на сравнимость типов
        Get_Lexeme();
        E1();
        check_op();
    }
}

void Parser::E1() {
    T();
    while ((Current_Type == LEX_ADD) || (Current_Type == LEX_SUBTRACT) ||
           (Current_Type == LEX_OR)) {
        Lex_Stack.push(Current_Type);
        Get_Lexeme();
        T();
        check_op();
    }
}

void Parser::T() {
    F();
    while ((Current_Type == LEX_DIVIDE) || (Current_Type == LEX_MULTIPLY) ||
           (Current_Type == LEX_AND) || (Current_Type == LEX_MOD)) {
        Lex_Stack.push(Current_Type);
        Get_Lexeme();
        F();
        check_op();
    }
}

void Parser::F() {
    if (Current_Type == LEX_ID) {
        check_id();
        Poliz.push_back(Lex(LEX_ID, Current_Value));
        Get_Lexeme();
    }
    else if (Current_Type == LEX_NUM) {
        Lex_Stack.push(LEX_NUM);
        Poliz.push_back(Current_Lex);
        Get_Lexeme();
    }
    else if (Current_Type == LEX_TEXT) {
        Lex_Stack.push(LEX_TEXT);
        Poliz.push_back(Current_Lex);
        Get_Lexeme();
    }
    else if (Current_Type == LEX_TRUE) {
        Lex_Stack.push(LEX_LOGIC);
        Poliz.push_back(Lex(LEX_TRUE ,1));
        Get_Lexeme();
    }
    else if (Current_Type == LEX_FALSE) {
        Lex_Stack.push(LEX_LOGIC);
        Poliz.push_back(Lex(LEX_FALSE, 0));
        Get_Lexeme();
    }
    else if (Current_Type == LEX_LOGIC) {
        Lex_Stack.push(LEX_LOGIC);
        Poliz.push_back(Current_Lex);
        Get_Lexeme();
    }
    else if (Current_Type == LEX_NOT) {
        Get_Lexeme();
        F();
        check_not();
    }
    else if (Current_Type == LEX_OPEN_BRACE) {
        Get_Lexeme();
        E();
        if (Current_Type == LEX_CLOSE_BRACE)
            Get_Lexeme();
        else {
            Output_Error_String();
            throw "Error! Unclosed brace";
        }
    }
    else {
        Output_Error_String();
        throw "Error!";
    }
}

void Parser::check_id() {
    if (TID[Current_Value].Get_Declare()) //уже объявлено
        Lex_Stack.push(TID[Current_Value].Get_Type());
    else {
        Output_Error_String();
        throw "Error! Not declared";
    }
}

void Parser::check_op() {
    Lex_Type T1, T2, Operation, T, Res_Type;
    T2 = Lex_Stack.top(); //извлечение верхнего элемента стека в переменную T2
    Lex_Stack.pop();
    Operation = Lex_Stack.top();
    Lex_Stack.pop();
    T1 = Lex_Stack.top();
    Lex_Stack.pop();

    if ( (Operation == LEX_ASSIGN) || (Operation == LEX_ADD) )
        Res_Type = T = T1;
    if (Operation == LEX_SUBTRACT){
        if (T == LEX_TEXT) { //для строк вычитсние не определено. для bool тот же С++ позволяет, так что оставим
            Output_Error_String();
            throw "Error! Wrong types of parameters";
        }
        else
            Res_Type = T = T1;
    }
    if ( (Operation == LEX_MULTIPLY) || (Operation == LEX_DIVIDE) || (Operation == LEX_MOD) ||
         (Operation == LEX_SUBTRACT) ){
        Res_Type = T = LEX_NUM;
    }
    if ((Operation == LEX_OR) || (Operation == LEX_AND)) {
        T = Res_Type = LEX_LOGIC;
    }
    if ((Operation == LEX_G) || (Operation == LEX_L) || (Operation == LEX_GEQ) ||
        (Operation == LEX_LEQ) || (Operation == LEX_EQ) || (Operation == LEX_NEQ)) {
        Res_Type = LEX_LOGIC;
        T = T1;
    }
    if ((T1 == T2) && (T1 == T))
        Lex_Stack.push(Res_Type);
    else {
        Output_Error_String();
        throw "Error! Wrong types of parameters";
    }
    Poliz.push_back(Lex(Operation));
}

void Parser::check_not() {
    if (Lex_Stack.top() != LEX_LOGIC) {
        Output_Error_String();
        throw "Error! Not a logical expression in \"not\"";
    }
    Poliz.push_back(Lex(LEX_NOT));
}

void Parser::eq_type() {
    Lex_Type T;
    T = Lex_Stack.top();
    Lex_Stack.pop();
    if (T != Lex_Stack.top()) { //если типы на вершине стека не совпадают. то есть если в выражении разные типы
        throw "Error! Wrong types\n";
    }
    Lex_Stack.pop(); //больше эти типы не нужны
}

void Parser::eq_bool() {
    if (Lex_Stack.top() != LEX_LOGIC) {
        throw "Error! Not a boolean expression\n";
    }
    Lex_Stack.pop();
}

void Parser::check_id_in_read() {
    if (!TID[Current_Value].Get_Declare())
        throw "Error! Not declared\n";
}

int Put_String(string &Str) {
    if (TS.empty()) {
        TS.push_back(Str);
        return 0;
    }
    for (int i = 0; i < TS.size(); i++)
        if (TS[i] == Str)
            return i;
    TS.push_back(Str);
    return (TS.size() - 1); //все равно если строку нашли, выйдем из функции немного раньше
}

bool IsLetter(char c) {
    if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))) return true;
    else return false;
}

bool IsDigit(char c) {
    if ((c >= '0') && (c <= '9')) return true;
    else return false;
}

ostream &operator<<(ostream &out, const Lex &L) {
    if (L.Type == LEX_NULL) cout << "NULL ";
    if (L.Type == LEX_AND) cout << "AND ";
    if (L.Type == LEX_END) cout << "END ";
    if (L.Type == LEX_BEGIN) cout << "BEGIN ";
    if (L.Type == LEX_BOOL) cout << "BOOL" << L.Get_Lex_Value() << ' ';
    if (L.Type == LEX_READ) cout << "READ ";
    if (L.Type == LEX_WRITE) cout << "WRITE ";
    if (L.Type == LEX_TRUE) cout << "TRUE ";
    if (L.Type == LEX_FALSE) cout << "FALSE ";
    if (L.Type == LEX_FIN) cout << "FIN ";
    if (L.Type == LEX_DIVIDE) cout << "/ ";
    if (L.Type == LEX_MULTIPLY) cout << "* ";
    if (L.Type == LEX_ADD) cout << "+ ";
    if (L.Type == LEX_SUBTRACT) cout << "- ";
    if (L.Type == LEX_THEN) cout << "THEN ";
    if (L.Type == LEX_OR) cout << "OR ";
    if (L.Type == LEX_ELSE) cout << "ELSE ";
    if (L.Type == LEX_EQ) cout << "== ";
    if (L.Type == LEX_NEQ) cout << "!= ";
    if (L.Type == LEX_GEQ) cout << ">= ";
    if (L.Type == LEX_LEQ) cout << "<= ";
    if (L.Type == LEX_G) cout << "> ";
    if (L.Type == LEX_L) cout << "< ";
    if (L.Type == LEX_IF) cout << "IF ";
    if (L.Type == LEX_INT) cout << "INT ";
    if (L.Type == LEX_NOT) cout << "NOT ";
    if (L.Type == LEX_WHILE) cout << "WHILE ";
    if (L.Type == LEX_ASSIGN) cout << "= ";
    if (L.Type == LEX_SEMICOLON) cout << "; ";
    if (L.Type == LEX_OPEN_BRACKET) cout << "{ ";
    if (L.Type == LEX_CLOSE_BRACKET) cout << "} ";
    if (L.Type == LEX_OPEN_BRACE) cout << "( ";
    if (L.Type == LEX_CLOSE_BRACE) cout << ") ";
    if (L.Type == LEX_COMMA) cout << ", ";
    if (L.Type == LEX_STRING) cout << "STRING ";
    if (L.Type == LEX_MOD) cout << "% ";
    if (L.Type == LEX_BREAK) cout << "BREAK ";
    if (L.Type == LEX_DO) cout << "DO ";
    if (L.Type == LEX_ID) cout << TID[L.Get_Lex_Value()].Get_Name() << ' ';
    if (L.Type == LEX_TEXT) cout << '\"' << TS[L.Lex_Value] << "\" ";
    if (L.Type == LEX_NUM) cout << L.Get_Lex_Value() << ' ';
    if (L.Type == POLIZ_GO) cout << "! ";
    if (L.Type == POLIZ_FGO) cout << "!F ";
    if (L.Type == POLIZ_BREAK) cout << "PB ";
    if (L.Type == POLIZ_LABEL) cout << "PL" << L.Get_Lex_Value() << ' ';
    if (L.Type == POLIZ_ADDRESS) cout << "&" << TID[L.Get_Lex_Value()].Get_Name() << ' '; // POLIZ_ADDRESS - это как раз адрес переменной
    return out;
}

Lex Lex::Lex_TW(string N) {
    if (N == "and") return LEX_AND;
    if (N == "boolean") return LEX_BOOL;
    if (N == "else") return LEX_ELSE;
    if (N == "if") return LEX_IF;
    if (N == "false") return LEX_FALSE;
    if (N == "true") return LEX_TRUE;
    if (N == "int") return LEX_INT;
    if (N == "not") return LEX_NOT;
    if (N == "or") return LEX_OR;
    if (N == "program") return LEX_BEGIN;
    if (N == "read") return LEX_READ;
    if (N == "write") return LEX_WRITE;
    if (N == "while") return LEX_WHILE;
    if (N == "string") return LEX_STRING;
    if (N == "break") return LEX_BREAK;
    return LEX_NULL; //если ничего не нашли. чтобы не получать ошибку непонятно на чем
}

int Put(const string &buf) //возвращаем int, чтобы в дальнейшем записать на нужное место
{
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) !=
        TID.end()) //то есть если элемент в таблице был. buf преобразовывается к ident
        return (k - TID.begin());
    TID.push_back(Ident(buf));  //иначе пишем в таблицу и возвращаем записанный номер
    return (TID.size() - 1); //записали на последнее (теперь последнее) место таблицы
}

Lex Scanner::Get_Lex() {
    string buf; //изначально пустая и считываем только одну лексему. Значит, clear не нужно
    int Num = 0; //"буфер" для сохранения числа
    int TID_Position; //номер элемента в таблице TID- для идентификаторов
    int TW_Position;
    int TD_Position;
    int TS_Position;
    CurrentState = Beginning;
    do {
        gc();
        switch (CurrentState) {
            case Beginning:
                if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t')) {
                    if (c == '{') return (LEX_OPEN_BRACKET);
                    else if (c == '}') return (LEX_CLOSE_BRACKET);
                    else if (c == ';') return (LEX_SEMICOLON);
                    else if (c == '%') return (LEX_MOD);
                    else if (c == ',') return (LEX_COMMA);
                    else if (c == '(') return (LEX_OPEN_BRACE);
                    else if (c == ')') return (LEX_CLOSE_BRACE);
                    else if (IsDigit(c))  //численное значение
                    {
                        Num = c - '0';
                        CurrentState = Number;
                    } else if (IsLetter(c)) //имя переменной или служебное слово
                    {
                        buf += c;
                        CurrentState = Ident;
                    } else if (c == '/')
                        CurrentState = Comment_Division;
                    else if ((c == '=') || (c == '>') || (c == '<') || (c == '!')) {
                        buf += c;
                        CurrentState = Comparsion;
                    } else if (c == '\"') CurrentState = String;
                    else if (c == EOF) return Lex(LEX_FIN);
                    else if (c == '+')
                        return LEX_ADD; //на этом этапе делим только на лексемы, а уж что она значит, будем смотреть потом
                    else if (c == '-') return LEX_SUBTRACT;
                    else if (c == '*') {
                        gc();
                        if (c == '/')
                            throw Error(Error::UNOPENED_COMMENT);
                        else {
                            ungetc(c, Text);
                            return LEX_MULTIPLY;
                        }
                    } else throw Error(Error::WRONG_CHARACTER, c);   //else нужны только для этого случая
                }
                break;
            case Comparsion:
                if (buf[0] == '>')
                    if (c == '=')
                        return LEX_GEQ;
                    else {
                        ungetc(c, Text); //тогда этот символ-от другой лексемы
                        return LEX_G;
                    }
                if (buf[0] == '<')
                    if (c == '=')
                        return LEX_LEQ;
                    else {
                        ungetc(c, Text); //тогда этот символ-от другой лексемы
                        return LEX_L;
                    }
                if (buf[0] == '!') {
                    if (c == '=')
                        return LEX_NEQ;
                    else
                        throw Error(Error::WRONG_CHARACTER, c); //тут можно не возвращать, и так ошибка
                }
                if (buf[0] == '=') {
                    if (c == '=')
                        return LEX_EQ;
                    else {
                        ungetc(c, Text);
                        return LEX_ASSIGN;
                    }
                }
            case Number:
                if (IsDigit(c)) {
                    Num *= 10;
                    Num += c - '0'; //если цифра, увеличиваем результирующее число
                } else {
                    TD_Position = 0;
                    if ((c != ' ') && (c != '\n') && (c != '\t') && (c != '\r') &&
                        (!Belongs_to_TD(c))) //если не является частью разделителя. значит, это ошибочная лексема
                    {
                        int Digit;
                        while (Num != 0) {
                            Digit = Num % 10;
                            buf += Digit + '0';
                            Num /= 10;
                        }
                        buf += c;
                        //уже понятно, что лексема неверная. нет смысла ничего сохранять. теперь просто считаем ее до конца
                        gc();
                        while ((c != EOF) && (c != ' ') && (c != '\n') && (c != '\t') && (c != '\r') &&
                               (!Belongs_to_TD(c))) {
                            buf += c;
                            gc();
                        }
                        throw Error(Error::WRONG_IDENTIFIER, buf);
                    } else {
                        ungetc(c, Text); //значит, считывание числа закончено
                        return Lex(LEX_NUM, Num);
                    }
                }
                break;
            case Ident:
                //к этому моменту первый символ уже есть и он верный. так что проверка на цифру не нужна
                if ((!IsDigit(c)) && (!IsLetter(c))) {
                    ungetc(c,
                           Text); //возвращаем символ и проверяем, не была ли лексема специальным словом, сравнивая ее ТЕКСТ
                    TW_Position = 0;
                    while ((TW[TW_Position] != "END_OF_ARRAY") && (TW[TW_Position] != buf))
                        TW_Position++;
                    if (TW[TW_Position] != "END_OF_ARRAY")
                        return Lex::Lex_TW(TW[TW_Position]);
                    else
                        return Lex(LEX_ID, Put(buf)); //это была новая переменная, элемент-последний, номер- длина -1
                    //приведением типов подгоняем индекс под элемент перечислимого типа за счет номера
                    //и получаем нужные характеристики лексемы
                } else
                    buf += c;
                break;
            case Comment_Division:
                // / уже получено
                if (c == '*') {
                    CurrentState = Comment;
                } else
                    CurrentState = Division;
                break;
            case Comment: //в комментарии не надо заморачиваться с \", а вот в строках надо
                if (c == EOF)
                    throw Error(Error::UNCLOSED_COMMENT); //значит, дошли до конца текста в комментарии
                if (c == '*'){
                    gc();
                    if (c == '/'){
                        gc();
                        CurrentState = Beginning;
                    }
                }
                break;
            case Division:
                return LEX_DIVIDE;
            case String:
                if (c == '\"') {
                    if (buf[buf.length() - 1] != '\\') {
                        TS_Position = Put_String(buf);
                        return Lex(LEX_TEXT, TS_Position);
                    } //если последний символ не \, то это конец строки
                    else {
                        buf.erase(buf.length() - 1); //удаление \ из строки. этот символ-служебный
                        buf += '\"'; //иначе это просто символ строки
                    }
                } else if (c == EOF)
                    throw Error(Error::UNCLOSED_STRING);
                else
                    buf += c;
                break;
            default:
                throw Error(Error::UNKNOWN_ERROR);
        }
    } while (true); //пока не выйдем по какому-то из return
}

bool Scanner::Belongs_to_TD(char c) {  //проверка, является ли символ разделителем или его началом
    int i = 0;
    while ((TD[i][0] != c) && (TD[i] != "END_OF_ARRAY")) i++;
    if (TD[i] == "END_OF_ARRAY")
        return false;
    else
        return true;
}

class Executer{
public:
    void Execute(vector <Lex> &Poliz);
};

void Executer::Execute(vector <Lex> &Poliz) {
    for (Lex l: Poliz)
        cout << l;
    cout << endl;
    Lex pc_el;
    stack <int> Int_Args;
    int i,j, Index = 0, Size = Poliz.size();
    while (Index < Size){
        pc_el = Poliz[Index];
        switch(pc_el.Get_Lex_Type()){
            case LEX_TRUE:
            case LEX_FALSE:
            case LEX_NUM:
            case LEX_TEXT:
            case LEX_LOGIC:
            case POLIZ_ADDRESS:
            case POLIZ_LABEL:
                Int_Args.push(pc_el.Get_Lex_Value());
                break;
            case LEX_SEMICOLON:
                while (!Int_Args.empty())
                    Int_Args.pop();
                break;
            case LEX_ID:
                i = pc_el.Get_Lex_Value();
                if (TID[i].Get_Assign()){
                    Int_Args.push(TID[i].Get_Value());
                    break;
                }
                else
                    throw "POLIZ: indefinite identifier\n";
            case LEX_NOT:
                i = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(!i);
                break;
            case LEX_OR:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j || i);
                break;
            case LEX_AND:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j && i);
                break;
            case POLIZ_GO:
                i = Int_Args.top();
                Int_Args.pop();
                Index = i - 1;
                break;
            case POLIZ_FGO:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                if (!j) Index = i - 1;
                break;
            case LEX_WRITE:
                j = Int_Args.top();
                Int_Args.pop();
                cout << j << endl;
                break;
            case LEX_READ:
                int k;
                i = Int_Args.top();
                Int_Args.pop();
                if (TID[i].Get_Type() == LEX_NUM){ //LEX_NUM, LEX_INT - это исключительно слово t, а LEX_NUM- целочисленный параметр
                    cout << "Input value for " << TID[i].Get_Name() << endl;
                    cin >> k;
                }
                else if (TID[i].Get_Type() == LEX_LOGIC){
                    string j;
                    while (1){
                        cout << "Input value for " << TID[i].Get_Name() << endl;
                        cin >> j;
                        if ( (j != "true")  && (j != "false") ){
                            cout << "Error! This variable is boolean. Write \"true\" or \"false\"\n";
                            continue;;
                        }
                        k = (j == "true") ? 1:0;
                        break;
                    }
                }
                TID[i].Set_Value(k);
                TID[i].Make_Assigned();
                break;
            case LEX_ADD:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j + i);
                break;
            case LEX_MULTIPLY:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j * i);
                break;
            case LEX_DIVIDE:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                if (i == 0)
                    throw "Error! Division by zero\n";
                Int_Args.push(j / i);
                break;
            case LEX_MOD:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                if (i == 0)
                    throw "Error! Division by zero\n";
                Int_Args.push(j % i);
                break;
            case LEX_SUBTRACT:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j - i);
                break;
            case LEX_EQ:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j == i);
                break;
            case LEX_NEQ:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j != i);
                break;
            case LEX_GEQ:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j >= i);
                break;
            case LEX_LEQ:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j <= i);
                break;
            case LEX_L:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j < i);
                break;
            case LEX_G:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                Int_Args.pop();
                Int_Args.push(j > i);
                break;
            case LEX_ASSIGN:
                i = Int_Args.top();
                Int_Args.pop();
                j = Int_Args.top();
                TID[j].Set_Value(i);
                TID[j].Make_Assigned();
                break;
            default:
                throw "POLIZ: unexpected symbol\n";
        } //end of switch
        Index++;
    } //end of while
    cout << "\nExecution was successfully finished!\n";
}

class Interpretator{
    Parser Pars;
    Executer E;
public:
    Interpretator (const char *Program): Pars(Program){}
    void Interpretation();
};

void Interpretator::Interpretation() {
    Pars.Analyse();
    E.Execute(Pars.Poliz);
}

int main() {
    try {
        Interpretator I ("Test1.txt");
        I.Interpretation();
        return 0;
    }
    //try {
    //    Parser P1("Test1.txt");
    //    P1.Analyse();
    //}
    catch (Error E1) {
        if (E1.Type == Error::WRONG_CHARACTER) cout << "Lexical Error: Wrong character: " << E1.c << endl;
        if (E1.Type == Error::WRONG_IDENTIFIER) cout << "Lexical Error: Incorrect type of identifier: " << E1.Wrong_Lex << endl;
        if (E1.Type == Error::UNCLOSED_STRING) cout << "Lexical Error: Code contains unclosed strings" << endl;
        if (E1.Type == Error::UNKNOWN_ERROR) cout << "Unknown lexical error" << endl;
        return 0;
    }
    catch (Lex L1) {
        cout << L1;
        return 0;
    }
    catch (const char *str) {
        cout << str << endl;
        return 0;
    }
    cout << "\nSyntax and lexical analyses were completed successfully!\n";
    return 0;
}