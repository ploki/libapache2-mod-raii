#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#define SZ 65536
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#define _FILE_OFFSET_BITS 64

volatile int reopen=0;
volatile int exit_request=0;

void raiilog_reopen(int sig) {
	reopen=1;
}
void raiilog_exit(int sig) {
	exit_request=sig;
}


int main (int argc, char **argv) {
	char buf[SZ];

	if ( argc != 2 ) {
		fprintf(stderr,"%s: usage: %s /path/to/logfile\n",argv[0],argv[0]);
		exit(1);
	}

	const char *logfilename = argv[1];
	int fd = open(logfilename,O_WRONLY|O_CREAT|O_APPEND|O_LARGEFILE,00640);

	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;

	act.sa_handler=raiilog_reopen;
	sigaction(SIGUSR2,&act,NULL);

	act.sa_handler=raiilog_exit;
	sigaction(SIGTERM,&act,NULL);
	sigaction(SIGUSR1,&act,NULL);
	sigaction(SIGWINCH,&act,NULL);

	if ( fd < 0 ) {
		perror(logfilename);
		exit(2);
	}
	openlog(argv[0],LOG_PID,LOG_DAEMON);

	for (;;) {
		int n = read(0,buf,SZ);
		int open_errno=errno;

		if ( exit_request ) {
			syslog(LOG_NOTICE,"exit with signal %d",exit_request);
			close(fd);
			exit(0);
		}
		if ( reopen ) {
			syslog(LOG_NOTICE,"reopening %s",logfilename);
			close(fd);
			fd = open(logfilename,O_WRONLY|O_CREAT|O_APPEND|O_LARGEFILE,00640);
			if ( fd < 0 ) {
				syslog(LOG_WARNING,"exit with error on open: %s",strerror(errno));
				break;
			}
			reopen = 0;
			if ( n < 0 ) {
				if ( open_errno == EINTR ) continue;
				syslog(LOG_WARNING,"continue with error on read: %s",strerror(open_errno));
				continue;
			}
		}
		if ( n < 0 ) {
			syslog(LOG_WARNING,"exit with error on read: %s",strerror(open_errno));
			break;
		}
		if ( n == 0 ) { 
			syslog(LOG_NOTICE,"disconnected from apache2");
			exit(0); }

		if ( flock(fd,LOCK_EX) < 0 ) {
			syslog(LOG_WARNING,"exit with error on flock: %s",strerror(errno));
			break;
		}

		int ret=0;
		while ( (ret=write(fd,buf+ret,n-ret)) > 0) {}
		if ( ret < 0 ) {
			syslog(LOG_WARNING,"exit with error on write: %s",strerror(errno));
			break;
		}
		if ( flock(fd,LOCK_UN) < 0 ) {
			syslog(LOG_WARNING,"exit with error on flock: %s",strerror(errno));
			break;
		}
	}

	return 2;
}
