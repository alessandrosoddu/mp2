CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = read_audio
SRCS = mp2_first.cpp

# Directory di installazione di TwoLAME (modifica secondo la tua configurazione)
TWOLAME_INCLUDE_DIR = /usr/include
COMMON_INCLUDE_DIR = /home/alex/Documenti/mp2/lib/twolame-0.4.0/libtwolame
TWOLAME_LIB_DIR = /usr/lib

# Librerie da collegare
LIBS = -ltwolame

# Regola per la compilazione
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -Wall -I$(TWOLAME_INCLUDE_DIR) -I$(COMMON_INCLUDE_DIR) -L$(TWOLAME_LIB_DIR) -o $(TARGET) $(SRCS) $(LIBS) -ltwolame

# Regola per pulire i file generati
clean:
	rm -f $(TARGET) *.o

# Regola per eseguire il programma
run: $(TARGET)
	./$(TARGET)
