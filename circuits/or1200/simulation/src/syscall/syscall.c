#include <stdio.h>
#include <sys/time.h>

int _gettimeofday_r( struct timeval *tv, void *tzvp ) {
	tv->tv_sec  = 222; // convert to seconds
	tv->tv_usec = 111; // get remaining microseconds
	return 0;  // return non-zero for error
} // end _gettimeofday()

int main() {
	struct timeval now;
    int rc;
	printf("Running syscall...!\n");
	rc = gettimeofday(&now, NULL);
    if( rc == 0 ) {
        printf("time = %u.%06u\n", now.tv_sec, now.tv_usec);
    } else {
        printf("gettimeofday() failed.\n");
        return 10;
    }
	return 0;
}
