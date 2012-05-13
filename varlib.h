/*
 * header for varlib.c package
 */

int	VLenviron2table(char **);
int	VLexport(char *);
char	*VLlookup(char *);
void	VLlist();
int	VLstore( char *, char * );
char	**VLtable2environ();
int	VLenviron2table(char **);
int	Vinsertchild(char *,int);
void 	Vchildquit(int);
int	Vlistpro();
int 	Vecho(char *);
int	Vinitchildpro();
int 	Vcd(char *);
char    *Vgetcurpath();
void    Vdeleteprotb(int);
void    Vopendebug();
int     Vretifdebug();
