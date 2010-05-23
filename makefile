#CXXFLAGS:=`wx-config --cxxflags`
#LDFLAGS:=`wx-config --libs`
CXXFLAGS:=`wx-config --cxxflags --debug=yes`
LDFLAGS:=`wx-config --libs --debug`
CONF:=debug release
SRC:=my_popstation.cpp gameData.cpp popstrip.cpp popstation.cpp popstationGUI.cpp
EXEFILE:=MyPopstation



OBJ := $(addsuffix .o, $(basename $(SRC)))
OBJDIR := $(addprefix obj/, $(OBJ))
#HEADER := $(addsuffix .h, $(basename $(SRC)))
vpath %.o obj

all: $(EXEFILE)

$(EXEFILE): $(OBJDIR)
	g++ -o $@ $^  $(LDFLAGS)
obj/my_popstation.o: my_popstation.cpp popstationGUI.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstationGUI.o: popstationGUI.cpp popstationGUI.h gameData.h header.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstation.o: popstation.cpp popstation.h data.h popstrip.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstrip.o: popstrip.cpp popstrip.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/gameData.o: gameData.cpp gameData.h header.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)

clean: 
	rm -f $(OBJDIR)

