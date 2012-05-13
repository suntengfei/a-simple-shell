/* builtin.c
 * contains the switch and the functions for builtin commands
 */

#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	"smsh.h"
#include	"varlib.h"

int assign(char *);
int okname(char *);

int builtin_command(char **args, int *resultp)
/*
 * purpose: run a builtin command 
 * returns: 1 if args[0] is builtin, 0 if not
 * details: test args[0] against all known builtins.  Call functions
 */
{
	int rv = 0;

	if ( strcmp(args[0],"set") == 0 ){	     /* 'set' command? */
		VLlist();
		*resultp = 0;
		rv = 1;
	}
	else if ( strchr(args[0], '=') != NULL ){   /* assignment cmd */
		*resultp = assign(args[0]);
		if ( *resultp != -1 )		    /* x-y=123 not ok */
			rv = 1;
	}
	else if ( strcmp(args[0], "export") == 0 ){
		if ( args[1] != NULL && okname(args[1]) )
			*resultp = VLexport(args[1]);
		else
			*resultp = 1;
		rv = 1;
	}
	else if( strcmp(args[0],"echo")==0){
		if(args[1]!=NULL)
			*resultp = Vecho(args[1]);
		else
			*resultp = 1;
		rv = 1;
			
	}
	else if( strcmp(args[0],"help")==0){
        printf("这是一个简单的shell：\n");
        printf("仅支持一些基本的命令和外部命令~\n");
        printf("在以后还需要不断完善~\n");
        *resultp = 0;
        rv = 1;
	}
    else if(strcmp(args[0],"jobs")==0){
        *resultp = Vlistpro();
        rv = 1;
    }
    else if(strcmp(args[0],"quit")==0){
        printf("谢谢使用，再见\n");
        exit(0);
    }
    else if(strcmp(args[0],"cd")==0)
    {
	if(args[1]!=NULL)
	    *resultp = Vcd(args[1]);
	else
	    *resultp = 1;
	rv = 1;
    }
	return rv;
}

int assign(char *str)
/*
 * purpose: execute name=val AND ensure that name is legal
 * returns: -1 for illegal lval, or result of VLstore 
 * warning: modifies the string, but retores it to normal
 */
{
	char	*cp;
	int	rv ;

	cp = strchr(str,'=');
	*cp = '\0';
	rv = ( okname(str) ? VLstore(str,cp+1) : -1 );
	*cp = '=';
	return rv;
}
int okname(char *str)
/*
 * purpose: determines if a string is a legal variable name
 * returns: 0 for no, 1 for yes
 */
{
	char	*cp;

	for(cp = str; *cp; cp++ ){
		if ( (isdigit(*cp) && cp==str) || !(isalnum(*cp) || *cp=='_' ))
			return 0;
	}
	return ( cp != str );	/* no empty strings, either */
}
