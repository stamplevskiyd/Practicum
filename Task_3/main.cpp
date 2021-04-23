#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

//Beta-1.1
//переделано хранение строк, убрано хранение комментариев, добавлены спец. символы.
//работа над ошибками. Оказалось, что по варианту, скорее всего, не надо ничего добавлять. Максимум-while do.

enum Lex_Type {LEX_NULL, LEX_AND, LEX_BEGIN, LEX_ID, LEX_BOOL,
        LEX_READ, LEX_TRUE, LEX_FALSE, LEX_WRITE, LEX_COMMENT,
        LEX_STRING, LEX_FIN, LEX_NUM, LEX_DIVIDE, LEX_MULTIPLY,
        LEX_ADD, LEX_SUBTRACT, LEX_OR, LEX_THEN, LEX_ELSE,
        LEX_EQ, LEX_NEQ, LEX_LEQ, LEX_GEQ, LEX_L, LEX_G,
        LEX_ASSIGN, LEX_END, LEX_IF, LEX_INT, LEX_NOT, LEX_WHILE,
        LEX_SEMICOLON, LEX_OPEN_BRACKET, LEX_CLOSE_BRACKET,
        LEX_OPEN_BRACE, LEX_CLOSE_BRACE, LEX_COMMA, LEX_TEXT, LEX_MOD,
        LEX_BREAK, LEX_DO}; //greater or equal -GEQ
        //LEX_SEMICOLON- точка с запятой
        //Bracket- фигурные скобки, brace- обычные, comma-запятая
        //LEX_TEXT- именнт текст строки. string-тип

struct Error{
    enum Error_type {WRONG_IDENTIFIER, UNKNOWN_ERROR,
        UNCLOSED_BRACKET, UNCLOSED_BRACE, UNCLOSED_COMMENT, WRONG_CHARACTER,
        UNOPENED_COMMENT, UNOPENED_BRACKET, UNOPENED_BRACE, UNCLOSED_STRING};
    Error(Error_type T, string str): Type(T), Wrong_Lex(str) {};
    Error(Error_type T, char C1 = '0'): Type(T), c(C1) {};
    Error_type Type;
    string Wrong_Lex;
    char c;
};

class Ident{
    string Name;
    bool Declare;  //обЪявлен или нет
    Lex_Type Type;
    bool Assign; //присвоено значение или нет
    int Value;
public:
    Ident(){Declare = false; Assign = false; Type = LEX_NULL; Value = 0;}
    Ident(const string& N)
    {
        Type = LEX_NULL;
        Value = 0;
        Name = N;
        Assign = false;
        Declare = false;
    }
    bool operator== (const string &s) const  {return (Name == s);} //именно его и не хватало для работы put и find
    string Get_Name() const {return Name;}
    bool Get_Declare() const {return Declare;}
    bool Get_Assign() const {return Assign;}
    void Make_Declared() {Declare = true;}
    void Make_Assigned() {Assign = true;}
    int Get_Value() const {return Value;}
    void Set_Value (int V) {Value = V;}
};

class Lex{
    Lex_Type Type;
    int Lex_Value;
public:
    Lex(Lex_Type T = LEX_NULL, int V = 0){ Type = T; Lex_Value = V;}
    Lex_Type Get_Lex_Type() const {return Type;}
    int Get_Lex_Value() const {return Lex_Value;}
    friend ostream &operator<<(ostream &out, const Lex& L); //перегрузка вывода лексемы. Нужно хотя бы для проверки
    static Lex Lex_TW(string N); //перевод строки из таблицы в лексему. возвращает без значения и строки, вроде логично
};

class Scanner{
    char c;
    bool Finish;
    FILE *Text;
    void gc(){c = fgetc(Text);}
    enum State {Beginning, Ident, Number, Comment,
        String, Comparsion, Comment_Division,
        Division};
    State CurrentState;
    const char* Name = "Test_code.txt";
public:
    Scanner() //в конструкторе сразу открываем файл
    {
        CurrentState = Beginning;
        Finish = false; //если наткнулись на символ конца файла-собачку
        if ( (Text = fopen(Name, "r")) == NULL)
        {
            std::cout << "Unable to open file!\n";
            throw "File_Error";
        }
    }
    ~Scanner() { if (Text != NULL) fclose(Text); }
    //void Scan();
    Lex Get_Lex();
    static string TW[], TD[];    //TW - таблица служебных слов, TD- таблица ограничителей
    bool Belongs_to_TD(char c);
};

vector<Ident> TID; //таблица идентификаторов
vector<string> TS; //таблица строк. чтобы получить строку, обращаемся к ее индексу в таблице

