#ifndef SOCKETS_H
#define SOCKETS_H
#include <inttypes.h>

class abstract_socket {
public:
    explicit abstract_socket(int fd);
    abstract_socket(abstract_socket const &other);
    int get_fd() const;
    char *get_er_message() const;
    bool connect(char const * ip, uint16_t port);
    bool disconect();
    bool is_valid() const;
    void swap(abstract_socket &other);

    virtual bool bind(char const * const ip, uint16_t port);
    virtual int receive(char *buf, int size) = 0;
    virtual int send(char const *buf, int size);
    virtual ~abstract_socket();

private:
    int m_fd;
    int *m_pcount;
};

//====================================================================================================

class socket_UDP: public abstract_socket
{
public:
    socket_UDP();
    socket_UDP(socket_UDP const &other);
    socket_UDP &operator=(socket_UDP const &other);
    int receive(char *buf, int size) override;
    ~socket_UDP();
};

//====================================================================================================
class socket_TCP;

class socket_TCP_listener: public abstract_socket {
public:
    explicit socket_TCP_listener(int queue_size);
    socket_TCP_listener(socket_TCP_listener const &other);
    socket_TCP_listener &operator=(socket_TCP_listener const &other);
    int receive(char *, int) override;
    int send(char const *, int) override;
    bool bind(const char * const ip, uint16_t port) override;
    socket_TCP accept();

private:
    int m_queue_size;
};

//====================================================================================================

class socket_TCP: public abstract_socket
{
public:
    socket_TCP();
    socket_TCP(socket_TCP const &other);
    socket_TCP &operator=(socket_TCP const &other);
    int receive(char *buf, int size) override;
    ~socket_TCP() override;
    friend socket_TCP socket_TCP_listener::accept();
private:
    explicit socket_TCP(int f_d);
};

#endif // SOCKETS_H
