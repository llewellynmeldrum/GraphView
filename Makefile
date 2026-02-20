#--------------------------------------------------------------------------------------------------------
# my personal makefile, to do the following:
# 1. Compile all $(SRC_EXT) files with selected compiler, into separate object files, in $(BUILD_DIR)
# 2. Link all obj files we've just built, against eachother
# 3. include any libs through ldlibs, prefer using $(shell pkg-config <name>) pattern
# 4. Offer options to debug segfaults, using adsan, usan, etc
# Do all of the above, but also make it pretty with fancy tput headers lol
#--------------------------------------------------------------------------------------------------------
.PHONY: all clean cclean crun ccrun run debug help h ? docs 
N_THREADS:=4
ifeq ($(UNAME_S), Linux) #LINUX
	N_THREADS:= $(shell nproc)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	N_THREADS:= $(shell sysctl -n hw.logicalcpu)
endif

all: 
	$(MAKE) -j$(N_THREADS) run
##---------------------------------------------------------------------
## Internal
##---------------------------------------------------------------------

EXE_DIR		:=bin
EXE_NAME 	:=test
EXE 		:=$(EXE_DIR)/$(EXE_NAME)
SRC_EXT		:=.cpp
OBJ_EXT 	:=.o


SRC_DIR		:=src
OBJ_DIR 	:=build
INC_DIR 	:=include
EXT_DIR 	:=external

