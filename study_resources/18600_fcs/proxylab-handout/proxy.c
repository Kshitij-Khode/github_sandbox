#include <stdio.h>
#include <ctype.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define DEBUG
#ifdef  DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; "
                                    "Linux x86_64; rv:10.0.3) "
                                    "Gecko/20120305 Firefox/10.0.3\r\n";

int check_args(int argc, char** argv);
int do_proxy(int fd);
void *thread(void *vargp);

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);

    int listfd, *connfd;
    char hostname[MAXBUF], port[MAXBUF];
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    pthread_t tid;

    if (!check_args(argc, argv)) {
        dprintf("help>> usage: %s <port>\n", argv[0]);
        return -1;
    }

    listfd    = Open_listenfd(argv[1]);
    clientlen = sizeof(clientaddr);

    while (1) {
        connfd  = malloc(sizeof(int));
        *connfd = Accept(listfd, (SA*)&clientaddr, &clientlen);
        Getnameinfo((SA*)&clientaddr,
                    clientlen,
                    hostname,
                    MAXBUF,
                    port,
                    MAXBUF,
                    0);
        dprintf("log>> accepted connection from (%s, %s)\n", hostname, port);
        pthread_create(&tid, NULL, thread, connfd);
    }

    return 0;
}

int check_args(int argc, char** argv) {
    char* s = argv[1];

    if (argc != 2) return 0;
    while (*s) {
        if (!isdigit(*s++)) return 0;
    }
    return 1;
}

int do_proxy(int fd) {
    rio_t rior, riow;
    char  bufr[MAXBUF], buf0[MAXBUF], buf1[MAXBUF], bufw[MAXBUF];
    char  *dom_p, *port_p, *path_p;
    int   hp, servfd, br;

    Rio_readinitb(&rior, fd);
    Rio_readlineb(&rior, bufr, MAXBUF);

    dprintf("log>> request l1,\n%s", bufr);

    strcpy(buf0, bufr);
    strtok(buf0, "//");
    dom_p  = strtok(NULL, "/");
    path_p = strtok(NULL, " ");
    if (strstr(dom_p, ":")) dom_p  = strtok_r(dom_p, ":", &port_p);
    else                    port_p = "80";

    sprintf(bufw, "GET /%s HTTP/1.0\r\n", path_p);
    while (Rio_readlineb(&rior, bufr, MAXBUF)) {
        strcpy(buf1, bufr);
        if(strcmp(buf1, "\r\n") == 0) break;
        strtok(buf1, ":");
        if (strcmp(buf1, "Host") == 0) hp = 1;
        if (strcmp(buf1, "User-Agent") == 0) continue;
        if (strcmp(buf1, "Connection") == 0) continue;
        if (strcmp(buf1, "Proxy-Connection") == 0) continue;
        strcat(bufw, bufr);
    }
    if (!hp) {
        sprintf(buf1, "Host: %s\r\n", dom_p);
        strcat(bufw, buf1);
    }
    strcat(bufw, user_agent_hdr);
    strcat(bufw, "Connection: close\r\n");
    strcat(bufw, "Proxy-Connection: close\r\n");
    strcat(bufw, "\r\n");

    dprintf("log>> proxy_request,\n%s", bufw);

    servfd = Open_clientfd(dom_p, port_p);
    Rio_writen(servfd, bufw, strlen(bufw));

    Rio_readinitb(&riow, servfd);
    while ((br = Rio_readnb(&riow, bufr, MAXBUF))) Rio_writen(fd, bufr, br);
    close(servfd);
    return 1;
}

void *thread(void *vargp) {
    int connfd = *((int*)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    do_proxy(connfd);
    close(connfd);
    return NULL;
}
