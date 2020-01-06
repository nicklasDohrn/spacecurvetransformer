main.out: main.o objectReader.o curveReader.o transformer.o writer.o approx.o timer.o lightReader.o
	g++ -o main.out -O3 main.o objectReader.o curveReader.o transformer.o writer.o approx.o timer.o lightReader.o -pthread

prof: main.o objectReader.o curveReader.o transformer.o timer.o
	g++ -o prof.out -O3 main.o objectReader.o curveReader.o transformer.o writer.o approx.o timer.o -g -pg -pthread

curveReader.o: curveReader.cpp
	g++ -c curveReader.cpp


objectReader.o: objectReader.cpp
	g++ -c objectReader.cpp

lightReader.o: lightReader.cpp
	g++ -c lightReader.cpp

main.o: main.cpp
	g++ -c main.cpp

transformer.o: transformer.cpp
	g++ -c transformer.cpp

writer.o: writer.cpp
	g++ -c writer.cpp

approx.o: approx.cpp
	g++ -c approx.cpp


timer.o: timer.cpp
	g++ -c timer.cpp


clean:
	rm *.o
	rm *.obj
	rm *.cmp
