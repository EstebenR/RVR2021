#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include <iostream>

//TCP Echo server
// ./server <host ip> <port>
int main(int argc, char **argv)
{

	struct addrinfo hints;
	struct addrinfo *res;

	memset((void *)&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

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

	if (listen(socketDesc, 16) != 0)
	{
		std::cerr << "[listen]: failed" << std::endl;
		return -1;
	}

	struct sockaddr cliente;
	socklen_t clientelen = sizeof(struct sockaddr);

	int clientSocket = accept(socketDesc, &cliente, &clientelen);

	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];

	getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

	std::cout << "Incoming connection from " << host << " " << serv << std::endl;

	while (true)
	{
		char buffer[80];

		int bytes = recv(clientSocket, buffer, 79, 0);

		if (bytes <= 0)
		{
			std::cout << "Connection ended" << std::endl;
			break;
		}

		buffer[bytes] = '\0';

		send(clientSocket,buffer,bytes,0);
	}

	return 0;
}