#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sockets.h"

#include <iostream>

uint16_t const port = 6061;
char const localhost_ip[] = "127.0.0.1"; 

int main()
{
    socket_TCP_listener listener(9);
    if(!listener.is_valid())
    {
        std::cerr << "Error TCP socket\n" << listener.get_er_message();
        return 1;
    }

    socket_UDP sock_UDP;
    if (!sock_UDP.is_valid()) {
        std::cerr << "Error UDP socket1\n" << sock_UDP.get_er_message();
        return 1;
    }

    if (!listener.bind(localhost_ip , port)) {
        std::cerr << "Error TCP bind\n" << listener.get_er_message();
        return 1;
    }

    if (!sock_UDP.bind(localhost_ip, port)) {
        std::cerr << "Error UDP bind\n" << listener.get_er_message();
        return 1;
    }

    char buf[1025];
    int bytes_read;
    for(;;)
    {
        fd_set sock_set;
        FD_ZERO(&sock_set);
        FD_SET(listener.get_fd(), &sock_set);
        FD_SET(sock_UDP.get_fd(), &sock_set);

        select(std::max(listener.get_fd(), sock_UDP.get_fd()) + 1, &sock_set, NULL, NULL, NULL);
        
        if(FD_ISSET(listener.get_fd(), &sock_set)) {

            socket_TCP sock_TCP = listener.accept();

            if(!sock_TCP.is_valid())
            {
                std::cerr << "Error accept\n" << listener.get_er_message();
                continue;
            }


            for(;;)
            {
                bytes_read = sock_TCP.receive(buf, 1024);

                if (bytes_read < 0) {
                    std::cerr << "Error receive TCP pack.\n" << listener.get_er_message();
                    continue;
                }

                if(bytes_read == 0) break;
                sock_TCP.send(buf, bytes_read);
            }
            continue;
        }

        if (FD_ISSET(sock_UDP.get_fd(), &sock_set)) {
            bytes_read = sock_UDP.receive(buf, 1024);
            if (bytes_read < 0) {
                std::cerr << "Error receive UDP pack.\n" << listener.get_er_message();
                continue;
            }
            if (bytes_read) {
                int n = sock_UDP.send(buf, bytes_read);
                if (n < 0) {
                    std::cerr << "Error send UDP pack.\n" << listener.get_er_message();
                    continue;
                }
                if (!sock_UDP.disconect()) {
                    std::cerr << "Error disconect UDP pack.\n" << listener.get_er_message();
                }

            }
        }
    }

    return 0;
}
