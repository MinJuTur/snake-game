#pragma once
#include <vector>
#include <utility>
#include <random>
#include "Map.h"

class WallManager {
private:
    std::vector<std::pair<int,int>> walls;
    int spawnInterval = 17; // 기본 5초
    int timer = 0;
    std::uniform_int_distribution<int> y_dist{1, 19};
    std::uniform_int_distribution<int> x_dist{1, 37};
    std::mt19937 gen{std::random_device{}()};

    bool isSafePosition(int y, int x, const Snake& snake) {
        SnakePiece head = snake.head();
        return (abs(y-head.getY()) > 2 || abs(x-head.getX()) > 2);
    }

public:
    void setStage(int stage) { // 스테이지별 난이도 조절 메서드
        spawnInterval = 5000 - (stage * 500);
    }

    void update(Map& map, const Snake& snake, bool forceReset=false) {
      if (++timer < spawnInterval) return;
  
      int retry = 0;
      while (retry < 50) { // 50번까지 시도
          int y = y_dist(gen);
          int x = x_dist(gen);
  
          // 조건: 빈칸, 뱀 몸통 없음, 기존 벽과 겹치지 않음, 뱀 머리와 안전거리
          if (map.getChar(y, x) == ' ' &&
              !snake.isOccupying(y, x) &&
              isSafePosition(y, x, snake)) {
  
              bool exists = false;
              for (auto& [wy, wx] : walls) {
                  if (wy == y && wx == x) exists = true;
              }
              if (!exists) {
                  walls.emplace_back(y, x);
                  map.addChar(y, x, '#');
                  break;
              }
          }
          retry++;
      }
      map.refresh();
      timer = 0;
    }
};
