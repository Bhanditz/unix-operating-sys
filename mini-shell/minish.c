#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "job.h"

#define BUFFER_SIZE 1024
#define TOKEN_SIZE 64
#define TRUE 1
#define FALSE 0
#define STDOUT_FILENO 1					//fd of stdout
#define STDIN_FILENO 0					//fd of stdin
#define RT_SUCCESS 1
#define RT_FAIL	0


pid_t pid;						//child pid
pid_t job_id;
pid_t pgrp_id;						//session id
pid_t session_id = -1;

int op_redirect_flag = FALSE;
int ip_redirect_flag = FALSE;
int tok_position = 0;
int pipe_count = 0;

pid_t temp_job_id = -1;

void INTH_CtrlC(int sigid)
{
	kill(pid, SIGINT);				//SIGINT
}

void INTH_CtrlZ(int sigid)
{
	kill(pid, SIGTSTP);
}

void sig_handler(int signo)
{
	if(signo == SIGTSTP)
		kill(temp_job_id, SIGSTOP);
	else
		kill(temp_job_id, signo);
}

void redirect_output(char** command, int* fd_out)
{
	int fdout = creat(command[tok_position - 1], 0644);
	int newfd = dup2(fdout, *fd_out);
	close(fdout);
	command[tok_position - 1] = 0;
	op_redirect_flag = FALSE;
}

void redirect_input(char** command)
{
	int fdin = open(command[tok_position - 1], O_RDONLY);
	int newfd = dup2(fdin, STDIN_FILENO);
	close(fdin);
	command[tok_position- 1] = 0;
	ip_redirect_flag = FALSE;
}

int minish_read_command(char** buffer)
{
	*buffer = (char *)calloc(0, BUFFER_SIZE * sizeof(char));
	fgets(*buffer, BUFFER_SIZE, stdin);
	
	if(strlen(*buffer) != 1)
	{
		(*buffer)[strlen(*buffer) - 1] = (*buffer)[strlen(*buffer)];	//replace newline char by null char
		return 0;
	}
	return -1;
}

char** split(char* command, const char* tok)
{
	char** args = NULL;
	char* token;
	tok_position = 0;
	if(command != NULL)
	{
		args = (char**)malloc(TOKEN_SIZE * sizeof(char*));
		token = strtok(command, tok);
		while(token != NULL)
		{
			if(strcmp(token, ">") == 0)
			{
				op_redirect_flag = TRUE;
			}
			else if(strcmp(token, "<") == 0)
			{
				ip_redirect_flag = TRUE;
			}
			else
			{
				//*async = FALSE;
				args[tok_position++] = token;
			}
			token = strtok(NULL, tok);
		}
		args[tok_position] = 0;
		return args;
	}
	return NULL;
}

int get_pipe_count(char* command)
{
	char* ch = command;
	int pipe_cnt = 0;
	while(ch != NULL)
	{
		ch = strchr(ch, '|');
		if(ch != NULL)
		{
			pipe_cnt++;
			ch++;
		}
	}
	return pipe_cnt;
}

