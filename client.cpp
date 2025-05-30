#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>


const size_t k_max_msg = 4096;

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static int32_t read_full (int _fd, char *_buf, size_t _n)
{
    while(_n > 0)
    {
        ssize_t rv = read(_fd, _buf, _n);
        if(rv <= 0)
        {
            return -1;
        }

        assert((size_t)rv <= _n);
        _n -= (size_t)rv;
        _buf += rv;

    }
        return 0;
}

static int32_t write_all(int _fd, const char *_buf, size_t _n)
{
    while( _n > 0)
    {
        ssize_t rv = write(_fd, _buf, _n);
        if(rv <= 0)
        {
            return -1;
        }
        assert((size_t)rv <= _n);
        _n -= (size_t)rv;
        _buf += rv;
    }
    return 0;
}

static int32_t query(int _fd, const char *_text)
{
    uint32_t len = (uint32_t)strlen(_text);

    if(len > k_max_msg)
    {
        return -1;
    }

    //send request 
    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], _text, len);
    if(int32_t err = write_all(_fd, wbuf, 4 + len))
    {
        return err;
    }

    //4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(_fd, rbuf, 4);
    if(err)
    {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }

    memcpy(&len, rbuf, 4);
    if(len > k_max_msg)
    {
        msg("too long");
        return -1;
    }


    //reply body
    err = read_full(_fd, &rbuf[4], len);
    if(err)
    {
        msg("read() error");
        return err;
    }

    //do something
    printf("server says: %.*s\n", len, &rbuf[4]);
    return 0;

}


int main()
{

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  //127.0.0.1

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));

    if(rv)
    {
        die("connect");
    }

    //send multiple requets
    int32_t err = query(fd, "hello1");
    if(err)
    {
        goto L_DONE;
    }

    err = query(fd, "hello2");
    if(err)
    {
        goto L_DONE;
    }
    err = query(fd, "hello3");
    if(err)
    {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}