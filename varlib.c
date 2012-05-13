/* varlib.c
 *
 * a simple storage system to store name=value pairs
 * with facility to mark items as part of the environment
 *
 * interface:
 *     VLstore( name, value )    returns 1 for 0k, 0 for no
 *     VLlookup( name )          returns string or NULL if not there
 *     VLlist()			 prints out current table
 *
 * environment-related functions
 *     VLexport( name )		 adds name to list of env vars
 *     VLtable2environ()	 copy from table to environ
 *     VLenviron2table()         copy from environ to table
 *
 * details:
 *	the table is stored as an array of structs that
 *	contain a flag for `global' and a single string of
 *	the form name=value.  This allows EZ addition to the
 *	environment.  It makes searching pretty easy, as
 *	long as you search for "name=" 
 *
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	"varlib.h"
#include	<string.h>
#include	<signal.h>

#include        <unistd.h>

#define	MAXVARS	200		/* a linked list would be nicer */
#define MAXCHILD 10
#define MAXPLEN 100 

struct var {
		char *str;		/* name=val string	*/
		int  global;		/* a boolean		*/
	};

static struct var tab[MAXVARS];			/* the table	*/

static struct var childpro[MAXCHILD+1];           /*  the child process talbe  */

static char *new_string( char *, char *);	/* private methods	*/
static struct var *find_item(char *, int);

static char path[MAXPLEN];

static int debug = 0;

int VLstore( char *name, char *val )
/*
 * traverse list, if found, replace it, else add at end
 * since there is no delete, a blank one is a free one
 * return 1 if trouble, 0 if ok (like a command)
 */
{
	struct var *itemp;
	char	*s;
	int	rv = 1;

	/* find spot to put it              and make new string */
	if ((itemp=find_item(name,1))!=NULL && (s=new_string(name,val))!=NULL) 
	{
		if ( itemp->str )		/* has a val?	*/
			free(itemp->str);	/* y: remove it	*/
		itemp->str = s;
		rv = 0;				/* ok! */
	}
	return rv;
}

char * new_string( char *name, char *val )
/*
 * returns new string of form name=value or NULL on error
 */
{
	char	*retval;

	retval = malloc( strlen(name) + strlen(val) + 2 );
	if ( retval != NULL )
		sprintf(retval, "%s=%s", name, val );
	return retval;
}

char * VLlookup( char *name )
/*
 * returns value of var or empty string if not there
 */
{
	struct var *itemp;

	if ( (itemp = find_item(name,0)) != NULL )
		return itemp->str + 1 + strlen(name);
	return "";

}

int VLexport( char *name )
/*
 * marks a var for export, adds it if not there
 * returns 1 for no, 0 for ok
 */
{
	struct var *itemp;
	int	rv = 1;

	if ( (itemp = find_item(name,0)) != NULL ){
		itemp->global = 1;
		rv = 0;
	}
	else if ( VLstore(name, "") == 1 )
		rv = VLexport(name);
	return rv;
}

static struct var * find_item( char *name , int first_blank )
/*
 * searches table for an item
 * returns ptr to struct or NULL if not found
 * OR if (first_blank) then ptr to first blank one
 */
{
	int	i;
	int	len = strlen(name);
	char	*s;

	for( i = 0 ; i<MAXVARS && tab[i].str != NULL ; i++ )
	{
		s = tab[i].str;
		if ( strncmp(s,name,len) == 0 && s[len] == '=' ){
			return &tab[i];
		}
	}
	if ( i < MAXVARS && first_blank )
		return &tab[i];
	return NULL;
}


void VLlist()
/*
 * performs the shell's  `set'  command
 * Lists the contents of the variable table, marking each
 * exported variable with the symbol  '*' 
 */
{
	int	i;
	for(i = 0 ; i<MAXVARS && tab[i].str != NULL ; i++ )
	{
		if ( tab[i].global )
			printf("  * %s\n", tab[i].str);
		else
			printf("    %s\n", tab[i].str);
	}
}

