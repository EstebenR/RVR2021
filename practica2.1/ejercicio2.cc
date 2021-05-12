#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#include <iostream>

// ./ej2 <dir escucha> <puerto>
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

    while (!exit)
    {
        char buffer[80];

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        int sent = 0;

        struct sockaddr cliente;
        socklen_t clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(socketDesc, (void *)buffer, 80, 0, &cliente, &clientelen);

        if (bytes == -1)
        {
            std::cerr << "[recvfrom]: failed" << std::endl;
            return -1;
        }
        else if (bytes == 2) //We make sure we only receive one character (and \0)
        {
            getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << bytes << " bytes received from " << host << ":" << serv << std::endl;

            time_t rawTime;
            struct tm *timeInfo;
            int msgLen;

            time(&rawTime);
            timeInfo = localtime(&rawTime);

            switch (buffer[0])
            {
            case 't':
                msgLen = strftime(buffer, 80, "%r", timeInfo);
                sent = sendto(socketDesc, buffer, msgLen, 0, &cliente, clientelen);
                break;
            case 'd':
                msgLen = strftime(buffer, 80, "%F", timeInfo);
                sent = sendto(socketDesc, buffer, msgLen, 0, &cliente, clientelen);
                break;
            case 'q':
                exit = true;
                break;
            default:
                std::cout << "Unsupported command: " << buffer[0] << std::endl;
                break;
            }
        }

        if (sent == -1)
        {
            std::cerr << "[sendto] Error sending information to client" << std::endl;
        }
    }

    close(socketDesc);

    std::cout << "Closing..." << std::endl;

    return 0;
}