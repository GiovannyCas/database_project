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

const size_t k_max_msg = 4049;

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
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


static int32_t one_request(int _connfd)
{
    //4 bytes header
    char rbuf[4 + k_max_msg];
    errno= 0;

    int32_t err = read_full(_connfd, rbuf, 4 );
    if(err)
    {
        msg(errno == 0 ? "EOF" : "read() error");
        return err;
    }
    uint32_t len = 0;
    memcpy(&len, rbuf, 4);  //assume litte endian

    if(len > k_max_msg)
    {
        msg("too long");
        return -1;
    }

    //request body
    err = read_full(_connfd, &rbuf[4], len);
    if(err)
    {
        msg("read() error");
        return -1;
    }

    //do_something
    printf("Client says: %.*s\n", len, &rbuf[4]);

    //reply using the same protocal
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(_connfd, wbuf, 4 + len);

}


static void do_something_with(int _connfd)
{
    char rbuf[64] = {};     //Read Buffer - rbuf
    ssize_t n = read(_connfd, rbuf, sizeof(rbuf) - 1);
    if(n < 0 )
    {
        msg("read() error");
        return;
    }

    printf("client says: %s\n", rbuf);

    char wbuf[] = "World";     //Write Buffer - wbuf
    write(_connfd, wbuf, strlen(wbuf));
}


int main ()
{
    // SOCK_STREAM for TCP. SOCK_DGRAM for UDP.
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);        //port
    addr.sin_addr.s_addr = htonl(0);    //wildcard IP 0.0.0.0
    

    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));

    rv = listen(fd, SOMAXCONN);

    while (true) {
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if(connfd < 0)
        {
            continue;   //error
        }

        //only serves one client connection at once
        while (true)
        {
            int32_t err = one_request(connfd);
            if(err)
            {
                break;
            }
        }
      
        
        close(connfd);
    }

    return 0;

}


