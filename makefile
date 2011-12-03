CXXFLAGS:= `wx-config --cxxflags`
#LDFLAGS:=`wx-config --libs`
#CXXFLAGS:=`wx-config --cxxflags --debug=yes`
LDFLAGS:=`wx-config --libs`
CONF:=debug release
SRC:=my_popstation.cpp gameData.cpp popstrip.cpp popstation.cpp popstationGUI.cpp config.cpp
EXEFILE:=MyPopstation



OBJ := $(addsuffix .o, $(basename $(SRC)))
OBJDIR := $(addprefix obj/, $(OBJ))
#HEADER := $(addsuffix .h, $(basename $(SRC)))
vpath %.cpp MyPopstation
vpath %.h MyPopstation

all: $(EXEFILE)

$(EXEFILE): $(OBJDIR)
	g++ -o $@ $^  -lz $(LDFLAGS) 
obj/my_popstation.o: my_popstation.cpp popstationGUI.h
	test -d obj || mkdir obj
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstationGUI.o: popstationGUI.cpp popstationGUI.h gameData.h header.h config.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstation.o: popstation.cpp popstation.h data.h popstrip.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/popstrip.o: popstrip.cpp popstrip.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/gameData.o: gameData.cpp gameData.h header.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)
obj/config.o: config.cpp config.h header.h
	g++ -g -Wall -o $@ -c $< $(CXXFLAGS)

.PHONY : clean
clean: 
	rm -f $(OBJDIR)

