CXXFLAGS = -g -m64 -Wall --std=c++11
COMPILER = /grid/common/pkgs/gcc/v6.3.0/bin/g++
COMPILER = C:\MinGW\bin\g++
EXE = tasks.exe
DELETE = rm
DELETE = del

$(EXE) : main.o Task.o Task.h
	$(COMPILER)  $(CXXFLAGS) -o $@ $< Task.o -static-libstdc++

main.o : main.cpp
	$(COMPILER) $(CXXFLAGS) -o $@ -c $<

Task.o : Task.cpp Task.h
	$(COMPILER) $(CXXFLAGS) -o $@ -c $<

clean:
	$(DELETE) $(EXE) main.o Task.o

test:
	./$(EXE)



