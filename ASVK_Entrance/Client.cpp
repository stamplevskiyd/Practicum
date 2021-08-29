#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>

//клиент. работает с несколькими входными файлами

//#-символ конца файла (ввода) у клиента
//% - символ конца данных, пересылаемых от сервера
#define ERROR_S "Client error: "
#define DEFAULT_PORT 1306
#define BUFFER_SIZE 1024
#define SERVER_CLOSE_SYMBOL '#'

bool End (const char* msg){
    for (int i = 0; i < BUFFER_SIZE; i++)
        if (msg[i] == '%')
            return true;
    return false;
}

bool Data_Division(const char *buf){
    for (int i = 0; i < BUFFER_SIZE; i++)
        if (buf[i] == '*')
            return true;
    return false;
}


int main(int argc, char** argv) {
    if (argc < 3){
        std::cout << "Error! Fewer parameters than needed\n";
        return 0;
    }
    char *SERVER_IP = argv[1];
    int client;
    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0); //создание сокета
    if (client < 0){ //то есть если функция вернула -1 -то есть ошибка
        std::cout << ERROR_S << "Establishing socket error\n";
        exit(0);
    }

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    std::cout << " ==> Client socket is created\n";

    int ret;
    ret = connect(client, reinterpret_cast<struct sockaddr*>(&server_address),sizeof(server_address));
    if (ret == 0){
        std::cout << "Connecting to server\n" << inet_ntoa(server_address.sin_addr) << " with port number: " << DEFAULT_PORT << std::endl;
    }

    std::cout << "Starting processing\n";
    char Symbol;
    int i;
    int Read_Count;
    FILE *Text;
    for (i = 2; i < argc; i++)
    {
        Text = fopen(argv[i], "rt"); //открываем как текстовый файл
        if (Text == NULL){
            std::cout << "Unable to open this file. It won't be processed\n";
            break;
        }
        while (!feof(Text)) {
            Read_Count = fread(&Symbol, sizeof(char), 1, Text); //пока не кончится файл, считываем из него и пересылаем
            send(client, &Symbol, Read_Count, 0);
        }
        fclose(Text); //он больше не нужен
        char End_Symbol = '!';
        send (client, &End_Symbol, 1, 0); //посылаем обозначение конца ввода этого файла
        while (Symbol != '*')
            recv(client, &Symbol, 1, MSG_PEEK); //если вдруг получим ненужные, просто игнорируем их, пока не получим данные от сервера, после спец символа
        while (Symbol != '%') {
            recv(client, &Symbol, 1, 0);
            if ( (Symbol != '%') && (Symbol != '*') )  //так как мы сперва проверяем, не было ли конца, и потом сразу читаем
                std::cout << Symbol;
            if (Symbol == ']')
                std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }
    char End_Symbol = '#';
    send (client, &End_Symbol, 1, 0); //посылаем обозначение конца всего ввода этого клиента
    close(client);
    std::cout << "Goodbye!\n";
    return 0;
}
