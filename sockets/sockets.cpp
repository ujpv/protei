#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring>
#include <errno.h>

#include <algorithm>

#include "sockets.h"

//============================abstact_socket==================================

abstract_socket::abstract_socket(int fd)
    : m_fd(fd) 
    , m_pcount(new int(1)) {}

abstract_socket::abstract_socket(abstract_socket const &other)
    : m_fd (other.m_fd)
    , m_pcount(other.m_pcount)
    {
        ++(*m_pcount);
    }

int abstract_socket::get_fd() const
{
    return m_fd;
}

bool abstract_socket::bind(const char * const ip, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip) {
        inet_aton(ip,  &(addr.sin_addr));
    } else {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    return ::bind(m_fd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
}

int abstract_socket::send(const char *buf, int size)
{
    int sent_total = 0;
    while (size > sent_total) {
        int sent = ::send(m_fd, buf + sent_total, size - sent_total, 0);
        if (sent == -1) {
            return -1;
        }
        sent_total += sent;    
    }
    return sent_total;
}

bool abstract_socket::connect(const char *ip, uint16_t port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip,  &(addr.sin_addr));
    return ::connect(m_fd, (struct sockaddr *) &addr, sizeof(addr)) >= 0;
}

bool abstract_socket::disconect()
{
    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_UNSPEC;
    return ::connect(m_fd, (struct sockaddr *) &addr, sizeof(addr)) >= 0;
}

bool abstract_socket::is_valid() const {
    return m_fd > 0;
}

char *abstract_socket::get_er_message() const {
     return std::strerror(errno);
}

void abstract_socket::swap(abstract_socket &other) {
    std::swap(m_fd, other.m_fd);
    std::swap(m_pcount, m_pcount);
}

abstract_socket::~abstract_socket()
{
    if (!--(*m_pcount)) {
        delete m_pcount;
        close(get_fd());
    }
}

//====================================socket_UDP========================================

socket_UDP::socket_UDP()
    :abstract_socket(socket(AF_INET, SOCK_DGRAM, 0)) {}

socket_UDP::socket_UDP(socket_UDP const &other)
    :abstract_socket(other) {}

socket_UDP & socket_UDP::operator=(socket_UDP const &other) {
    socket_UDP(other).swap(*this);
    return *this;
}

int socket_UDP::receive(char *buf, int size)
{
    sockaddr client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int bytes_read = recvfrom(get_fd(), buf, size, 0, &client_addr, &client_addr_len);
    ::connect(get_fd(), &client_addr, client_addr_len);
    return bytes_read;
}

socket_UDP::~socket_UDP() {}

//===============================socket_TCP====================================

socket_TCP::socket_TCP()
    : abstract_socket(socket(AF_INET, SOCK_STREAM, 0)) {}

socket_TCP::socket_TCP(socket_TCP const &other)
    : abstract_socket(other) {}

socket_TCP & socket_TCP::operator=(socket_TCP const &other) {
    socket_TCP(other).swap(*this);
    return *this;
}

socket_TCP::socket_TCP(int f_d)
    : abstract_socket(f_d) {}

int socket_TCP::receive(char *buf, int size)
{
    int bytes_read = recv(get_fd(), buf, size, 0);
    return bytes_read;
}

socket_TCP::~socket_TCP() {}

//===============================socket_TCP_listener====================================

socket_TCP_listener::socket_TCP_listener(int queue_size)
    : abstract_socket(socket(AF_INET, SOCK_STREAM, 0))
    , m_queue_size(queue_size) {}

socket_TCP_listener::socket_TCP_listener(socket_TCP_listener const &other)
    :abstract_socket(other) {}

socket_TCP_listener & socket_TCP_listener::operator=(socket_TCP_listener const &other) {
    socket_TCP_listener(other).swap(*this);
    return *this;
}

int socket_TCP_listener::receive(char *, int )
{
    return 0;
}

int socket_TCP_listener::send(const char *, int )
{
    return 0;
}

bool socket_TCP_listener::bind(const char * const ip, uint16_t port)
{
    return abstract_socket::bind(ip, port) && listen(get_fd(), m_queue_size) >= 0;
}

socket_TCP socket_TCP_listener::accept()
{
    int fd = ::accept(get_fd(), NULL, NULL);
    return socket_TCP(fd);
}