int execute_command(JOB** head, char** args, int async)
{
	pid_t wpid;
	int status;
	int sig_index = 1;				//signal
	if(strcmp(args[0], "cd") == 0)	
	{
		//changing the working directory of minish process
		if(args[1] != 0)
			chdir(args[1]);	//getenv("HOME")
		else
			chdir(getenv("HOME"));
	}
	else if(strcmp(args[0], "fg") == 0)
	{
		//bringing the background process to foreground
		JOB* temp = find_job(head, atoi(args[1]));
		if(temp != NULL) {
			signal(SIGTSTP, sig_handler);
			signal(SIGINT, sig_handler);
			//signal(SIGTTOU, SIG_IGN);
			printf("%s\n", temp->process_name);
			temp_job_id = temp->pid;
			tcsetpgrp(STDIN_FILENO, temp->pid);
			kill(temp->pid, SIGCONT);
			do{
				waitpid(temp->pid, &status, WUNTRACED | WCONTINUED);
			}while(!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
			if(WIFEXITED(status) || WIFSIGNALED(status))
				remove_Jobs(head, temp->pid);
		} 
		else 
		{
			printf("bash: fg: %s: no such job\n", args[1]);
		}
	}
	else if(strcmp(args[0], "jobs") == 0)
	{
		display_all_job(head); 
	}
	else if(strcmp(args[0], "kill") == 0)
	{
		remove_Jobs(head, atoi(args[1]));
	}
	else
	{
		if((pid = fork()) == -1)
			perror("\n Fork Failed");
		int fd_out = STDOUT_FILENO;
		if(pid == -1)
		{
			printf("\n fork faild");
		}
		else if(pid == 0)
		{
			for(; sig_index < NSIG; ++sig_index)	//catch all signals in child which can be transmmitted to all its childs
				signal(sig_index, sig_handler);
			if(op_redirect_flag)	//redirect output of command to file.
			{
				redirect_output(args, &fd_out);
			}
			if(ip_redirect_flag)	//redirect input from file to command
			{
				redirect_input(args);
			}
			
			//pid_t spid = setsid();
			//printf("\n session pid is %d", spid);
			
			//child process
			if(execvp(args[0], args) == -1)
			{
				printf("\n error\n");
				return -1;
			}
		}
		else if(pid > 0)
		{
			signal(SIGINT, INTH_CtrlC);
			signal(SIGTSTP, INTH_CtrlZ);
			//session_id = getsid(pid);
			//printf("\n session id is %d", (int)session_id);
			if(!async)
			{
				//No asynchronous execution of child so wait on pid
				do 
				{
				    wpid = waitpid(pid, &status, WUNTRACED | WCONTINUED);
				    if (wpid == -1) 
				    {
					perror("waitpid");
					exit(EXIT_FAILURE);
			    	    }

				   if (WIFSTOPPED(status)) 
				   {
				   	insert_job(head, wpid, args[0], 0); // create job
					display_job(head, wpid, 1); 
				   } 
				} while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
		     	}
		     	else
		     	{
		     		
		     	}
		}
	}
	return RT_SUCCESS;
}

void execute_pipe_command(char* buffer, int pipe_count)
{
	char** pipe_chain_command;// = (char**)malloc((pipe_count + 1) * sizeof(char*));
	int i = 0;
	int status = 0;
	int fds[2];
	pid_t pchild, pchild_2, wpid;
	char** pipe_command;
	int   p[2];
	int   fd_in = 0;
	int fd_out = 1;
	int temp_pipe_count = 0;
	pipe_chain_command = split(buffer, "|");
	dup2(fd_out, 1);
	for(i = 0; i <= pipe_count; ++i)
	{
		pipe_command = split(pipe_chain_command[i], " ");
		pipe(p);
		pchild = fork();
		if(pchild == 0)
		{
			if(op_redirect_flag)	//redirect output of command to file.
			{
				redirect_output(pipe_command, &fd_out);
			}
			if(dup2(fd_in, 0) < 0) //change the input according to the old one 
				 perror("dup2 failed");
			if(i != pipe_count)
				dup2(p[1], 1);
			close(p[0]);
			execvp(pipe_command[0], pipe_command);
		}
		else
		{
			do 
			{
			    wpid = waitpid(pchild, &status, WUNTRACED | WCONTINUED);
			    if (wpid == -1) 
			    {
				perror("waitpid");
				exit(EXIT_FAILURE);
		    	    }

			   i
				printf("process %d stopped by signal %d\n",(int)pid, WSTOPSIG(status));
			   } 
			} while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
			close(p[1]);
			fd_in = p[0];
		}
		free(pipe_command);
		//free(pipe_chain_command);
	}	
}



int execute_jobs(JOB** head, char** args, const char* buf)
{
	job_id = fork();
	if(job_id == -1)
		perror("Fork Faild...");
	else if(job_id == 0)
	{
		if(session_id == -1)
			setsid();
		else
			setpgid(0, session_id);
		if(execvp(args[0], args) == -1)
		{
			printf("\n error\n");
			return -1;
		}
	}
	else
	{
		//async = FALSE;
		session_id = getsid(job_id);
		insert_job(head, job_id, buf, 1); // create job
		display_job(head, job_id, 0); 
	}
}

int is_command_async(const char* command)
{
	char* ret = strchr(command, '&');
	if(ret != NULL)
		return TRUE;
	return FALSE;
}

int is_command_pipe(const char* command)
{
	char* ret = strchr(command, '|');
	if(ret != NULL)
		return TRUE;
	return FALSE;
}

void minish_start(void)
{
	char** args;
	char* command = NULL;
	
	int async = FALSE;
	int pipeline = FALSE;
	int status = 1;
	int ret = 0;
	int pipe_count = 0;
	char *buffer;
	JOB* head = NULL;
	do
	{
		async = FALSE;
		op_redirect_flag = FALSE;
		ip_redirect_flag = FALSE;
		printf("minish>");
		ret = minish_read_command(&command);
		char* ch = NULL;
		buffer = (char*)malloc(strlen(command));
		strcpy(buffer, command);
		if(ret != -1)
		{
			async = is_command_async(command);
			pipeline = is_command_pipe(command);
			if(pipeline)
				pipe_count = get_pipe_count(buffer);
			
			if(strcmp(command, "exit") == 0)
			{
				killpg(getpgrp(), SIGKILL);
			}
			if(args != NULL)
			{
				if(!async && !pipeline)
				{
					//execute normal commands which don't have pipes
					args = split(command, " ");
					status = execute_command(&head, args, async);
				}
				else if(async && pipe_count == 0)
				{
					//printf("\n jobs execution");
					command[strlen(command) - 1] = '\0';			//replace & character with '\0'
					args = split(command, " ");
					status = execute_jobs(&head, args, buffer);
				}
				else
				{
					// execute command which has pipe
					args = split(command, "|");
					execute_pipe_command(buffer, get_pipe_count(buffer));
					pipe_count = 0;
					if(buffer != NULL)
						free(buffer);		
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
		if(command != NULL)
		{
			free(command);
		}
		if(args != NULL)
		{
			free(args);
		}
	}while(status);
	free_all_jobs(&head);
}

int main(void)
{
	char* argv[] = {"gedit", "minish.cpp", 0};
	minish_start();
	
	return 0;
}
