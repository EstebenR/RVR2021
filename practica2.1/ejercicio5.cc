#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <iostream>

//TCP Echo client
// ./client 127.0.0.1 3000
int main(int argc, char **argv)
{
    bool exit = false;
    char buffer[80];
    struct addrinfo hints;
    struct addrinfo *res;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] Errored with " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int socketDesc = socket(res->ai_family, res->ai_socktype, 0);

    if (socketDesc == -1)
    {
        std::cerr << "[socket] Failed" << std::endl;
        return -1;
    }

    if (connect(socketDesc, (struct sockaddr *)res->ai_addr, res->ai_addrlen) != 0)
    {
        std::cerr << "[connect] Error connecting to server" << std::endl;
        return -1;
    }

    while (!exit)
    {
        std::cin >> buffer;
        if (buffer[0] == 'Q' && buffer[1] == '\0')
        {
            //user wants to exit
            exit = true;
        }
        else
        {
            //send info to server
            if(send(socketDesc,buffer,strlen(buffer),0) == -1){
                std::cerr << "[send] Error sending message" << std::endl;
            }

            //Receive and display answer
            int bytes = recv(socketDesc,buffer,79,0);
            buffer[bytes] = '\0';
            std::cout << buffer << std::endl;
        }
    }

    freeaddrinfo(res);
    close(socketDesc);

    return 0;
}