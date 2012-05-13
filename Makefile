#
#
# Type  make    to compile all the programs
#


shell: smsh.c splitline.c execute2.c process2.c controlflow.c \
		builtin.c varlib.c 
	cc -o shell smsh.c splitline.c execute2.c process2.c controlflow.c \
		builtin.c varlib.c 