string Scanner::TD[] = {"", ";", "@", ",", ":", "=", "(", ")", "<", ">", "+",
                        "-", "*", "/", "<=", ">=", "!=", "{", "}", "%", "END_OF_ARRAY"}; //END_OF_ARRAY- для более простой реализации поиска
string Scanner::TW[] = {"", "and", "boolean", "else", "if", "false", "int", "not", "or",
                        "program", "read", "true", "while", "write", "string", "break", "do", "END_OF_ARRAY"};

int Put_String(string &Str)
{
    if (TS.empty())
    {
        TS.push_back(Str);
        return 0;
    }
    for (int i = 0; i < TS.size(); i++)
        if (TS[i] == Str)
            return i;
    TS.push_back(Str);
    return (TS.size() - 1); //все равно если строку нашли, выйдем из функции немного раньше
}

bool IsLetter(char c)
{
    if ( ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) ) return true;
    else return false;
}

bool IsDigit(char c)
{
    if ( (c >= '0') && (c <= '9') ) return true;
    else return false;
}

ostream &operator<<(ostream &out, const Lex& L)
{
    if (L.Type == LEX_NULL) cout << "LEX_NULL";
    if (L.Type == LEX_AND) cout << "LEX_AND";
    if (L.Type == LEX_END) cout << "LEX_END";
    if (L.Type == LEX_BEGIN) cout << "LEX_BEGIN";
    if (L.Type == LEX_BOOL) cout << "LEX_BOOL";
    if (L.Type == LEX_ID) cout << "LEX_ID";
    if (L.Type == LEX_READ) cout << "LEX_READ";
    if (L.Type == LEX_WRITE) cout << "LEX_WRITE";
    if (L.Type == LEX_TRUE) cout << "LEX_TRUE";
    if (L.Type == LEX_FALSE) cout << "LEX_FALSE";
    if (L.Type == LEX_COMMENT) cout << "LEX_COMMENT";
    if (L.Type == LEX_FIN) cout << "LEX_FIN";
    if (L.Type == LEX_NUM) cout << "LEX_NUM";
    if (L.Type == LEX_DIVIDE) cout << "LEX_DIVIDE";
    if (L.Type == LEX_MULTIPLY) cout << "LEX_MULTIPLY";
    if (L.Type == LEX_ADD) cout << "LEX_ADD";
    if (L.Type == LEX_SUBTRACT) cout << "LEX_SUBTRACT";
    if (L.Type == LEX_THEN) cout << "LEX_THEN";
    if (L.Type == LEX_OR) cout << "LEX_OR";
    if (L.Type == LEX_ELSE) cout << "LEX_ELSE";
    if (L.Type == LEX_EQ) cout << "LEX_EQ";
    if (L.Type == LEX_NEQ) cout << "LEX_NEQ";
    if (L.Type == LEX_GEQ) cout << "LEX_GEQ";
    if (L.Type == LEX_LEQ) cout << "LEX_LEQ";
    if (L.Type == LEX_G) cout << "LEX_G";
    if (L.Type == LEX_L) cout << "LEX_L";
    if (L.Type == LEX_IF) cout << "LEX_IF";
    if (L.Type == LEX_INT) cout << "LEX_INT";
    if (L.Type == LEX_NOT) cout << "LEX_NOT";
    if (L.Type == LEX_WHILE) cout << "LEX_WHILE";
    if (L.Type == LEX_ASSIGN) cout << "LEX_ASSIGN";
    if (L.Type == LEX_SEMICOLON) cout << "LEX_SEMICOLON";
    if (L.Type == LEX_OPEN_BRACKET) cout << "LEX_OPEN_BRACKET";
    if (L.Type == LEX_CLOSE_BRACKET) cout << "LEX_CLOSE_BRACKET";
    if (L.Type == LEX_OPEN_BRACE) cout << "LEX_OPEN_BRACE";
    if (L.Type == LEX_CLOSE_BRACE) cout << "LEX_CLOSE_BRACE";
    if (L.Type == LEX_COMMA) cout << "LEX_COMMA";
    if (L.Type == LEX_STRING) cout << "LEX_STRING";
    if (L.Type == LEX_TEXT) cout << "LEX_TEXT";
    if (L.Type == LEX_MOD) cout << "LEX_MOD";
    if (L.Type == LEX_BREAK) cout << "LEX_BREAK";
    if (L.Type == LEX_DO) cout << "LEX_DO";
    if ( (L.Type == LEX_ID) || (L.Type == LEX_NUM) ) //чтобы вывод не замусоривать
        cout << ' ' << L.Lex_Value;
    if (L.Type == LEX_TEXT)
        cout << ' ' << TS[L.Lex_Value];
    cout << endl;
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
    return LEX_NULL; //если ничего не нашли. чтобы не получать ошибку непонятно на чем
}

