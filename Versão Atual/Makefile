.SUFFIXES: .c .cc .cpp .cxx .h

# compiler names:
CXX		= g++ 
CC		= gcc

# flags for C++ compiler:
CFLAGS		= -g 
CXXFLAGS	= -g

# libraries to link with:

INC_PATH		=	
LIB_PATH 		=	-L/usr/lib/nvidia-375/ -L/usr/lib/x86_64-linux-gnu/

GL_LDFLAGS 		=	-lGL -lGLU -lGLEW
GLFW_LDFLAGS 	=	-lglfw
GLUT_LDFLAGS 	=	-lglut
ASSIMP_LDFLAGS 	=	-lassimp
SOIL_LDFLAGS	= 	-lSOIL

OBJFILES 		=	controls.o LoadShaders.o  

# compile rules
.c.o:	$*.h
	@echo "Compiling C code ...."
	$(CC) -o $*.o -c $(CXXFLAGS) $(INC_PATH) $*.c

.cpp.o:	$*.h
	@echo "Compiling C++ code ...."
	$(CXX) -o $*.o -c $(CXXFLAGS) $(INC_PATH) $*.cpp

# ***********************************************************************************
all:	main

main:	main.o $(OBJFILES)
	@echo "Linking ...."
	g++ -o $@ $^ $(LIB_PATH) $(GL_LDFLAGS) $(GLFW_LDFLAGS)
			
clean:	
	@echo "Clearing ..."
	rm -f *.o core *.*~ *~ main
