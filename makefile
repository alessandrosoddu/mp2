CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = read_audio
SRCS = main.cpp

# Directory di installazione di TwoLAME (modifica secondo la tua configurazione)
TWOLAME_INCLUDE_DIR = /usr/include
TWOLAME_LIB_DIR = /usr/lib

# Librerie da collegare
LIBS = -ltwolame

# Regola per la compilazione
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -I$(TWOLAME_INCLUDE_DIR) -L$(TWOLAME_LIB_DIR) -o $(TARGET) $(SRCS) $(LIBS)

# Regola per pulire i file generati
clean:
	rm -f $(TARGET) *.o

# Regola per eseguire il programma
run: $(TARGET)
	./$(TARGET)
