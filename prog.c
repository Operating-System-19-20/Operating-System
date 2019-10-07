#include <stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include <string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>

#define maxline 80
char * substr(char *s,int start,int end)
{
	char *d=malloc(10);
	int index=0;
	for(int i=start;i<=end;i++)
	{
		d[index]=s[i];
		index++;
	}
	d[index]='\0';
	return d;
}
char **split(char *s,char *delim)
{
	void* data;
	char *_s=(char*) s;
	char **ptrs;
	unsigned int psize,nword=1,slen=strlen(s),delen=strlen(delim);
	while((_s=strstr(_s,delim))){
	_s+=delen;
	++nword;
	}
	psize=(nword+1)*sizeof(char*);
	ptrs=data=malloc(psize+slen+1);
	if(data){
	*ptrs=_s=strcpy(((char*) data) +psize,s);
	if(nword>1){
	while((_s=strstr(_s,delim))){
	*_s='\0';
	_s+=delen;
	*++ptrs=_s;
	}}
	*++ptrs=NULL;
	}
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
	while(run)
	{
	
		
		printf("osh>");
		fflush(stdout);
		
		gets(temp);
		
		
		args=split(temp," ");
		if(args==NULL)
			continue;
		if(strstr(temp,"!!"))
			cp=0;
		else
			cp=1;
		int u=0;
		int t=1;
		while(args[u+1]!=NULL)
		{
				
			u++;
		}
		if(strcmp(args[u],"&")==0)
		
		{
			t=0;
			printf("not wait");
			args[u]=NULL;
		}
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
			if (strcmp(args[0],"!!")==0)
			{
				if(his==0)
				{
					printf("No command in history\n");
					exit(1);
				}
				else
				{
					
					free(args);
					args=split(temp1," ");
				}
			}
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



