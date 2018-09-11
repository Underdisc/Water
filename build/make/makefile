#=COLOUR========================================================================

#-TEXT--------------------------------------------------------------------------
RT=@tput setaf 1
GT=@tput setaf 2
YT=@tput setaf 3
BT=@tput setaf 4
PT=@tput setaf 5
CT=@tput setaf 6
WT=@tput setaf 7

#-BACKGROUND--------------------------------------------------------------------
RB=@tput setab 1
GB=@tput setab 2
YB=@tput setab 3
BB=@tput setab 4
PB=@tput setab 5
CB=@tput setab 6
WB=@tput setab 7

#-STYLE-------------------------------------------------------------------------
BOLD=@tput bold
UNDERLINE=@tput smul

#-RESET-------------------------------------------------------------------------
RESET=@tput sgr0

#=MACROS========================================================================

CC = g++
CFLAGS = -c -O -std=c++11 -I../../src/ext/inc
CPEDANTIC = -Wall -Wextra -ansi -pedantic -Wunused -Werror
LFLAGS = -lglut32 -lglu32 -lopengl32 

SRCDIR = ../../src/
EXTDIR = ../../src/ext/

OBJS = Camera.o CameraController.o Complex.o Context.o Error.o FFT.o Framer.o GenericAction.o GraphicsTest.o main.o OpenGLContext.o OpenGLError.o Shader.o Time.o Water.o WaterFFT.o
EXTOBJS = json.o imgui.o imgui_demo.o imgui_draw.o imgui_impl_sdl_gl3.o
EXE = water.exe

#=TARGETS=======================================================================

$(EXE) : $(OBJS) $(EXTOBJS)
	$(GT)
	$(BOLD)
	$(CC) $(LFLAGS) $(OBJS) $(EXTERNALOBJS) -o $(EXE)
	$(RESET)

%.o : $(SRCDIR)%.cpp
	$(BT)
	$(BOLD)
	$(CC) $(CFLAGS) $< -o $@
	$(RESET)

%.o : $(EXTDIR)%.cpp
	$(BT)
	$(BOLD)
	$(CC) $(CFLAGS) $< -o $@
	$(RESET)

clean :
	$(RT)
	rm $(EXE) $(OBJS)
	$(RESET)
