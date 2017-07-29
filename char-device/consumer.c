#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#define MAX_CHAR 15
#define MAX_ITERATION 10

int main(int argc, char *argv[]){
	
	int ret, fd, count;
	char * receive;
	
	fd = open("/dev/mypipe", O_RDWR);
	printf("CONSUMER : %d\n", atoi(argv[1]));
   
	if (fd < 0){
		perror("Error Failed to open the device..!!");
      		return errno;
   	}

	count = 0;
	while(count < MAX_ITERATION) {
		receive = (char *)calloc(MAX_CHAR, sizeof(char));
		ret = read(fd, receive, MAX_CHAR);
		printf("CONSUMER %d [MESSAGE]: %s\n", atoi(argv[1]), receive);
		free(receive);
		count++;
	}
	close(fd);
	return 0;
}