int Put (const string &buf) //возвращаем int, чтобы в дальнейшем записать на нужное место
{
    vector<Ident>::iterator k;
    if ( (k = find(TID.begin(), TID.end(), buf)) != TID.end()) //то есть если элемент в таблице был. buf преобразовывается к ident
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
    do{
        gc();
        switch (CurrentState)
        {
            case Beginning:
                if ( (c != ' ') && (c != '\n') && (c != '\r') && (c != '\t') )
                {
                    if (c == '{') return (LEX_OPEN_BRACKET);
                    else if (c == '}') return (LEX_CLOSE_BRACKET);
                    else if (c == ';') return (LEX_SEMICOLON);
                    else if (c == '%') return (LEX_MOD);
                    else if (c == ',') return (LEX_COMMA);
                    else if (c == '(') return (LEX_OPEN_BRACE);
                    else if (c == ')') return (LEX_CLOSE_BRACE);
                    else if (IsDigit(c))  //численное значение
                    {
                        Num = c -'0';
                        CurrentState = Number;
                    }
                    else if (IsLetter(c)) //имя переменной или служебное слово
                    {
                        buf += c;
                        CurrentState = Ident;
                    }
                    else if (c == '/')
                        CurrentState = Comment_Division;
                    else if ( (c == '=') || (c == '>') || (c == '<') || (c == '!') )
                    {
                        buf += c;
                        CurrentState = Comparsion;
                    }
                    else if (c == '\"') CurrentState = String;
                    else if (c == EOF) return Lex(LEX_FIN);
                    else if (c == '+') return LEX_ADD; //на этом этапе делим только на лексемы, а уж что она значит, будем смотреть потом
                    else if (c == '-') return LEX_SUBTRACT;
                    else if (c == '*')
                    {
                        gc();
                        if (c == '/')
                            throw Error(Error::UNOPENED_COMMENT);
                        else
                        {
                            ungetc(c, Text);
                            return LEX_MULTIPLY;
                        }
                    }
                    else throw Error(Error::WRONG_CHARACTER, c);   //else нужны только для этого случая
                }
                break;
            case Comparsion:
                if (buf[0] == '>')
                    if (c == '=')
                        return LEX_GEQ;
                    else
                    {
                        ungetc(c, Text); //тогда этот символ-от другой лексемы
                        return LEX_G;
                    }
                if (buf[0] == '<')
                    if (c == '=')
                        return LEX_LEQ;
                    else
                    {
                        ungetc(c, Text); //тогда этот символ-от другой лексемы
                        return LEX_L;
                    }
                if (buf[0] == '!') {
                    if (c == '=')
                        return LEX_NEQ;
                    else
                        throw Error(Error::WRONG_CHARACTER, c); //тут можно не возвращать, и так ошибка
                }
                if (buf[0] == '=')
                {
                    if (c == '=')
                        return LEX_EQ;
                    else
                    {
                        ungetc(c, Text);
                        return LEX_ASSIGN;
                    }
                }
            case Number:
                if (IsDigit(c))
                {
                    Num *= 10;
                    Num += c - '0'; //если цифра, увеличиваем результирующее число
                }
                else
                {
                    TD_Position = 0;
                    if ( (c != ' ') && (c != '\n' ) && (c != '\t') && (c != '\r') && (!Belongs_to_TD(c)) ) //если не является частью разделителя. значит, это ошибочная лексема
                    {
                        int Digit;
                        while (Num != 0)
                        {
                            Digit = Num%10;
                            buf += Digit + '0';
                            Num /= 10;
                        }
                        buf += c;
                        //уже понятно, что лексема неверная. нет смысла ничего сохранять. теперь просто считаем ее до конца
                        gc();
                        while ( (c != EOF) && (c != ' ') && (c != '\n') && (c != '\t') && (c != '\r') && (!Belongs_to_TD(c)))
                        {
                            buf +=c;
                            gc();
                        }
                        throw Error(Error::WRONG_IDENTIFIER, buf);
                    }
                    else
                    {
                        ungetc(c, Text); //значит, считывание числа закончено
                        return Lex(LEX_NUM, Num);
                    }
                }
                break;
            case Ident:
                //к этому моменту первый символ уже есть и он верный. так что проверка на цифру не нужна
                if ( (!IsDigit(c)) && (!IsLetter(c)) )
                {
                    ungetc(c, Text); //возвращаем символ и проверяем, не была ли лексема специальным словом, сравнивая ее ТЕКСТ
                    TW_Position = 0;
                    while ( (TW[TW_Position] != "END_OF_ARRAY") && (TW[TW_Position] != buf) )
                        TW_Position++;
                    if (TW[TW_Position] != "END_OF_ARRAY")
                        return Lex::Lex_TW(TW[TW_Position]);
                    else
                    return Lex (LEX_ID, Put(buf)); //это была новая переменная, элемент-последний, номер- длина -1
                    //приведением типов подгоняем индекс под элемент перечислимого типа за счет номера
                    //и получаем нужные характеристики лексемы
                }
                else
                    buf += c;
                break;
            case Comment_Division:
                // / уже получено
                if (c == '*') {
                    CurrentState = Comment;
                }
                else
                    CurrentState = Division;
                break;
            case Comment: //в комментарии не надо заморачиваться с \", а вот в строках надо
                if (c == EOF)
                    throw Error(Error::UNCLOSED_COMMENT); //значит, дошли до конца текста в комментарии
                if (c == '/')
                    return Lex(LEX_COMMENT, 0); //никакого смысла в значении у комментария нет
                break;
            case Division: return LEX_DIVIDE;
            case String:
                if (c == '\"')
                {
                    if (buf[buf.length() - 1] != '\\'){
                        TS_Position = Put_String(buf);
                        return Lex(LEX_TEXT, TS_Position);
                    } //если последний символ не \, то это конец строки
                    else
                    {
                        buf.erase(buf.length() - 1); //удаление \ из строки. этот символ-служебный
                        buf += '\"'; //иначе это просто символ строки
                    }
                }
                else
                    if (c == EOF)
                        throw Error(Error::UNCLOSED_STRING);
                    else
                        buf +=c;
                break;
            default:
                throw Error(Error::UNKNOWN_ERROR);
        }
    }while (true); //пока не выйдем по какому-то из return
}