int Vecho(char *name)
{
	struct var *itemp;
	char *this;
	this = name; 	
	if(*this=='$')
	{
		this++;
		if ( (itemp = find_item(this,0)) != NULL )
			printf("%s\n",itemp->str+1+strlen(this));
		else		
			printf("\n");
	}
	else
	printf("%s\n",name);
}

int Vinsertchild(char *name,int pid)
{
	char * newstring;
	int i;
	for(i=0;i<MAXCHILD;i++)
	{
		if(childpro[i].global==0)
		{
			newstring = malloc(1+strlen(name));			
			if ( newstring == NULL )
				return 0;
			strcpy(newstring, name);			
			childpro[i].str = newstring;
			childpro[i].global = pid;
			return 1;
		}
	}
	return 0;	
}

void Vchildquit(int m)
{
	int child_info;		
	int pid ;
	if((pid = wait(&child_info))!=-1){
		Vdeleteprotb(pid);
	}
}

void Vdeleteprotb(int pid)
{
	int i,j;
	for(i=0;i<MAXCHILD;i++)
	{
		if(pid==childpro[i].global)
		{
            if(debug)
              printf("the child %s pid is %d is exit\n",childpro[i].str,childpro[i].global);
			childpro[i].str=NULL;
			childpro[i].global = 0;
			for(j=i+1;j<MAXCHILD;j++)
				if(childpro[j].global!=0)
				{
					childpro[j-1].str = childpro[j].str;
					childpro[j-1].global = childpro[j].global;
					childpro[j].str = NULL;
					childpro[j].global = 0;
				}
		}
	}
}

int Vlistpro()
{
	int i;
	for(i=0;childpro[i].global!=0;i++)
	{
		printf("%s    %d\n",childpro[i].str,childpro[i].global);
	}
	return 0;
}

int Vinitchildpro()
{
	int i;
	for(i=0;i<MAXCHILD;i++)
	{
		childpro[i].str = NULL;
		childpro[i].global = 0;
	}
	return 0;	
}

int VLenviron2table(char *env[])
/*
 * initialize the variable table by loading array of strings
 * return 1 for ok, 0 for not ok
 */
{
	int     i;
	char	*newstring;

	for(i = 0 ; env[i] != NULL ; i++ )
	{
		if ( i == MAXVARS )
			return 0;
		newstring = malloc(1+strlen(env[i]));
		if ( newstring == NULL )
			return 0;
		strcpy(newstring, env[i]);
		tab[i].str = newstring;
		tab[i].global = 1;
	}
	while( i < MAXVARS ){		/* I know we don't need this	*/
		tab[i].str = NULL ;	/* static globals are nulled	*/
		tab[i++].global = 0;	/* by default			*/
	}
	return 1;
}

char ** VLtable2environ()
/*
 * build an array of pointers suitable for making a new environment
 * note, you need to free() this when done to avoid memory leaks
 */
{
	int	i,			/* index			*/
		j,			/* another index		*/
		n = 0;			/* counter			*/
	char	**envtab;		/* array of pointers		*/

	/*
	 * first, count the number of global variables
	 */

	for( i = 0 ; i<MAXVARS && tab[i].str != NULL ; i++ )
		if ( tab[i].global == 1 )
			n++;

	/* then, allocate space for that many variables	*/
	envtab = (char **) malloc( (n+1) * sizeof(char *) );
	if ( envtab == NULL )
		return NULL;

	/* then, load the array with pointers		*/
	for(i = 0, j = 0 ; i<MAXVARS && tab[i].str != NULL ; i++ )
		if ( tab[i].global == 1 )
			envtab[j++] = tab[i].str;
	envtab[j] = NULL;
	return envtab;
}

int Vcd(char *dir)
{
	int i;	
	i = chdir(dir);
	if(i!=0)
	{	
		printf("cd error\n");
		return 1;	
	}
	strcpy(path, dir);
	path[strlen(dir)+1] = '\0';
	return 0;
}

char *Vgetcurpath()
{
	char *a;
	a = getcwd(path,MAXPLEN);
	return a;
}

void Vopendebug()
{
    debug = 1;
}

int Vretifdebug()
{
    return debug;
}












