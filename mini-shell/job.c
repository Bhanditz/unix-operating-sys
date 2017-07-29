#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "job.h"

void create_job(JOB** head, pid_t pid, char p_name[], int jobid, int inProgress)
{
    JOB* node = NULL;
    JOB* temp = *head;
    node = (JOB*)malloc(sizeof(JOB));
    node->pid = pid;
    strcpy( node->process_name , p_name);
    if(strstr(node->process_name, " &") != NULL) {
    	node->process_name[strlen(node->process_name)-1] = 0;
    }
    node->job_id = ++jobid;
    node->status = inProgress;
    node->next = NULL;
    if(*head == NULL)
    	*head = node;
    else
    {
    	while(temp->next != NULL)
    		temp = temp->next;
    	temp->next = node;
    }
}

void insert_job(JOB** head, pid_t pid, char p_name[], int inProgress) {
    JOB* temp = *head;
    if(*head == NULL) {
        create_job(head, pid, p_name, 0, inProgress);
    }
    else
    {    
    	while(temp->next != NULL) 
    	{
                temp = temp->next;
        }
        create_job(head, pid, p_name, temp->job_id, inProgress);
    }
}


JOB* find_job(JOB** head, int j_id) {
	JOB* job = NULL;
	if(*head != NULL)
	{
		job = *head;
		while(job != NULL) 
		{
			if(job->job_id == j_id) 
			{
				return job;
			}
			job = job->next;
		}
	}
	return job;
}

void remove_Jobs(JOB** head, pid_t pid) {
 	JOB* job = *head;	
 	JOB* temp;
 	while(job != NULL && job->pid != pid )
      	{
      		temp = job;
      		job = job->next;
      	}
      	if(job == *head)
      	{
      		*head = (*head)->next;
      		free(job);
      	}
      	else if(job != NULL)
      	{
      		temp->next = job->next;
      		free(job);
      	}
      		
}

void display_job(JOB** head, pid_t pid, int show_pName) 
{ 
      JOB* job = *head;
      while(job != NULL) 
      {
          if(job->pid == pid) 
          {
          	if(show_pName) 
          	{
          		char* status = (job->status == 1) ? "Running" : "Stopped";
          		printf("[%d] \t %s \t %s\n", (int)job->job_id, status, job->process_name);
          	}
          	else
          	{
          		printf("[%d]  \t %d\n", (int)job->job_id, (int)job->pid);
          	}   
              break;
          }
          job = job->next;
      }
}

void display_all_job(JOB** head) 
{ 
      JOB* job = *head;
      while(job != NULL) 
      {
      		char* status = (job->status == 1) ? "Running" : "Stopped";
		printf("[%d] \t %s \t %s\n", (int)job->job_id, status, job->process_name);
          	job = job->next;
      }
}

void free_all_jobs(JOB** head)
{
	JOB* job;
	while(*head != NULL)
	{
		job = *head;
		*head = (*head)->next;
		free(job);
	}
}
