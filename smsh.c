#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	"smsh.h"
#include	"varlib.h"

/**
 **	small-shell version 4
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#define	DFL_PROMPT	"> "

int main(int ac,char *av[])
{
	char	*cmdline, *prompt, **arglist;
	int	result, process(char **);
	void	setup();
	
	FILE 	*fp;
	prompt = DFL_PROMPT ;
	setup();
	
	int ii = 1;   //test
	
	if(ac == 2)
	{
        if(strcmp(av[1],"-v")==0)
        {
            Vopendebug();
        }
        else if((fp = fopen(av[1],"r"))!=NULL)
		{
			while ( (cmdline = next_cmd(prompt, fp)) != NULL ){
				if ( (arglist = splitline(cmdline)) != NULL  ){
					if(arglist[0]==NULL)
					{
						fclose(fp);
						return 0;
					}
					//printf("      %d: %s\n",ii++,arglist[0]);
					result = process(arglist);
					printf("\n");
					freelist(arglist);
					fflush(fp);
				}
			free(cmdline);
			}
			fclose(fp);
			return 0;
		}
		else
		{
			perror("open file");
			exit(1);
		}
	}	
	
	while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
		if ( (arglist = splitline(cmdline)) != NULL  ){
			result = process(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}
	return 0;
}

void setup()
/*
 * purpose: initialize shell
 * returns: nothing. calls fatal() if trouble
 */
{
	extern char **environ;
	Vinitchildpro();
	VLenviron2table(environ);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
