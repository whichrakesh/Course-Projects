#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>

#define MAXLINE 1000
#define DEBUG 0

/* Function declarations and globals */
int parent_pid ;
char ** tokenize(char*) ;
int execute_command(char** tokens) ;
void my_handler(int sig);

int main(int argc, char** argv){
	parent_pid = getpid() ;

	/* Set (and define) appropriate signal handlers */
	/* Exact signals and handlers will depend on your implementation */
	// signal(SIGINT, quit);
	// signal(SIGTERM, quit);

	char input[MAXLINE];
	char** tokens;
	signal(SIGINT,&my_handler);
	signal(SIGQUIT,&my_handler);

	while(1) {
		char pwd[MAXLINE];
		getcwd(pwd,sizeof(pwd));
		fflush(stdout);
		printf("----------------------------------------------------------------\n");
		fputs(pwd,stdout);
		printf("$ "); // The prompt
		fflush(stdin);

		char *in = fgets(input, MAXLINE, stdin); // Taking input one line at a time
		//Checking for EOF
		if (in == NULL){
			if (DEBUG) 
				printf("jash: EOF found. Program will exit.\n");
			break ;
		}

		// Calling the tokenizer function on the input line
		tokens = tokenize(input);
		// Executing the command parsed by the tokenizer
		execute_command(tokens) ;

		// Freeing the allocated memory
		int i ;
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);
	}

	/* Kill all Children Processes and Quit Parent Process */
	return 0 ;
}

/*The tokenizer function takes a string of chars and forms tokens out of it*/
char ** tokenize(char* input){
	int i, doubleQuotes = 0, tokenIndex = 0, tokenNo = 0 ;
	char *token = (char *)malloc(MAXLINE*sizeof(char));
	char **tokens;

	tokens = (char **) malloc(MAXLINE*sizeof(char*));

	for(i =0; i < strlen(input); i++){
		char readChar = input[i];

		if (readChar == '"'){
			doubleQuotes = (doubleQuotes + 1) % 2;
			if (doubleQuotes == 0){
				token[tokenIndex] = '\0';
				if (tokenIndex != 0){
					tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0;
				}
			}
		} else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		} else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		} else {
			token[tokenIndex++] = readChar;
		}
	}

	if (doubleQuotes == 1){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
			strcpy(tokens[tokenNo++], token);
		}
	}

	tokens[tokenNo] = NULL ;
	return tokens;
}

/* my_handler handles the termination of child processes on ctrl-c and ctrl-\ */
void my_handler(int sig){
	if(getpid() != parent_pid){
		exit(0);
	} 
}


