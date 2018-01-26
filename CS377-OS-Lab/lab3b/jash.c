#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <time.h>

#define MAXLINE 1000
#define DEBUG 0

/* Function declarations and globals */
int parent_pid ;
int background_processes[100]; 
int back_count = 0;
char ** tokenize(char*) ;
int execute_command(char** tokens) ;
void my_handler(int sig);
static void my_parent_handler( int sig, siginfo_t* siginfo, void* context);

int main(int argc, char** argv){
	parent_pid = getpid() ;

	/* Set (and define) appropriate signal handlers */
	/* Exact signals and handlers will depend on your implementation */
	// signal(SIGINT, quit);
	// signal(SIGTERM, quit);

	signal(SIGINT,&my_handler);
	signal(SIGQUIT,&my_handler);
	char input[MAXLINE];
	char** tokens;
	int i;
	for( i = 0; i < 100 ; i++){
		background_processes[i] = -1;
	}
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
		}
		else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		}
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(MAXLINE*sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else {
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
	if(sig == SIGINT || sig == SIGQUIT){
		if(getpid() != parent_pid){
			exit(0);
		}
	}
}

int runNow(char* tokenMinute, char* tokenHour){
    time_t now;
    struct tm *now_tm;
    int hourNow, minuteNow;

    now = time(NULL);
    now_tm = localtime(&now);
    hourNow = now_tm->tm_hour;
    minuteNow = now_tm->tm_min;
    int hour = atoi(tokenHour);
    int minute = atoi(tokenMinute);


    if(!strcmp(tokenHour,"*") && !strcmp(tokenMinute,"*")) return 1;
    else if(!strcmp(tokenHour,"*") && minute == minuteNow) return 1;
    else if(hour == hourNow && minute == minuteNow) return 1;
    else if(hour == hourNow && strcmp(tokenMinute,"*")) return 1;
    else return 0;
}
static void my_parent_handler( int sig, siginfo_t* siginfo, void* context){
	int i,is_back = 0;
	for( i = 0; i < 100 ; i++){
		if(background_processes[i] == siginfo->si_pid){			
			is_back = 1;
			background_processes[i] = -1;
			back_count--;
		}
	}
	if(is_back){
		printf("\n[%d] Done with :%d status\n",siginfo->si_pid,siginfo->si_status);
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
	}
	else if (tokens[0] == NULL) {
		return 0 ;					// Empty Command
	}
	else if (!strcmp(tokens[0],"exit")) {
		/* Quit the running process */
		kill(-getpgid(parent_pid),SIGTERM);
		exit(0);
	}
	else if (!strcmp(tokens[0],"cd")) {
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
	}
	else if (!strcmp(tokens[0],"parallel")) {
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
		   	if (errno == ECHILD) {
		      	break;
		   	}
		}
        fflush(stdout);
        //fflush(stderr);
        //printf("how come this came-------------------->\n");
        free(command);
		return 0 ;
	}
	else if (!strcmp(tokens[0],"sequential")) {
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
	}
	else if(!strcmp(tokens[0],"sequential_or")){
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
	}
	else if(!strcmp(tokens[0],"cron")){
        if(fork() == 0){
            char* filename = tokens[1];
            if(tokens[1] != NULL){
                FILE *fp;
                fp = fopen(filename,"r");
                char command[MAXLINE];
                while(1){
                    fp = fopen(filename,"r");
                    if(fp != NULL){
                        //printf("ghanta\n");
                        while(fgets(command,sizeof(command),fp) != NULL){
                            fflush(stdout);
                            //printf("running");
                            char** tokens1 = tokenize(command);
                            int timeMatched = runNow(tokens1[1], tokens1[2]);
                            if(timeMatched){
                                if(fork() == 0){
                                    int ret = 0;
                                    ret = execute_command(tokens1 + 3);
                                    if(ret == -1){
                                        //fprintf(stderr,"error in executing command: %s\n", command);
                                        //fflush(stderr);
                                        exit(-1);
                                    }
                                    else {
                                        if(DEBUG)
                                            printf("command successful\n");
                                        exit(0);
                                    }
                                    //sleep(1);
                                }
                                else {
                                    int i ;
                                    for(i=0;tokens1[i]!=NULL;i++){
                                        free(tokens1[i]);
                                    }
                                    free(tokens1);
                                }
                            }
                        }
                    }
                    else {
                        fputs("File not found\n",stderr);
                        exit(-1);
                    }
                    sleep(60);
                }

            }
            exit (0) ;
        }
	}
	else {
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int index,run_in_background = 0;
		for (index = 0; tokens[index] != NULL; index++){
			if(!strcmp(tokens[index],"&")){
            	run_in_background = 1;
				struct sigaction sig;
				memset(&sig, '\0',sizeof(sig));				
				sig.sa_sigaction = &my_parent_handler;
				sig.sa_flags = SA_SIGINFO | SA_RESTART;
				if(sigaction(SIGCHLD,&sig,NULL) < 0){
					perror("sigaction");
					return -1;
				}
            }
		}
		int pid = fork() ;
		if (pid == 0) {
			int index;
			int in = -1, out = -1;
			int less_found = 0, greater_found = 0, greater_greater_found = 0;
			int pipe_found = 0;
			int index_of_pipe = -1;	
			if(run_in_background){
				signal(SIGINT,SIG_IGN);
				signal(SIGQUIT,SIG_IGN);				
			}					
			for (index = 0; tokens[index] != NULL; index++){
				if(!strcmp(tokens[index],"<")){
                    if(less_found == 1){
                        fputs("error in command\n",stderr);
						exit(-1);
                    }
                    less_found = 1;
					index++;
					if(tokens[index] == NULL) {
						fputs("error in command\n",stderr);
						exit(-1);
					}
					else{
						char* file_in = tokens[index];
						in = open(file_in, O_RDONLY , S_IRUSR | S_IWUSR | S_IXUSR);
						if(in < 0){
							fputs("file not found\n",stderr);
							exit(-1);
						}
						dup2(in,0);
					}
				}
				else if(!strcmp(tokens[index],">")){
                    if(greater_found == 1 || greater_greater_found ==1 ){
                        fputs("error in command\n",stderr);
						exit(-1);
                    }
                    greater_found = 1;
					index++;
					if(tokens[index] == NULL) {
						fputs("error in command\n",stderr);
						exit(-1);
					}
					else{
						char* file_out = tokens[index];
						out = open(file_out, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
						if(out < 0){
							fputs("could not open file for writing\n",stderr);
							exit(-1);
						}
						dup2(out,1);
					}
				}
				else if(!strcmp(tokens[index],">>")){
                    if(greater_found == 1 || greater_greater_found ==1 ){
                        fputs("error in command\n",stderr);
						exit(-1);
                    }
                    greater_greater_found = 1;
					index++;
					if(tokens[index] == NULL) {
						fputs("error in command\n",stderr);
						exit(-1);
					}
					else{
						char* file_out = tokens[index];
						out = open(file_out, O_CREAT | O_APPEND | O_RDWR , S_IRUSR | S_IWUSR | S_IXUSR);
						if(out < 0){
							fputs("could not open file for writing\n",stderr);
							exit(-1);
						}
						dup2(out,1);
					}
				}
				else if(!strcmp(tokens[index],"|")){
                    pipe_found= 1;
                    index_of_pipe = index;
                    //printf("pipe found\n");
                }
			}
			for (index = 0; tokens[index] != NULL; index++){
				if(!strcmp(tokens[index],">>") || !strcmp(tokens[index],"<") || !strcmp(tokens[index],">") || !strcmp(tokens[index],"&")){
					int j;
					for(j = index; tokens[j] != NULL; j++){
						free(tokens[j]);
						tokens[j] = NULL;
					}
					break;
				}
			}
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
							if(DEBUG)
								printf("command to execute: %s",command);
							fflush(stdout);
							char** tokens1 = tokenize(command);
							int ret = execute_command(tokens1);
							if(ret == -1){
								//fprintf(stderr,"error in executing command: %s\n", command);
								//fflush(stderr);
							}
							else {
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

                int returnOfExecvp = 0;
                if(pipe_found){
                    int fd[2];
                    pipe(fd); //fd[0] for stdin, fd1 for stdout
                    int pid = fork();
                    if(pid==0){
                        dup2(fd[1], 1);
                        close(fd[0]);
                        int i;
                        for(i = index_of_pipe; tokens[i]!=NULL; i++){
                            free(tokens[i]);
                            tokens[i]= NULL;
                        }
                        returnOfExecvp = execvp(tokens[0],tokens);

                    }
                    else{
                        //close(0);
                        dup2(fd[0], 0);
                        close(fd[1]);
                        returnOfExecvp = execvp(tokens[index_of_pipe+1], tokens + index_of_pipe + 1);
                        int status;
                        //printf("parent%d\n",getpid());
                        waitpid(pid,&status,0);
                        if(WIFEXITED(status) && WEXITSTATUS(status) != 0)
                            returnOfExecvp = -1;
                    }
                }//pipe code end
				else{
                    returnOfExecvp = execvp(tokens[0],tokens);
				}
				if(returnOfExecvp != -1)	{
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
			if(in > 0){
				close(in);
			}
			if(out > 0){
				close(out);
			}
		}
		else {
			/* Parent Process */
			/* Wait for child process to complete */
			int status;
			//run_in_background = 0;
			if(!run_in_background){
				waitpid(pid,&status,0);
				if(WIFEXITED(status) && WEXITSTATUS(status) == 0)
					return 0;
				else
					return -1;	
			} else {
				int i;
				for(i = 0; i < 100; i++){
					if(background_processes[i] == -1 && back_count < 100){
						background_processes[i] = pid;
						back_count++;
						break;
					}
				}
			}
			return 0;
		}
	}
	return -1 ;
}
