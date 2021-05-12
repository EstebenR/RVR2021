#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

//./ej3 127.0.0.1 3000 t
int main(int argc, char **argv)
{
    char buffer[80];
    struct addrinfo hints;
    struct addrinfo *result;

    if (argc != 4 || argv[3][1] != '\0')
    {
        std::cout << "Usage ./tool <host IP> <port> <1 char command (t/d/q)>" << std::endl;
        return -1;
    }

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0)
    {
        std::cerr << "[getaddrinfo] Errored with " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int socketDesc = socket(result->ai_family, result->ai_socktype, 0);

    if (socketDesc == -1)
    {
        std::cerr << "[socket] Failed" << std::endl;
        return -1;
    }

    //try to send command to server
    int bytes = sendto(socketDesc, (void *)argv[3], 2, 0, result->ai_addr, result->ai_addrlen);

    if (bytes != 2)
    {
        std::cerr << "[sendto] Command sending failed" << std::endl;
    }

    if (argv[3][0] != 'q')
    {
        //Don't expect answer if quitting
        bytes = recvfrom(socketDesc, buffer, 79, 0, result->ai_addr, &result->ai_addrlen);

        if (bytes == -1)
        {
            std::cerr << "[recfrom] Failed retrieving info" << std::endl;
        }

        buffer[79] = '\0'; //Make sure buffer has end
        buffer[bytes] = '\0';

        std::cout << buffer << std::endl;
    }

    freeaddrinfo(result);
    close(socketDesc);

    return 0;
}