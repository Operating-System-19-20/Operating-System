#include <stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include <string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>

#define maxline 80


char * substr(char *src, int start, int end){
    // it is similar function new in C++
	char *dest = malloc(10);

	int count = 0;
	// Copy dest string from src string
	for(int i=start; i<=end; i++){
		dest[count]=src[i];
		count++;
	}

    // Add stop word at last string
	dest[count]='\0';
	return dest;
}

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

int main(void)
{
	char **args;
	char temp[100];
	char temp1[100];
	int run=1;
	int cp=1;
	int his=0;
	int t = 1;
	while(run)
	{


		printf("osh>");
		fflush(stdout);

		gets(temp);


		args=split(temp," ");
		if(args==NULL)
			continue;
		if (strcmp(args[0], "!!") == 0)
		{
			cp = 0;
			if (his == 0)
			{
				printf("No command in history\n");
				exit(1);
			}
			else
			{

				free(args);
				args = split(temp1, " ");
			}
		}
		else
			cp=1;
		int u=0;

		while(args[u+1]!=NULL)
		{

			u++;
		}
		if (strcmp(args[u], "&") == 0)

		{
			t = 0;
			args[u] = NULL;
		}
		else
			t = 1;
		if(strcmp(args[0],"exit")==0)
		{
			run=0;

		}
else
{
		pid_t id=fork();
		if(id==0)
		{
			int i=0;
			int k=0;

			while(args[i]!=NULL)
			{

				if(strcmp(args[i],">")==0)
				{
					int file_desc=open(args[i+1],O_WRONLY|O_CREAT);
					dup2(file_desc,1);
					while(args[i]!=NULL)
					{
						args[i]=NULL;
						i++;
					}
					break;
				}
				else
				if(strcmp(args[i],"<")==0)
				{

					strcpy(args[i],args[i+1]);
					args[i+1]=NULL;
					break;
				}
				else
				if(strcmp(args[i],"|")==0)
				{
					int p1[2];
					int p2[2];
					pipe(p1);
					pipe(p2);
					int r=fork();
					if(r==0)
					{
						execvp(args[i+1],args);
						dup2(p1[1],1);
						close(p1[1]);
						close(p1[0]);
						dup2(p2[0],0);

					}
					else
					{
						while(args[i]!=NULL)
						{
							args[i]=NULL;
							i++;
						}
						execvp(args[0],args);
						dup2(p2[1],1);
						close(p2[0]);
						close(p2[1]);
						dup2(p1[0],0);
					}
					exit(1);
				}
				i++;
			}



			execvp(args[0],args);

			exit(1);

		}
		else
		{
			if(t==1)
			{

				wait(NULL);

			}


		}
	}
	if(cp==1)
	{
		strcpy(temp1,temp);
		his=1;
	}
	free (args);

	}

	return 0;
}



