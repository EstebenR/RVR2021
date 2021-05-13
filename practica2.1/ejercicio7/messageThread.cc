#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <thread>

//TCP message thread
class MessageThread
{
  public:
    MessageThread(int sd) : socketDesc(sd) {}

    void do_message()
    {
        while (true)
        {
            int bytes = recv(socketDesc, buffer, 79, 0);

            if (bytes <= 0)
            {
                std::cout << "Connection ended" << std::endl;
                break;
            }

            buffer[bytes] = '\0';

            send(socketDesc, buffer, bytes, 0);
        }
        close(socketDesc);
    }

  private:
    int socketDesc;
    char buffer[80];
};