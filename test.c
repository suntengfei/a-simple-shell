#include<stdio.h>

int main(int ac,char *av[])
{
    FILE *fp;
    int c;
    if(ac!=2)
    {
        printf("av bushi 2\n");
        exit(1);
    }
    fp = fopen(av[1],"r");
    while((c = getc(fp))!= EOF)
    printf("%c",c);
}












