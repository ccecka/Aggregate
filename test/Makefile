
EXEC = $(basename $(wildcard *.cpp) $(wildcard *.cu))

#########################
## Library Directories ##
########################

INCLUDES += -I../include

####################
## Makefile Setup ##
####################

# Define the C++ compiler to use
CXX := $(shell which g++) -std=c++11

# Dependency directory and flags
DEPSDIR := $(shell mkdir -p .deps; echo .deps)
# MD: Dependency as side-effect of compilation
# MF: File for output
# MP: Include phony targets
DEPSFILE = $(DEPSDIR)/$(notdir $*.d)
DEPSFLAGS = -MD -MF $(DEPSFILE) #-MP

# Define any directories containing header files
#   To include directories use -Ipath/to/files
INCLUDES += -I.

# Define cxx compile flags
CXXFLAGS  = -funroll-loops -O3 -W -Wall -Wextra

# Define any directories containing libraries
#   To include directories use -Lpath/to/files
LDFLAGS +=

# Define any libraries to link into executable
#   To link in libraries (libXXX.so or libXXX.a) use -lXXX
LDLIBS  +=

######################
## Makefile Options ##
######################

####################
## Makefile Rules ##
####################

# Suffix replacement rules
#   $^: the name of the prereqs of the rule
#   $<: the name of the first prereq of the rule
#   $@: the name of the target of the rule
.SUFFIXES:               # Delete the default suffixes
.SUFFIXES: .hpp .cpp .o  # Define our suffix list

# 'make' - default rule
all: $(EXEC)

# Default rule for creating an exec of $(EXEC) from a .o file
$(EXEC): % : %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Default rule for creating a .o file from a .cpp file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEPSFLAGS) -c -o $@ $<

# 'make clean' - deletes all .o and temp files, exec, and dependency file
clean:
	-$(RM) *.o
	-$(RM) $(EXEC)
	$(RM) -r $(DEPSDIR)

# Define rules that do not actually generate the corresponding file
.PHONY: clean all

# Include the dependency files
-include $(wildcard $(DEPSDIR)/*.d)
