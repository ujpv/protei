#include <iostream>
#include <algorithm>
#include <cstring>
#include <memory>

#include "sockets/sockets.h"

int main(int argc, char **argv)
{
    char const localhos_ip[] = "127.0.0.1";
    uint16_t const port = 6061;
    int const BUF_SIZE = 66561; //65kb + 1b
    char const syntax_mess[] = "syntax:\nclient PROTOCOL [SERVER IP]\nPROTOCOL - may be UDP or TCP\n";

    if (argc < 2) {
        std::cout << syntax_mess;
        return 0;
    }


    char const *ip = localhos_ip;
    if (argc > 2) {
        ip = argv[2];
    }

    abstract_socket *tmp = nullptr;

    if (!std::strcmp(argv[1], "UDP")) {
        tmp = new socket_UDP();           
    } else if (!std::strcmp(argv[1], "TCP")) {
        tmp = new socket_TCP();
    } else {
        std::cout << syntax_mess;
        return 0;
    }

    std::unique_ptr<abstract_socket> sock(tmp);

    if (!sock->is_valid()) {
        std::cerr << "Creating socket error.\nReason: " << sock->get_er_message() << std::endl;
        return 1;
    }

    if (!sock->connect(ip, port)) {
        std::cerr << "Connection failed.\nReason: " << sock->get_er_message() << std::endl;
        return 1;
    }

    for (;;) {
        std::string s;
        std::cin >> s;
        int byte_count = sock->send(s.c_str(), s.size());
        if (byte_count < 0) {
            std::cerr << "Sending failed.\nReason: " << sock->get_er_message() << std::endl;
        }
        std::cout << "Sent " << byte_count << " bytes." << std::endl;

        char buf[BUF_SIZE];
        byte_count = sock->receive(buf, BUF_SIZE);

        if (!byte_count) {
            std::cout << "Connection is lost." << std::endl;
            return 0;
        }

        if (byte_count < 0) {
            std::cerr << "Receiving failed.\nReason: " << sock->get_er_message() << std::endl;
        }

        buf[std::min(byte_count, BUF_SIZE - 1)] = '\0';
        std::cout << "Received " << byte_count << " bytes.\nMessage: " << buf << std::endl;
    }
    return 0;
}