SRC 		:=$(wildcard $(SRC_DIR)/*$(SRC_EXT)) 
CXXFLAGS 	:=-std=c++2b -I$(INC_DIR) -Wall -Wformat -O0
LDFLAGS		:=
LDLIBS		:=

##---------------------------------------------------------------------
## External (pkg-cfg linking, /external stuff)
##---------------------------------------------------------------------
IMGUI_DIR	:=$(EXT_DIR)/imgui
IMGUI_GLFW	:=$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp 
IMGUI_OPENGL:=$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SRC 		+=$(wildcard $(IMGUI_DIR)/*$(SRC_EXT)) 
SRC 		+=$(IMGUI_GLFW) $(IMGUI_OPENGL)
CXXFLAGS	+=-I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends

#glad 
CXXFLAGS	+=-Iexternal/glad/include
SRC 		+=external/src/glad.cpp

##---------------------------------------------------------------------
## GLAD (static lib)
##---------------------------------------------------------------------
CC := gcc
CCFLAGS:=
GLAD_DIR     := $(EXT_DIR)/glad
GLAD_INC     := $(GLAD_DIR)/include
GLAD_SRC     := $(GLAD_DIR)/src/glad.c

GLAD_OBJ_DIR := $(OBJ_DIR)/external/src
GLAD_OBJ     := $(GLAD_OBJ_DIR)/glad.o

# Make sure glad headers are on the include path for your whole project
CXXFLAGS += -I$(GLAD_INC)
CCFLAGS   += -I$(GLAD_INC)


# Compile glad.c (C file -> use CC/CFLAGS, not CXX/CXXFLAGS)
$(GLAD_OBJ): $(GLAD_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

# Link with it
LDLIBS += $(GLAD_LIB)

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL
ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LDLIBS += $(LINUX_GL_LIBS) `pkg-config --libs glfw3`

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LDLIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LDLIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	#LIBS += -lglfw3
	LDLIBS += -lglfw

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "Chud OS detected, unsupported."
endif
##---------------------------------------------------------------------

##--<Should stay the same>---------------------------------------------
#OBJS := $(patsubst $(SRC_EXT),$(OBJ_DIR)/%$(OBJ_EXT),$(SRC))
OBJS := $(patsubst %$(SRC_EXT),$(OBJ_DIR)/%$(OBJ_EXT),$(SRC))\
        $(patsubst %.c,$(OBJ_DIR)/%.o,$(filter %.c,$(SRC)))
##--<-------------------->---------------------------------------------
$(OBJ_DIR)/%$(OBJ_EXT) : %$(SRC_EXT)
	@$(ECHO_COMP_BANNER)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

# build executable (linking obj -> bin)
$(EXE): $(EXE_DIR) $(OBJ_DIR) $(OBJS)
	@$(ECHO_LINK_BANNER)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# executing binary
run: $(EXE)
	@$(ECHO_EXE_BANNER)
	./$(EXE) $(ARGS)

#NOTE: AVOID PUTTING TARGETS IN CLEAN COMMAND! BE VERY SPECIFIC SO YOU DONT NUKE YOUR SOURCE FILES ON ACCIDENT
clean: 
	$(ECHO_CLEAN_BANNER)
	rm -rf build/src/* bin/* 
cclean: 
	$(ECHO_CLEAN_BANNER)
	rm -rf build/* bin/* 

crun: clean run
ccrun: cclean run

# ------------ DEBUGGING ------------ #
debug: $(EXE)
	lldb -o run -- $(EXE) $(TERM_COLS) $(ARGS)

	
# Address san: lower overhead than thread-san, cleaner stack traces,
asan: CFLAGS  += -fsanitize=address -fno-omit-frame-pointer 
asan: LDFLAGS += -fsanitize=address
asan: LDLIBS  += -fsanitize=address
asan: ASAN_ENV:= ASAN_OPTIONS=abort_on_error=1
asan: clean run 

usan: CFLAGS  += -fsanitize=undefined -fno-omit-frame-pointer -g
usan: LDFLAGS += -fsanitize=undefined 
usan: LDLIBS  += -fsanitize=undefined 
usan: clean run 


ausan: CFLAGS  += -fsanitize=address,undefined -fno-omit-frame-pointer -g
ausan: LDFLAGS += -fsanitize=address,undefined
ausan: LDLIBS  += -fsanitize=address,undefined
ausan: CFLAGS+= -fsanitize-address-use-after-scope
ausan: ASAN_ENV:= ASAN_OPTIONS=abort_on_error=1

ausan: CFLAGS  += -fsanitize=undefined -fno-omit-frame-pointer -g
ausan: LDFLAGS += -fsanitize=undefined 
ausan: LDLIBS  += -fsanitize=undefined 
ausan: clean run

tsan: CFLAGS  += -fsanitize=thread -fno-omit-frame-pointer 
tsan: LDFLAGS += -fsanitize=thread
tsan: LDLIBS  += -fsanitize=thread
tsan: clean run 

# ------------ EXTRAS ------------ #
$(OBJ_DIR):
	@mkdir -p $@

$(EXE_DIR):
	@mkdir -p $@

help h ? docs: HELP
HELP:
	
	@$(FMT_REV)
	@printf "===== MAKE VARIABLES =====\n"; $(FMT_RESET)
	@$(FMT_ALT1) 	printf "SRC      = $(SRC)\n" 
	@$(FMT_ALT2) 	printf "EXE_DIR  = $(EXE_DIR )\n"
	@$(FMT_ALT1)	printf "EXE_NAME = $(EXE_NAME)\n"
	@$(FMT_ALT2)	printf "EXE      = $(EXE)\n"
	@$(FMT_ALT1)	printf "SRC_DIR  = $(SRC_DIR)\n"
	@$(FMT_ALT2)	printf "OBJ_DIR  = $(OBJ_DIR)\n"
	@$(FMT_ALT1)	printf "SRC_EXT  = $(SRC_EXT)\n"
	@$(FMT_ALT2)	printf "OBJ_EXT  = $(OBJ_EXT)\n"
	@$(FMT_ALT1)	printf "CXX     = $(CXX)\n"
	@$(FMT_ALT2)	printf "SRC      = $(SRC)\n"
	@$(FMT_ALT1)	printf "OBJS     = $(OBJS)\n"
	@$(FMT_ALT2)	printf "CXXFLAGS  = $(CXXFLAGS)\n"
	@$(FMT_ALT1)	printf "LDFLAGS  = $(LDFLAGS)\n"
	@$(FMT_ALT2)	printf "LDLIBS   = $(LDLIBS)\n"
	@$(FMT_ALT1)	printf "ALLFLAGS = $(ALLFLAGS)\n"




# ------------ PRETTY PRINTING ------------ #
FMT_RESET	:=tput sgr0;
FMT_REDBANNER	:=tput rev; tput bold; tput setaf 1;
FMT_GREENBANNER	:=tput rev; tput bold; tput setaf 2;
FMT_YELLOWBANNER:=tput rev; tput bold; tput setaf 3;
FMT_REV		:=tput rev; tput bold;


FMT_ALT1	:= tput setaf 7;
FMT_ALT2	:= tput sgr0;


ECHO_LINK_BANNER := @$(FMT_YELLOWBANNER) 	printf " LINKING OBJ to BIN -> "; $(FMT_RESET) 	printf "\t"
ECHO_COMP_BANNER := @$(FMT_GREENBANNER)		printf " COMPILE SRC to OBJ -> "; $(FMT_RESET)	printf "\t"
ECHO_EXE_BANNER := @$(FMT_REDBANNER) 		printf " EXECUTING BINARY -> "; $(FMT_RESET)	printf "\t"
ECHO_CLEAN_BANNER:= @$(FMT_REV) 		printf " REMOVING EXEUTABLES AND OBJECT FILES... "; $(FMT_RESET)printf "\t"


# EXTRA MAKEFILE STUFF:
# 1-1: To exclude certain files from compilation:
# 	EXCLUDE_PAT	:= unit_tests.c
# 	SRC 		:= $(filter-out src/$(EXCLUDE_SRC), $(wildcard src/*.c))
