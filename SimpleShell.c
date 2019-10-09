#include <stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include <string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>

#define MAXLINE 80



unsigned int count_word(char* str_cpy, const char* seperator, unsigned int n_words)
{
	// Count number of word in this string
	unsigned int seperator_length = strlen(seperator);

	while ( ( str_cpy = strstr( str_cpy, seperator ) ) ) {
		// Search seperator in string return first position
		str_cpy += seperator_length;
		++n_words;
	}

	return n_words;
}

void word_token(void* data, const char** ptr_str, const char* str, char* str_cpy, unsigned int n_words, unsigned int line_size, const char* seperator, unsigned int seperator_length)
{
	if ( data ) {
		// Copy first word to data because ptr_str -> data
		*ptr_str = str_cpy = strcpy((( char* )data ) + line_size, str);

		// Line has more one word
		if ( n_words > 1 ) {

			while ( ( str_cpy = strstr( str_cpy, seperator ) ) ) {
				// Search seperator in string return first position
				*str_cpy = '\0';
				str_cpy += seperator_length;
				// Copy next word to data
				*++ptr_str = str_cpy;
			}

		// Add stop word at last string
		*++ptr_str = NULL;
		}
	}
}
char** split( const char* str, const char* seperator ) {
	void* data;
	char* str_cpy = ( char* )str;
	const char** ptr_str;
	unsigned int line_size, n_words = 1, length = strlen( str ), seperator_length = strlen( seperator );

	// Count number of word in this string
	n_words = count_word(str_cpy, seperator, n_words);

    // It is the same as line = new[n_word + 1] in C++
	line_size = ( n_words + 1 ) * sizeof( char* );
	ptr_str = data = malloc( line_size + length + 1 );

	// Word token means split line to one word or many words
	word_token(data, ptr_str, str, str_cpy, n_words, line_size, seperator, seperator_length);

	return data;
}

char ** substr(char **argv,int pos,int size)
{
  size_t strlen_sum;
  char **argp;
  char *data;
  size_t len;
  int i;

  strlen_sum = 0;
  for (i = pos ; i < pos + size; i++) strlen_sum += strlen(argv[i]) + 1;

  argp = malloc(sizeof(char *) * (size + 1) + strlen_sum);
  if (!argp) return NULL;
  data = (char *) argp + sizeof(char *) * (size + 1);

  for (i = 0; i < size; i++) {
    argp[i] = data;
    len = strlen(argv[i+pos]) + 1;
    memcpy(data, argv[i+pos], len);
    data += len;
  }
  argp[size] = NULL;

  return argp;
}


int main(void)
{
	char **args;
	char cmd[MAXLINE];
	char previousCmd[MAXLINE];
	int history = 0;
	int concurrent = 1;
  int pipeline = 0;

  //No previous commandline
  previousCmd[0] = 0;

	while(1)
	{
		printf("osh>");
		fflush(stdout);

		gets(cmd);
    // get arguments from the commandline
		args = split(cmd," ");

    // Initialize there is no pipe communication
    pipeline = 0;

		if(args == NULL) continue;
        
    // History feature 
		if (strcmp(args[0], "!!") == 0)
		{
			history = 0;

      // No previous commandline
			if (previousCmd[0] == 0)
			{
        printf("No command in history\n");
				continue;
			}
			else
			{
        // if is founded, copy the previous arguments
				free(args);
				args = split(previousCmd, " ");
			}
		}
		else history = 1;

    // Get the length of args
		int len = 0;
		while(args[len+1] != NULL) len++;
	
    // Check if is concurrently running
		if (strcmp(args[len], "&") == 0)
		{
			concurrent = 0;
			args[len] = NULL;
		}
		else concurrent = 1;

    // Check user wants to exist
		if(strcmp(args[0],"exit") == 0) break;
        
      // Create a child processing
      pid_t id = fork();

      //Check if it is in child processing
      if(id == 0)
      {
        int i=0;
        while(args[i] != NULL)
        {
            // Redirecting ouput
            if(strcmp(args[i],">") == 0)
            {
                int file_desc = open(args[i+1],O_WRONLY|O_CREAT);
                dup2(file_desc,1);
                while(args[i] != NULL)
                {
                    args[i] = NULL;
                    i++;
                }
                break;
            }
            //Redirecting input
            if(strcmp(args[i],"<") == 0)
            {
                strcpy(args[i],args[i+1]);
                args[i+1]=NULL;
                break;
            }
            //Communication via pipe
            if(strcmp(args[i],"|") == 0)
            {
              pipeline = 1;
              int pipefd[2];
              int childpid;
              pipe(pipefd);
              if(childpid=fork()){
                //parent
                //Write
                while(args[i]!=NULL)
                {
                  args[i]=NULL;
                  i++;
                }
                close(pipefd[0]);
                dup2(pipefd[1],STDOUT_FILENO);
                execvp(args[0],args);
                wait(NULL);
                break;
              }
              else{  
                //child
                //Read
                char ** subCmd = substr(args,i+1,len-i);
                close(pipefd[1]);
                dup2(pipefd[0],STDIN_FILENO);
                execvp(subCmd[0],subCmd);
                free(subCmd);
                exit(1);
              }
            }

            i++;
        }

        // Execute cmd
        if(!pipeline) execvp(args[0],args);
        // End of child processing
        exit(1);
      }
      else if(concurrent == 1) wait(NULL);// if is in parent prcessing, chek if it is concurrent

      //if is not in history feature, save a copy of current cmd for future use
      if(history == 1) strcpy(previousCmd,cmd);
      free (args);
    }
  return 0;
}



