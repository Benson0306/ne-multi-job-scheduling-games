alpha-ne: alpha-ne.cc jsgame.cc
	g++ -pthread -g -o alpha-ne alpha-ne.cc jsgame.cc

clean:
	rm alpha-ne