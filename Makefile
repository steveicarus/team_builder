

CXXFLAGS = -g

O = main.o

team_builder: $O
	$(CXX) -o team_builder $O
