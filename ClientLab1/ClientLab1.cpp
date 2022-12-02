// Пример простого TCP-клиента
#include <stdio.h>
#include <string.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
using namespace std;

#define PORT 8050
char SERVERADDR[11] = "127.0.0.1";

typedef double(*pointFunc)(double);
double f(double x) {
    return (pow(x, 3) * cos(x));
}
double rectangle_integral(pointFunc f, double a, double b, int n) {
    double x, h;
    double sum = 0.0;
    double fx;
    h = (b - a) / n;  //шаг

    for (int i = 0; i < n; i++) {
        x = a + i * h;
        fx = f(x);
        sum += fx;
    }
    return (sum * h); //приближенное значение интеграла равно 
    //сумме площадей прямоугольников
}

int main(int argc, char* argv[])
{
    char buff[1024];
    printf("TCP CLIENT\n");



    // Шаг 1 - инициализация библиотеки Winsock
    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        printf("WSAStart error %d\n", WSAGetLastError());
        return -1;
    }

    // Шаг 2 - создание сокета
    SOCKET my_sock;
    my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0)
    {
        printf("Socket() error %d\n", WSAGetLastError());
        return -1;
    }

    // Шаг 3 - установка соединения
    // заполнение структуры sockaddr_in - указание адреса и порта сервера
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    HOSTENT* hst;

    // преобразование IP адреса из символьного в сетевой формат
    if (inet_addr(SERVERADDR) != INADDR_NONE)
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else
    {
        // попытка получить IP адрес по доменному имени сервера
        if (hst = gethostbyname(SERVERADDR))
            // hst->h_addr_list содержит не массив адресов,
            // а массив указателей на адреса
            ((unsigned long*)&dest_addr.sin_addr)[0] =
            ((unsigned long**)hst->h_addr_list)[0][0];
        else
        {
            printf("Invalid address %s\n", SERVERADDR);
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    }

    // адрес сервера получен - пытаемся установить соединение
    if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
    {
        printf("Connect error %d\n", WSAGetLastError());
        return -1;
    }

    printf("Connection to %s successfully\n \
            Type quit for quit\n\n", SERVERADDR);

    // Шаг 4 - чтение и передача сообщений
    int arraysize = 0;
    int nsize;
    double A;
    double B;
    double eps;
    double s1, s;
    int n = 1;
    nsize = recv(my_sock, (char*)&A, sizeof(double), NULL);
    nsize = recv(my_sock, (char*)&B, sizeof(double), NULL);
    nsize = recv(my_sock, (char*)&eps, sizeof(double), NULL);
    nsize = nsize / sizeof(double);
    // ставим завершающий ноль в конце строки
    buff[nsize] = 0;
    int tmp;
    s1 = rectangle_integral(f, A, B, n); //первое приближение для интеграла
    do {
        s = s1;     //второе приближение
        n = 2 * n;  //увеличение числа шагов в два раза, 
        //т.е. уменьшение значения шага в два раза
        s1 = rectangle_integral(f, A, B, n);
    } while (fabs(s1 - s) > eps);
    cout << "\nIntegral = " << s1 << endl;
    // передаем строку клиента серверу
    send(my_sock, (char*)&s1, sizeof(double), 0);

    closesocket(my_sock);
    WSACleanup();
    int i;
    cin >> i;
    return 0;
}
