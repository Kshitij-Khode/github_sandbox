/*

Proxy Lab
Name: Kshitij Khode
Andrew ID: kkhode

Application launches a proxy socket that listens for connections and
launches threads that address their request by fetching HTTP response
from the target server. If the requested URI has never been addressed,
then it stores the URI into it's local cache. If the requested URI is
available in this cache then no outgoing connection to the target
server is made, and the cache'd URI is returned to the requestng
connection. Cache insertion is array storage without any tag mapping.
Eviction policy is LRU.

*/

#include <stdio.h>
#include <ctype.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_CACHE_LINES MAX_CACHE_SIZE/MAX_OBJECT_SIZE

// #define DEBUG
#ifdef  DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; "
                                    "Linux x86_64; rv:10.0.3) "
                                    "Gecko/20120305 Firefox/10.0.3\r\n";
typedef struct cline_t {
    int  age;
    char tag[MAXBUF];
    char obj[MAX_OBJECT_SIZE];
} cline_t;

cline_t  pcache[MAX_CACHE_LINES];
pthread_rwlock_t pl;

int  check_args(int argc, char** argv);
int  do_proxy(int fd);
void *thread(void *vargp);
void save_cache(char *tag, char *obj);
int  load_cache(char *tag, char *obj);

int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);

    int       i, listfd, *connfd;
    char      hostname[MAXBUF], port[MAXBUF];
    socklen_t clientlen;
    struct    sockaddr_in clientaddr;
    pthread_t tid;

    if (!check_args(argc, argv)) {
        dprintf("help>> usage: %s <port>\n", argv[0]);
        return -1;
    }

    for (i = 0; i < MAX_CACHE_LINES; i++) strcpy(pcache[i].tag, "");
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


/*
Inputs:
argc: No of arguments provided to the application
argv: String representation of the arguments provided to the application

Outputs:
1: If argument provided to application is 1 port number containing only
   numbers
0: Otherwise
*/
int check_args(int argc, char** argv) {
    char* s = argv[1];

    if (argc != 2) return 0;
    while (*s) {
        if (!isdigit(*s++)) return 0;
    }
    return 1;
}

/*
Inputs:
fd: file descriptor for incoming connection received by the proxy

Outputs:
1: if connection is incoming proxy connection is properly serviced
0: Otherwise
*/
int do_proxy(int fd) {
    rio_t rior, riow;
    char  uri[MAXBUF], bufr[MAXBUF], buf0[MAXBUF], buf1[MAXBUF], bufw[MAXBUF];
    char  bufo[MAX_OBJECT_SIZE];
    char  *dom_p, *port_p, *path_p;
    int   hp, servfd, br, tr = 0;

    Rio_readinitb(&rior, fd);
    Rio_readlineb(&rior, bufr, MAXBUF);

    if (strcmp(bufr, "") == 0) {
        dprintf("log>> received null str as request\n");
        return 0;
    }
    dprintf("log>> orig_request l1:%s", bufr);

    strcpy(buf0, bufr);
    strtok(buf0, "//");
    dom_p  = strtok(NULL, " ");
    dom_p  = strtok_r(dom_p, "/", &path_p);

    if (strstr(dom_p, ":")) dom_p  = strtok_r(dom_p, ":", &port_p);
    else                    port_p = "80";

    dprintf("log>> dom_p:%s, port_p:%s, path_p:%s\n", dom_p, port_p, path_p);

    strcat(uri, dom_p);
    strcat(uri, ":");
    strcat(uri, port_p);
    strcat(uri, "/");
    strcat(uri, path_p);
    if (!load_cache(uri, bufo)) {
        dprintf("log>> %s not found in cache\n", uri);

        sprintf(bufw, "GET /%s HTTP/1.0\r\n", path_p);
        while (Rio_readlineb(&rior, bufr, sizeof(bufr))) {
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

        if ((servfd = open_clientfd(dom_p, port_p)) < 0) {
            dprintf("log>> (%s:%s) connection error\n", dom_p, port_p);
            return 0;
        }

        if (rio_writen(servfd, bufw, strlen(bufw)) <= 0) {
            dprintf("log>> error proxy_request write\n");
            close(servfd);
            return 0;
        }
        Rio_readinitb(&riow, servfd);

        dprintf("log>> proxy_response,\n");

        while ((br = Rio_readlineb(&riow, bufr, sizeof(bufr)))) {
            tr += br;
            if(tr <= sizeof(bufo)) strcat(bufo, bufr);
            if (rio_writen(fd, bufr, br) < 0) {
                dprintf("log>> error proxy_response write\n");
                close(servfd);
                return 0;
            }
        }
        close(servfd);
        if (tr <= MAX_OBJECT_SIZE) save_cache(uri, bufo);
        return 1;
    }
    dprintf("log>> %s was found in cache\n", uri);
    if (rio_writen(fd, bufo, sizeof(bufo)) < 0) {
        dprintf("log>> error responding via cache\n");
        return 0;
    }
    return 1;
}

/*
Description:
Function detaches thread to avoid memory leak issues, frees incoming proxy
connection's file descriptor malloc, calls function that addresses the
incoming request and closes the incoming connection. Malloc is used for pass
by value rather than pass by reference, in order to avoid memory leak issues.

Inputs:
vargp: pointer to array of arguments passed to the thread when pthread_create
       was called
*/
void *thread(void *vargp) {
    pthread_detach(pthread_self());
    int connfd = *((int*)vargp);
    free(vargp);
    do_proxy(connfd);
    close(connfd);
    return NULL;
}

/*
Description:
Looks up through the entire cache for any line that isn't currently storing
a URI and the URI in such a line. Tag for the line is the URI string itself.
If no such line is found, a line is evicted based on LRU policy.

Inputs:
tag: URI string corresponding to the URI that requiress being stored in cache
obj: pass by reference of serialized version of the URI required storing in
     the cache.
*/
void save_cache(char *tag, char *obj) {
    int i, mi = 0, mage = 0;
    for (i = 0; i < MAX_CACHE_LINES; i++) {
        if (pcache[i].age > mage) {
            mi   = i;
            mage = pcache[i].age;
        }
        if (strcmp(pcache[i].tag, "") == 0) {
            pcache[i].age = 0;
            break;
        }
        if (strcmp(pcache[i].tag, tag) == 0) {
            pcache[i].age++;
            break;
        }
    }
    pthread_rwlock_wrlock(&pl);
    if (i == MAX_CACHE_LINES) {
        strcpy(pcache[mi].tag, tag);
        strcpy(pcache[mi].obj, obj);
    }
    else {
        strcpy(pcache[i].tag, tag);
        strcpy(pcache[i].obj, obj);
    }
    pthread_rwlock_unlock(&pl);
}

/*
Inputs:
tag: URI string corresponding to the URI being stored in the cache

Outputs:
obj: return by reference of serialized version of the URI being requested that
     was stored in the cache.
1: If requested URI was found in the cache.
0: Otherwise
*/
int load_cache(char *tag, char *obj) {
    int i;
    pthread_rwlock_rdlock(&pl);
    for (i = 0; i < MAX_CACHE_LINES; i++) {
        if (strcmp(pcache[i].tag, tag) == 0) {
            strcpy(obj, pcache[i].obj);
            pthread_rwlock_unlock(&pl);
            return 1;
        }
    }
    pthread_rwlock_unlock(&pl);
    return 0;
}