int execute_command(char** tokens) {
	/*
	Takes the tokens from the parser and based on the type of command
	and proceeds to perform the necessary actions.
	Returns 0 on success, -1 on failure.
	*/
	if (tokens == NULL) {
		return -1 ; 				// Null Command
	} else if (tokens[0] == NULL) {
		return 0 ;					// Empty Command
	} else if (!strcmp(tokens[0],"exit")) {
		/* Quit the running process */
		exit(0);
	} else if (!strcmp(tokens[0],"cd")) {
		/* Change Directory, or print error on failure */
		if(tokens[1] != NULL){
			int ret = chdir(tokens[1]);
			if(ret == 0){
				return 0;
			} else {
				fputs("directory does not exist\n",stderr);
				return -1;
			}
		}
		return 0 ;
	} else if (!strcmp(tokens[0],"parallel")) {
		/* Analyze the command to get the jobs */
		/* Run jobs in parallel, or print error on failure */
		char** tokens1;
        char* command = (char*) malloc(MAXLINE*sizeof(char));
        int index;
        strcpy(command,"");        
        for(index = 1 ; ; index++){
            if(tokens[index] != NULL && strcmp(tokens[index],":::")){
                strcat(command,tokens[index]);
                strcat(command," ");
            }
            else{            	
            	if(DEBUG)
            		printf("%s\n",command);
            	int child_pid = fork();
                if(child_pid == 0){
                	tokens1 = tokenize(command);
                	int i;
                	int result = execute_command(tokens1);
                	if(result == -1){
                		fputs("error in executing command\n",stderr);
                	}
                	for(i = 0; tokens1[i] != NULL; i++)
						free(tokens1[i]);
					free(tokens1);
					exit(0);					
                } 
                if(tokens[index] == NULL)
            		break;            	                
                strcpy(command,""); 
            }
        }
        signal(SIGCHLD, SIG_IGN);       
        while (waitpid(-1, NULL, 0) >= 0) {
        	printf("%d\n",errno);
		   	if (errno == ECHILD) {
		      	break;
		   	}
		}
        fflush(stdout);
        //fflush(stderr);
        //printf("how come this came-------------------->\n");
        free(command);        		
		return 0 ;		
	} else if (!strcmp(tokens[0],"sequential")) {
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */		
        char** tokens1;
        char* command = (char*) malloc(MAXLINE*sizeof(char));
        int return_value = 0; 
        int result,index;
        strcpy(command,"");
        for(index = 1 ; ; index++){
            if(tokens[index] != NULL && strcmp(tokens[index],":::")){
                strcat(command,tokens[index]);
                strcat(command," ");
            }
            else{
            	if(DEBUG)
            		printf("%s\n",command);
            	tokens1 = tokenize(command);
                result = execute_command(tokens1);
                int i;
     			for(i = 0; tokens1[i] != NULL; i++)
					free(tokens1[i]);
                free(tokens1);
                strcpy(command,"");
                if(result == -1){
                	return_value = -1;
                    break;
                }
                if(tokens[index] == NULL)
            		break;                            
            }
        }        
        free(command);	
		return return_value ;					// Return value accordingly
	} else if(!strcmp(tokens[0],"sequential_or")){
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */
        char** tokens1;
        char* command = (char*) malloc(MAXLINE*sizeof(char));
        int count = 0;
        int return_value = -1; 
        int result,index;
        strcpy(command,"");
        for(index = 1 ; ; index++){
            if(tokens[index] != NULL && strcmp(tokens[index],":::")){
                strcat(command,tokens[index]);
                strcat(command," ");
                count++;
            }
            else{
            	if(DEBUG)
            		printf("%s\n",command);
            	tokens1 = tokenize(command);
                result = execute_command(tokens1);
                int i;
     			for(i = 0; tokens1[i] != NULL; i++)
					free(tokens1[i]);
                free(tokens1);
                strcpy(command,"");
                if(result == 0){
                	return_value = 0;
                    break;
                }           
                count = 0;
                if(tokens[index] == NULL)
            		break;
            }
        }        
        free(command);		
		return return_value ;
	} else {
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int pid = fork() ;
		if (pid == 0) {
			if (!strcmp(tokens[0],"run")) {
				/* Locate file and run commands */
				/* May require recursive call to execute_command */
				/* Exit child with or without error */
				if(tokens[1] != NULL){
					char* filename = tokens[1];
					FILE *fp;
					fp = fopen(filename,"r");
					char command[MAXLINE];
					if(fp != NULL){
						while(fgets(command,sizeof(command),fp) != NULL){
							printf("command to execute: %s",command);
							fflush(stdout);
							char** tokens1 = tokenize(command);
							int ret = execute_command(tokens1);
							if(ret == -1){
								//fprintf(stderr,"error in executing command: %s\n", command);
								//fflush(stderr);
							} else {
								if(DEBUG)
									printf("command successful\n");
							}
							//sleep(1);
							int i ;
							for(i=0;tokens1[i]!=NULL;i++){
								free(tokens1[i]);
							}
							free(tokens1);

						}
					} else {
						fputs("File not found\n",stderr);
						exit(-1);
					}
				}
				exit (0) ;
			}
			else {
				/* File Execution */
				/* Print error on failure, exit with error*/
				if(execvp(tokens[0],tokens) != -1)	{
					printf("file executed successfully\n");
					exit(0);
				} else if(errno == ENOENT) {
					fputs("file not found\n",stderr);
					exit(-1);
				}	else {
					fputs("error in executing command\n",stderr);
					exit(-1);
				}
			}
		}
		else {
			/* Parent Process */
			/* Wait for child process to complete */
			int status;
			waitpid(pid,&status,0);
			//printf("%d\n",WEXITSTATUS(status));
			if(WIFEXITED(status) && WEXITSTATUS(status) == 0)
				return 0;
			else
				return -1;
		}
	}
	return -1 ;
}