bool Scanner::Belongs_to_TD(char c) {  //проверка, является ли символ разделителем или его началом
    int i = 0;
    while ( (TD[i][0] != c) && (TD[i] != "END_OF_ARRAY") ) i ++;
    if (TD[i] == "END_OF_ARRAY")
        return false;
    else
        return true;
}

int main() {
    ofstream Additional ("Additional file.txt"); //перенаправим вывод в дополнительный файл, чтобы, в случае ошибки, выдавать только ошибку
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(Additional.rdbuf());
    try {
        Scanner S1;
        Lex L1(LEX_AND, 0);
        while ((L1 = S1.Get_Lex()).Get_Lex_Type() != LEX_FIN)
            cout << L1;
        if (L1.Get_Lex_Type() == LEX_FIN)
            cout << L1;
    }
    catch (Error E1){
        cout.rdbuf(coutbuf); //перенаправляем вывод обратно
        if (E1.Type == Error::WRONG_CHARACTER) cout << "Error: Wrong character: " << E1.c << endl;
        if (E1.Type == Error::WRONG_IDENTIFIER) cout << "Error: Incorrect type of identifier: " << E1.Wrong_Lex << endl;
        if (E1.Type == Error::UNCLOSED_COMMENT) cout << "Error: Code contains unclosed comments" << endl;
        if (E1.Type == Error::UNCLOSED_BRACE) cout << "Error: Code contains unclosed braces" << endl;
        if (E1.Type == Error::UNCLOSED_BRACKET) cout << "Error: Code contains unopened brackets" << endl;
        if (E1.Type == Error::UNOPENED_COMMENT) cout << "Error: Code contains unopened comments" << endl;
        if (E1.Type == Error::UNOPENED_BRACE) cout << "Error: Code contains unopened braces" << endl;
        if (E1.Type == Error::UNOPENED_BRACKET) cout << "Error: Code contains unopened brackets" << endl;
        if (E1.Type == Error::UNCLOSED_STRING) cout << "Error: Code contains unclosed strings" << endl;
        if (E1.Type == Error::UNKNOWN_ERROR) cout << "Unknown error" << endl;
        return 0;
    }
    cout.rdbuf(coutbuf); //перенаправляем вывод обратно
    cout << "Lexical analyses was completed successfully!\n\n";
    ifstream Output("Additional file.txt");
    string buf; //строка-буфер для вывода
    while(buf != "LEX_FIN") //это работает только при корректной программе, а у корректной программы всегда есть LEX_FIN
    {
        getline(Output, buf, '\n');
        cout << buf << endl;
    }
    Additional.close();
    ofstream Clearing("Additional file.txt", ios::out); //очистка файла-буфера
    return 0;
}
