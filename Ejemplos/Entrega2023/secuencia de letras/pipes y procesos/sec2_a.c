#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	int pipeA[2], pipeB[2], pipeC[2];
	int pid, cantHijos = 3;
	char c;
	
	pipe(pipeA);
	pipe(pipeB);
	pipe(pipeC);
	
	write(pipeA[1],"a", sizeof(char));
	
	for(int i=0;i<cantHijos;i++){
		pid = fork();
		if (pid < 0){ printf("ERROR al crear un proceso. \n"); exit(-1); }
		if (pid == 0){	
			if(i == 0){					//Proceso A
				close(pipeB[0]);
				close(pipeC[0]);
				close(pipeA[1]);
				close(pipeC[1]);		
				while(1){
					read(pipeA[0], &c, sizeof(char));
					printf("A");
					fflush(stdout);
					write(pipeB[1], "a", sizeof(char));
				}
				exit(0);
			}
			
			if(i == 1){					//Proceso B
				close(pipeA[0]); 
				close(pipeC[0]);
				close(pipeB[1]);
				while(1){
					read(pipeB[0], &c, sizeof(char));
					printf("B");
					fflush(stdout);					
					write(pipeC[1], "a", sizeof(char));
					read(pipeB[0], &c, sizeof(char));
					write(pipeA[1], "a", sizeof(char));
				}
				exit(0);
			}
			
			if(i == 2){
				close(pipeA[0]);					//Proceso C
				close(pipeB[0]);
				close(pipeC[1]);
				while(1){
					read(pipeC[0], &c, sizeof(char));
					write(pipeB[1], "a", sizeof(char));
					read(pipeC[0], &c, sizeof(char));
					printf("C");
					fflush(stdout);
					write(pipeB[1], "a", sizeof(char));
				}
				exit(0);
			}
		}
	}
	
	for (int j=0;j<cantHijos;j++){
		wait(NULL);
	}
	return 0;
}
