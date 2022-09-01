LIB = -L/usr/local/lib/ -lGLEW -lglfw -framework OpenGL
UNI_HEADER = -I/usr/local/include/
LOCAL_HEADER = -I./include/
ALL_HEADERS = ./include/shader.hpp ./include/Game.hpp ./include/Inputs.hpp ./include/Util.hpp ./include/Entity.hpp

ALL: main
	./main

main: main.o Game.o shader.o Entity.o Util.o
	g++ shader.o main.o Game.o Entity.o Util.o -o main $(LIB)

main.o: main.cpp $(ALL_HEADERS)
	g++ -c main.cpp $(UNI_HEADER) $(LOCAL_HEADER)

shader.o: shader.cpp $(ALL_HEADERS)
	g++ -c shader.cpp $(UNI_HEADER) $(LOCAL_HEADER)

Game.o: Game.cpp $(ALL_HEADERS)
	g++ -c Game.cpp $(UNI_HEADER) $(LOCAL_HEADER)

Entity.o: Entity.cpp $(ALL_HEADERS)
	g++ -c Entity.cpp $(UNI_HEADER) $(LOCAL_HEADER)

Util.o: Util.cpp $(ALL_HEADERS)
	g++ -c Util.cpp $(UNI_HEADER) $(LOCAL_HEADER)

test: test.o
	g++ test.o -o test $(LIB)

.PHONY:
clean:
	rm main main.o *.o