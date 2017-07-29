#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<unistd.h>
#include<time.h>

int main() {
	int size;
	int x = 1;
	struct timespec *current_time;
	current_time = (struct timespec*)malloc(sizeof(struct timespec)); 

	printf("Get Current System Time:\n1. Get current time\n2. Check failure\n");
	scanf("%d", &size);
	
	
	switch(size) {
	case 1: x = syscall(326, current_time, sizeof(struct timespec));
		break;
	case 2: x = syscall(326, current_time, -1);
		break;	

	default: break;
	}

	if(x < 0) {
		printf("Invalid argument\n");
	}
	else {
		printf("Current Time(Nanosecond) : %ld \n", current_time->tv_nsec);
	}	
	return 0;
}
