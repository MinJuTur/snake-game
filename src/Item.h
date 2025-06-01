#pragma once
#include <chrono>
using namespace std::chrono;

enum ItemType {
    GROWTH,
    POISON,
    SPEED_UP
};

class Item {
private:
    int row, col;
    ItemType type;
    steady_clock::time_point createdTime;
public:
    Item(int y, int x, ItemType t) : row(y), col(x), type(t), createdTime(std::chrono::steady_clock::now() ) {}

    int getY() const {
        return row;
    }

    int getX() const {
        return col;
    }

    ItemType getType() const { 
        return type; 
    }

    bool isExpired(int lifetime) const {
        return duration_cast<seconds>(steady_clock::now() - createdTime).count() >= lifetime;
    }

    char getSymbol() const {
      if (type == GROWTH) return '+';
      if (type == POISON) return '-';
      return 'S';  // 5번 규칙: SPEED_UP 심볼 'S' 지정
  }
};