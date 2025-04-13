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
// C++ 
#include <vector>


const size_t k_max_msg = 32 << 20;

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static int32_t read_full (int _fd, uint8_t *_buf, size_t _n)
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

static int32_t write_all(int _fd, const uint8_t *_buf, size_t _n)
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

static void buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len)
{
    buf.insert(buf.end(), data, data + len);
}


static int32_t send_req(int fd, const uint8_t *text, size_t len)
{
    if(len > k_max_msg)
    {
        return -1;
    }

    std::vector<uint8_t> wbuf;
    buf_append(wbuf, (const uint8_t *)&len, 4);
    buf_append(wbuf, text, len);
    return write_all(fd, wbuf.data(), wbuf.size());
}

static int32_t read_res(int fd)
{
    // 4 bytes header
    std::vector<uint8_t> rbuf;
    rbuf.resize(4);
    errno = 0;
    int32_t err = read_full(fd, &rbuf[0], 4);
    if (err)
    {
        if(errno == 0)
        {
            msg("EOF");
        }
        else
        {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf.data(), 4);
    if( len  > k_max_msg)
    {
        msg("too long");
        return -1;
    }

    // reply body
    rbuf.resize(4 + len);
    err = read_full(fd, &rbuf[4], len);
    if(err)
    {
        msg("read() error");
        return err;
    }

    // so something
    printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, &rbuf[4]);
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


    // multiple pipelined requests
    std::vector<std::string> query_list = 
    {
        "hello1", "hello2", "hello3", std::string(k_max_msg, 'z'),
        "hello5",
    };

    for( const std::string &s : query_list)
    {
        int32_t err = send_req(fd, (uint8_t *)s.data(), s.size());
        if (err)
        {
            msg("error");
            goto L_DONE;
        }
    }

    for (size_t i = 0; i < query_list.size(); ++i)
    {
        int32_t err = read_res(fd);
        if(err)
        {
            msg("error");
            goto L_DONE;
        }
    }
    

L_DONE:
    msg("closing");
    close(fd);
    return 0;
}