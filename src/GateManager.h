#pragma once
#include "Gate.h"
#include "Map.h"
#include "Snake.h"
#include <vector>
#include <cstdlib>
#include <ncurses.h>
#include <chrono>
using namespace std::chrono;

class GateManager {
private:
    Gate gate1;
    Gate gate2;
    bool gateActive = false;
    int gateUseCount = 0; // 게이트 사용 횟수 카운트
    steady_clock::time_point lastGateChangeTime; 
    int lifetime = 15; // 게이트 수명(초): gate는 한 쌍으로 시간 관리
    int snakeLength = 5;  // 뱀 길이 Gate 생성 최소 기준

public:
    GateManager() {
        lastGateChangeTime = steady_clock::now();
    }

    void createGates(Map& map) {
        removeGates(map); // 기존 Gate 위치 Wall로 복원

        // Wall 위치 탐색
        std::vector<std::pair<int, int>> wallPositions;
        for (int y = 0; y < 21; ++y) {
            for (int x = 0; x < 39; ++x) {
                char ch = map.getChar(y, x);
                if (ch == '*') {
                    wallPositions.push_back({y, x});
                }
            }
        }

        if (wallPositions.size() < 2) return;

        // 무작위 두 위치 선택
        auto pos1 = wallPositions[rand() % wallPositions.size()];
        auto pos2 = wallPositions[rand() % wallPositions.size()];
        while (pos1 == pos2) {
            pos2 = wallPositions[rand() % wallPositions.size()];
        }

        gate1.setPosition(pos1.first, pos1.second);
        gate2.setPosition(pos2.first, pos2.second);

        // Gate 표시
        map.addChar(pos1.first, pos1.second, ACS_CKBOARD);
        map.addChar(pos2.first, pos2.second, ACS_CKBOARD);
        gateActive = true;
    }

    // Gate 제거, 최초 생성, 재생성 조건 포함
    void update(Map& map, int snakeSize, const Snake& snake) {
        auto now = steady_clock::now();

        // 뱀 머리 위치가 Gate 중 하나와 겹치는지 확인
        int headY = snake.head().getY();
        int headX = snake.head().getX();
        bool snakeInGate = (gateActive && (gate1.isAt(headY, headX) || gate2.isAt(headY, headX)));
        if (snakeInGate) return;

        // 뱀 길이가 snakeLength 미만이고 게이트가 있으면 Gate 제거
        if (gateActive && snakeSize < snakeLength) {
            removeGates(map);
            return;
        }

        // Gate가 아직 없고, 뱀 길이가 snakeLength 이상이면 최초 생성
        if (!gateActive && snakeSize >= snakeLength) {
            createGates(map);
            lastGateChangeTime = now;
            return;
        }

        // Gate가 없으면 종료
        if (!gateActive) return;

        // 일정 시간 지났으면 Gate 재생성
        auto elapsed = duration_cast<seconds>(now - lastGateChangeTime).count();
        if (elapsed >= lifetime) {
            createGates(map);
            lastGateChangeTime = now;
        }
    }

    void removeGates(Map& map) {
        if (!gateActive) return;
        map.addChar(gate1.getY(), gate1.getX(), '*'); // Wall 복원
        map.addChar(gate2.getY(), gate2.getX(), '*');
        gateActive = false;
    }

    bool isGate(int y, int x) const {
        return gate1.isAt(y, x) || gate2.isAt(y, x);
    }

    std::pair<Gate, Gate> getGatePair(int y, int x) const {
        if (gate1.isAt(y, x)) return {gate1, gate2};
        if (gate2.isAt(y, x)) return {gate2, gate1};
        return {Gate(), Gate()};  // 기본 생성자 있어야 함
    }

    bool isActive() const {
        return gateActive;
    }

    void incrementGateUse(Map& map) {
        gateUseCount++;
        map.incrementGate();  // Map에 게이트 사용 횟수 전달
    }

    int getGateUseCount() const {
        return gateUseCount;
    }
};

