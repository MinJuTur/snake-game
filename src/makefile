SRC = main.cpp
HEADERS = SnakeGame.h Snake.h Map.h Item.h ItemManager.h GateManager.h Gate.h WallManager.h
TARGET = main

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	g++ -o $(TARGET) $(SRC) -lncurses

clean:
	rm -f $(TARGET)