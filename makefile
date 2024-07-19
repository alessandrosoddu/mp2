CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = read_audio
SRCS = mp2_first.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o

run: $(TARGET)
	./$(TARGET)
