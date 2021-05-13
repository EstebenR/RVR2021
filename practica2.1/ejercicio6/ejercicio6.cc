#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include <iostream>
#include <thread>
#include "messageThread.cc"

#define MAX_THREADS 5

//UDP multithread server
// ./ej6 <ip server> <puerto>

int main(int argc, char **argv)
{
    bool exit = false;
    struct addrinfo hints;
    struct addrinfo *res;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int socketDesc = socket(res->ai_family, res->ai_socktype, 0);

    if (socketDesc == -1)
    {
        std::cerr << "[socket]: failed" << std::endl;
        return -1;
    }

    rc = bind(socketDesc, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    if (rc != 0)
    {
        std::cerr << "[bind]: failed" << std::endl;
        return -1;
    }

    std::thread pool[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        MessageThread *mt = new MessageThread(socketDesc);

        std::thread([&mt](){
            mt->do_message();

            delete mt;
        }).detach();
    }

    char command = ' ';
    while (!exit)
    {
        //Check constantly to close server on main thread
        std::cin >> command;
        if (command == 'q')
        {
            exit = true;
        }
    }

    close(socketDesc);

    std::cout << "Closing..." << std::endl;

    return 0;
}