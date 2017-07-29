#include<stdio.h>
#include<unistd.h>
#include<linux/unistd.h>
#include<linux/time.h>

int main() {
	int id;
	int x = 1;
	printf("Experimenting with BAD code:\n1. Division by zero\n2. Dereferencing a null pointer\n3. Returning incorrect value\n5. Other\n");
	printf("Enter ID:\n");
	scanf("%d", &id);

	switch(id) {
	case 1:	x = syscall(327); // Sys crash 1: Divide by zero
		break; 
	case 2:	x = syscall(328); // Sys crash 2 : Deferencing a null pointer
		break;
	case 3: x = syscall(329); // Sys crash 3 : Returning -ve value
		break;
	case 5: x = syscall(331); // Sys crash 5 : Stack overflow (system hang)
		break;
	default: break;
	}
	if(x < 0) {
		printf("Failure : %d\n", x);	
	} 
	else {
		printf("Success : %d\n", x);	
	}
	return 0;
}
