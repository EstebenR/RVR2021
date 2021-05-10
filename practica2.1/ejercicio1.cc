#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

int main(int argc, char **argv)
{
	struct addrinfo hints;
	struct addrinfo *res;

	memset((void *)&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC; //Any ip version
	hints.ai_socktype = 0; //Any socktype
	hints.ai_flags = AF_UNSPEC;

	int result = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (result != 0)
	{
		//Failure
		std::cerr << "[getaddrinfo]: " << gai_strerror(result) << std::endl;
		return -1;
	}

	//Print info for each address
	for (struct addrinfo *i = res; i != NULL; i = i->ai_next)
	{
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];
		getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		std::cout << host << " " << res->ai_family << " " << res->ai_socktype << std::endl;
	}

	freeaddrinfo(res);

	return 0;
}