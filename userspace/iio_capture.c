#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static long long ns_now(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;return (long long)t.tv_sec*1000000000LL+t.tv_nsec;}
int main(int argc,char **argv){const char *path=argc>1?argv[1]:"/dev/iio:device0";long seconds=argc>2?strtol(argv[2],NULL,10):5;int fd=open(path,O_RDONLY|O_NONBLOCK);if(fd<0){fprintf(stderr,"open %s: %s\n",path,strerror(errno));return 1;}unsigned char buf[4096];unsigned long long bytes=0,reads=0;long long start=ns_now(),end=start+(long long)seconds*1000000000LL;while(ns_now()<end){struct pollfd p={.fd=fd,.events=POLLIN};int r=poll(&p,1,100);if(r<0){if(errno==EINTR)continue;perror("poll");break;}if(r==0)continue;ssize_t n=read(fd,buf,sizeof(buf));if(n>0){bytes+=(unsigned long long)n;reads++;}else if(n<0&&errno!=EAGAIN&&errno!=EINTR){perror("read");break;}}long long elapsed=ns_now()-start;double secs=elapsed/1e9;printf("device=%s elapsed_s=%.3f bytes=%llu reads=%llu throughput_Bps=%.1f\n",path,secs,bytes,reads,secs>0?bytes/secs:0);close(fd);return 0;}
