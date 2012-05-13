#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	"varlib.h"
/* execute2.c - includes environmment handling */

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
{
	extern char **environ;
	int	pid ;
	int	child_info = -1;
	int	bk = 0;
	
	if ( argv[0] == NULL )		/* nothing succeeds	*/
		return 0;
	
	if (*(argv[0]+strlen(argv[0])-1)=='&')
	{
		printf("%s\n",argv[0]);		
		bk = 1;
		signal(SIGCHLD,Vchildquit);		
		*(argv[0]+strlen(argv[0])-1)='\0';
		
	}
	if ( (pid = fork())  == -1 )
		perror("fork");
	else if ( pid == 0 ){
		environ = VLtable2environ();
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}
	else {
        if(Vretifdebug())
		    printf("the pid of the cild is %d\n",pid);  //test
		Vinsertchild(argv[0],pid);
		if(bk==0)
		{
		if ( (pid=wait(&child_info)) == -1 )
			perror("wait");
		Vdeleteprotb(pid);
		}
		bk=0;
	}
	return child_info;
}
