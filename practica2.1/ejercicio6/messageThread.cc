#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <thread>

//UDP message thread
class MessageThread
{
  public:
    MessageThread(int sd) : socketDesc(sd) {}

    void do_message()
    {
        bool exit = false;

        while (!exit)
        {
            int sent = 0;

            struct sockaddr cliente;
            socklen_t clientelen = sizeof(struct sockaddr);

            int bytes = recvfrom(socketDesc, (void *)buffer, 79, 0, &cliente, &clientelen);

            buffer[79] = '\0';

            if (bytes == -1)
            {
                std::cerr << "[recvfrom]: failed" << std::endl;
            }
            else
            {
                getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

                std::cout << bytes << " bytes received from " << host << ":" << serv
                          << " being processed by " << std::this_thread::get_id() << std::endl;
                sleep(3);

                if (bytes == 2) //We make sure we only receive one character (and \0)
                {
                    time_t rawTime;
                    struct tm *timeInfo;
                    int msgLen;

                    time(&rawTime);
                    timeInfo = localtime(&rawTime);

                    switch (buffer[0])
                    {
                    case 't':
                        msgLen = strftime(buffer, 79, "%r", timeInfo);
                        sent = sendto(socketDesc, buffer, msgLen, 0, &cliente, clientelen);
                        break;
                    case 'd':
                        msgLen = strftime(buffer, 79, "%F", timeInfo);
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
            }

            if (sent == -1)
            {
                std::cerr << "[sendto] Error sending information to client" << std::endl;
            }
        }
    }

  private:
    int socketDesc;
    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
};