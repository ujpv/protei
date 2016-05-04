#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>

#include "sockets/sockets.h"

int main()
{
    uint16_t const port = 6061;
    char const localhost_ip[] = "127.0.0.1"; 
    int const BUF_SIZE = 66561; //65kb + 1b
    
    socket_TCP_listener listener(10);
    if(!listener.is_valid())
    {
        std::cerr << "Creating TCP socket error.\nReason: " << listener.get_er_message() << std::endl;
        return 1;
    }

    socket_UDP sock_UDP;
    if (!sock_UDP.is_valid()) {
        std::cerr << "Creating UDP socket error.\nReason: " << sock_UDP.get_er_message() << std::endl;
        return 1;
    }

    if (!listener.bind(localhost_ip , port)) {
        std::cerr << "Bind TCP socket error.\nReason: " << listener.get_er_message() << std::endl;
        return 1;
    }

    if (!sock_UDP.bind(localhost_ip, port)) {
        std::cerr << "Bind UDP socket error.\nReason: " << sock_UDP.get_er_message() << std::endl;
        return 1;
    }

    char buf[BUF_SIZE];
    int bytes_read = 0;

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
                std::cerr << "Accept error\nReason: " << listener.get_er_message() << std::endl;
                continue;
            }

            for(;;)
            {
                bytes_read = sock_TCP.receive(buf, BUF_SIZE);

                if (bytes_read < 0) {
                    std::cerr << "Receive TCP pack error.\nReason: " << listener.get_er_message() << std::endl;
                    continue;
                } else if(bytes_read == 0) { 
                    break;
                }

                sock_TCP.send(buf, bytes_read);
            }
            continue;
        }

        if (FD_ISSET(sock_UDP.get_fd(), &sock_set)) {
            bytes_read = sock_UDP.receive(buf, 1024);
            if (bytes_read < 0) {
                std::cerr << "Receive UDP pack error.\nReason: " << listener.get_er_message() << std::endl;
                continue;
            } else if (bytes_read) {
                int n = sock_UDP.send(buf, bytes_read);
                if (n < 0) {
                    std::cerr << "Send UDP pack error.\nReason: " << listener.get_er_message() << std::endl;
                    continue;
                }

                if (!sock_UDP.disconect()) {
                    std::cerr << "Disconect UDP socket error.\nReason: " << listener.get_er_message() << std::endl;
                }
            }
        }
    }
    return 0;
}
