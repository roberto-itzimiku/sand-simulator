#######################################################################################
#MACROS
#######################################################################################

# $(1): compiler
# $(2): file generate
# $(3): source file
# $(4): additinal dependencies
# $(5): flags
define COMPILE 
$(2): $(3) $(4)
	$(1) -c -o $(2) $(3) $(INCLUDE_PATHS) $(5)
endef

# $(1): source file
define C2O
$(patsubst %.c,%.o,$(patsubst src/%,obj/%,$(patsubst %.cpp,%.o,$(1))))
endef

# $(1): source file
define C2H
$(patsubst %.c,%.h,$(patsubst %.cpp,%.hpp,$(1)))
endef

#######################################################################################
#CONFIG
#######################################################################################
APP   := game
CC    := g++
C     := gcc
MKDIR := mkdir -p
OBJ   := obj
SRC   := src

SUBDIRS    := $(shell findgnu src -type d) 
OBJSUBDIRS := $(patsubst src%,obj%,$(SUBDIRS))

CFILES           := $(shell findgnu src/ -type f -iname *.c)
OBJFILESC        := $(patsubst $(SRC)%,$(OBJ)%,$(patsubst %.c,%.o,$(CFILES)))
CPPFILES         := $(shell findgnu src/ -type f -iname *.cpp)
OBJFILESCPP      := $(patsubst $(SRC)%,$(OBJ)%,$(patsubst %.cpp,%.o,$(CPPFILES)))

INCLUDE_PATHS    := -I raylib/include/ -I src
LIBRARY_PATHS    := -L raylib/lib

COMPILER_CCFLAGS := -std=c++17 -Wno-missing-braces
COMPILER_CFLAGS  := -std=c99   -Wno-missing-braces
LINKER_CFLAGS    := -lraylib -lopengl32 -lgdi32 -lwinmm

ifdef DEBUG
	COMPILER_CCFLAGS += -g
	COMPILER_CFLAGS += -g
else
	COMPILER_CCFLAGS += -O3
	COMPILER_CFLAGS += -O3
endif

#######################################################################################
#DEPENDENCIES
#######################################################################################
$(APP): $(OBJSUBDIRS) $(OBJFILESC) $(OBJFILESCPP)
	$(C) -o $(APP) $(OBJFILESC) $(OBJFILESCPP) $(LIBRARY_PATHS) $(LINKER_CFLAGS)

$(OBJSUBDIRS): $(SUBDIRS)
	mkdir -p $(OBJSUBDIRS)

#MAKE RULES FOR C AND C++
$(foreach F,$(CPPFILES),$(eval $(call COMPILE,$(CC),$(call C2O,$(F)),$(F),$(call C2H,$(F)),$(COMPILER_CCFLAGS))))
$(foreach F,$(CFILES),$(eval $(call COMPILE,$(C),$(call C2O,$(F)),$(F),$(call C2H,$(F)),$(COMPILER_CFLAGS))))

info:
	$(info $(OBJFILESCPP))

clean:
	rm -f -r obj/ $(APP)