#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
 
#define MAX_CHAR 15
#define MAX_ITERATION 10

int main(int argc, char *argv[]){
	
	int ret, fd, count;
	char * str;
	
	fd = open("/dev/mypipe", O_RDWR);
	printf("PRODUCER : %d\n", atoi(argv[1]));
   
	if (fd < 0){
		perror("Error Failed to open the device..!!");
      		return errno;
   	}

	count = 0;
	while(count < MAX_ITERATION) {
		str = (char *)malloc(MAX_CHAR);
		sprintf(str, "P%d: Produces %d", atoi(argv[1]), count);
		ret = write(fd, str, strlen(str));
		printf("PRODUCER %d [MESSAGE]: %s\n", atoi(argv[1]), str);
		free(str);
		count++;
	}
	close(fd);
	return 0;
}
