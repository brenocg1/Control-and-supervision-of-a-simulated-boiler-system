#Makefile Breno Campos
#Nome do projeto aqui, o executavel...
PROJ_NAME=controlemanual

# Compiler C
CC=gcc

#A funcao wildcard pega todos os arquivos com extensao especificada
#No caso as .c
#Na variável C_SOURCE o nome de cada arquivo estará separado um do outro por um espaço

# .c files
C_SOURCE=$(wildcard *.c)

# .h files
H_SOURCE=$(wildcard *.h)

# Object files
OBJ=$(C_SOURCE:.c=.o)

# Flags for compiler
CC_FLAGS=-c         \
         -W         \
         -Wall      \
	 	 -Wuninitialized \

# Libraries
# pthread é usado na fase de linkagem, e não compilação  
# -lm para math.h
LIBS=-pthread -lm

#
# Compilation and linking
#

#target ALL a ser executado
all: $(PROJ_NAME)


$(PROJ_NAME): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

# O símbolo ‘%’ pega o stem (tronco) do nome, que é utilizado de referência no pré-requisito.
 %.o: %.c %.h
	$(CC) -o $@ $< $(CC_FLAGS) 

main.o: main.c $(H_SOURCE)	
	$(CC) -o $@ $< $(CC_FLAGS) 

clean:
	rm -rf *.o $(PROJ_NAME) *~

rebuild:
	+@make clean
	+@make

run:
	./$(PROJ_NAME) localhost 4545





