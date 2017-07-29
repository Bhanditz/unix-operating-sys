#include<time.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include <linux/ioctl.h>

#define MAX_BUFFER 500
#define OUT_FILE "plot"
#define QUERY_GET_VALUE 1

typedef struct {
	struct timespec cTime;
	long virtual_addr;

}ProcessInfo;

long getNanoSeconds(ProcessInfo p) {
	if(p.cTime.tv_nsec != 0 && p.cTime.tv_sec != 0) {
		return (p.cTime.tv_sec * 1000000000) + (p.cTime.tv_nsec);
	}
	return 0;
}

void writeToFile(ProcessInfo* processInfo) {
	int i;
	FILE * output;
	output = fopen(OUT_FILE, "w");
	fprintf(output, "# System-Time	 Virtual-Address\n");

	long min = getNanoSeconds(processInfo[0]);
	for(i = 1; i < MAX_BUFFER; i++) {
		if(min > getNanoSeconds(processInfo[i]) ) {
			min = getNanoSeconds(processInfo[i]);
		}
	}
	for(i = 0; i < MAX_BUFFER; i++) {
		if(processInfo[i].cTime.tv_nsec != 0 && processInfo[i].cTime.tv_sec != 0) {			
			fprintf(output, "%ld\t%ld\n", 
				(getNanoSeconds(processInfo[i]) - min), processInfo[i].virtual_addr);
		}
	}
	fclose(output);
}

int main(int argc, char *argv[]){
	
	int fd;
	ProcessInfo* processInfo;
	fd = open("/dev/jprobe", O_RDWR);
	if(fd < 0) {
		perror("Failed to open device...!!!");
		return -1;	
	}
	processInfo = (ProcessInfo*)malloc(MAX_BUFFER * sizeof(ProcessInfo));
	if(processInfo == NULL) {
		perror("Failed to allocate memory...!!!");
		close(fd);
		return -1;
	}
	ioctl(fd, QUERY_GET_VALUE, processInfo);
	writeToFile(processInfo);
	printf("-------FILE COPIED-------\n");
	close(fd);
	return 0;
}
