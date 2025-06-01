#pragma once
#include <vector>
#include <optional> 
#include <cstdlib>
#include "Item.h"
#include "Map.h"
#include "Snake.h"

class ItemManager { 
private: 
    std::vector<Item> items; // 아이템 저장 벡터
    int maxItemCount = 5; // 동시 출현 가능 아이템 수
    int lifetime = 20; // 아이템 수명(초): item은 하나씩 시간 관리
    int growthCount = 0;
    int poisonCount = 0;
    int speedUpCount = 0;  // SPEED_UP 카운트

public:
    void update(Map& map, const Snake& snake) {

        // 수명 지난 아이템 제거
        for (auto it = items.begin(); it != items.end();) {
            if (it->isExpired(lifetime)) {
                map.addChar(it->getY(), it->getX(), ' ');
                it = items.erase(it);
            } else {
                ++it;
            }
        }

        // 아이템 부족하면 생성
        while (items.size() < maxItemCount) {
            // 랜덤 위치
            int y = std::rand() % 21;
            int x = std::rand() % 39;

            if (map.getChar(y,x) != ' ' || snake.isOccupying(y,x)) continue;  // 뱀 위치 중복 검사 추가[9][17] // 빈 공간에만 생성
            
        
            ItemType types[] = {GROWTH, GROWTH, GROWTH, POISON, POISON, SPEED_UP};
            ItemType type = types[rand() % 6];  // 3종류 랜덤 생성

            items.emplace_back(y, x, type);  // 벡터에 아이템 추가
            map.addChar(y, x, items.back().getSymbol()); // 맵에 아이템 추가
        }
    }   
    

    // 뱀이 아이템을 획득했는지 확인
    std::optional<ItemType> checkCollision(const SnakePiece& head, Map& map) {
      for (auto it = items.begin(); it != items.end(); ++it) {
          if (it->getY() == head.getY() && it->getX() == head.getX()) {
              ItemType type = it->getType();
              if (type == GROWTH) {
                  ++growthCount;
                  map.incrementGrowth();  // Map 클래스에 카운트 전달
              } else if (type == POISON) {
                  ++poisonCount;
                  map.incrementPoison();  // Map 클래스에 카운트 전달
              }
              items.erase(it);
              return type;
          }
      }
      return std::nullopt;
    }

    int getGrowthCount() const {
        return growthCount;
    }

    int getPoisonCount() const {
        return poisonCount;
    }

    int getSpeedUpCount() const {
        return speedUpCount;
    }

    void clear() {
        items.clear();
    }

};