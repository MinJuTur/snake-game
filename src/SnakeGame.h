#include <ncurses.h>
#pragma once
#include "Map.h"
#include "Snake.h"
#include "ItemManager.h"
#include "Item.h"
#include "GateManager.h"
#include <chrono>
using namespace std::chrono;

class SnakeGame{
    Map map;
    Snake snake;
    ItemManager itemManager;
    GateManager gateManager;
    
    bool game_over = false;
    steady_clock::time_point last_move;
    int moveDelay = 300;
public:
    SnakeGame(int height, int width) : map(height, width), snake() {
        map.initialize(0);  // 초기시작 0단계
        snake.initialize();
        map.addChar(1, 4, '%');  // snack 초기 위치 설정
        map.addChar(1, 3, '#');
        map.addChar(1, 2, '#');
        map.addChar(1, 1, '#');
        last_move = steady_clock::now();  // 초기화 당시 시간 측정
    }

    // 메인 게임 루프
    void run() {
        redraw();
        while (!checkOver()) {
            userInput();
            if (isTimeToMove()) {
                last_move = steady_clock::now();

                // 다음 위치로 이동 및 처리
                moveSnakeToNext(snake.nextHead());   
                
                itemManager.update(map, snake);
                gateManager.update(map, snake.getSize(), snake);
                redraw();
            }    
        }
    }
        
    // 게임 새로고침
    void redraw(){   
        map.updateScore(snake.getSize());
        map.refresh();
    }

    // 게임오버 체크
    bool checkOver()
    {
        return game_over;
    }

    // 사용자 입력에 따라 진행방향 설정
    void userInput() { 
        int input = map.getInput();
        if (input == ERR) return;

        switch (input)
        {
        case 'w':
            if (snake.getDirection() == down){
                game_over = true;
            }
            snake.setDirection(up);
            break;
        case 's':
            if (snake.getDirection() == up){
                game_over = true;
            }
            snake.setDirection(down);
            break;
        case 'd':
            if (snake.getDirection() == left){
                game_over = true;
            }
            snake.setDirection(right);
            break;
        case 'a':
            if (snake.getDirection() == right){
                game_over = true;
            }
            snake.setDirection(left);
            break;
        }
    }

    // 일정 시간 경과 확인
    bool isTimeToMove() { 
        auto now = steady_clock::now();
        return duration_cast<milliseconds>(now - last_move).count() >= moveDelay;
    }

    void moveSnakeToNext(SnakePiece next){
        int nextRow = next.getY();
        int nextCol = next.getX();

        // Gate 충돌 여부 확인
        if (gateManager.isGate(nextRow, nextCol)) {
            gateManager.incrementGateUse();  // 게이트 사용 횟수 증가
            handleGateTeleport(next);
            return;
        }

        // 아이템 충돌 여부 확인
        auto item = itemManager.checkCollision(next);
        if (item) {  
            handleItemCollision(next, *item);
            return;
        } 
        
        // 벽 또는 몸통 충돌 체크
        if (map.getChar(nextRow, nextCol) != ' ') { 
            game_over = true;
            return;
        }

        // 평범한 이동

        // 꼬리 제거
        map.addChar(snake.tail().getY(), snake.tail().getX(), ' ');
        snake.removePiece();
        
        // 머리 추가
        snake.head().setIcon('#');
        map.addChar(snake.head().getY(), snake.head().getX(), '#');
        snake.addPiece(next);
        snake.head().setIcon('%');
        map.addChar(snake.head().getY(), snake.head().getX(), '%');
    }

    
    // 아이템 효과 처리
    void handleItemCollision(const SnakePiece& next, ItemType itemType) {
        
        // 머리 추가(공통)
        snake.head().setIcon('#');
        map.addChar(snake.head().getY(), snake.head().getX(), '#');
        snake.addPiece(next);
        snake.head().setIcon('%');
        map.addChar(snake.head().getY(), snake.head().getX(), '%');
        

        switch(itemType){
            case ItemType::GROWTH: // 아무 것도 안 함(꼬리 유지)
                break;

            case ItemType::POISON: // 꼬리 두 번 삭제
                for (int i = 0; i < 2; i++) {
                    map.addChar(snake.tail().getY(), snake.tail().getX(), ' ');
                    snake.removePiece();

                    if(snake.getSize() < 3){
                        game_over = true;
                        break;
                    }    
                }
                break;
        }
    }

    // gate 통과 처리
    void handleGateTeleport(const SnakePiece& entry) {
        auto [entryGate, exitGate] = gateManager.getGatePair(entry.getY(), entry.getX());
        
        // 현재 방향 기준으로 진출 방향 계산
        Direction inDir = snake.getDirection();
        Direction outDir = determineExitDirection(exitGate.getY(), exitGate.getX(), inDir);
        snake.setDirection(outDir);

        // 출구 Gate에서 나온 머리 생성
        SnakePiece exitHead = getNextFromGate(exitGate.getY(), exitGate.getX(), outDir);
        // 기존 머리는 몸통으로 변경
        snake.head().setIcon('#'); 
        map.addChar(snake.head().getY(), snake.head().getX(), '#');
        snake.addPiece(exitHead);
        snake.head().setIcon('%');
        map.addChar(snake.head().getY(), snake.head().getX(), '%');

        // 꼬리 제거(머리 생성했으므로)
        map.addChar(snake.tail().getY(), snake.tail().getX(), ' ');
        snake.removePiece();
    }

    // 진출 방향 계산
    Direction determineExitDirection(int y, int x, Direction inDir) {
        // 가장자리인 경우
        if (y == 0) return down;
        if (y == 20) return up;
        if (x == 0) return right;
        if (x == 38) return left;

        // 중앙이라면 시계방향 우선순위
        // Gate에서 나올 때 갈 수 있는 방향 후보
        std::vector<Direction> dirs = {
            inDir,
            (Direction)((inDir + 1) % 4),
            (Direction)((inDir + 3) % 4),
            (Direction)((inDir + 2) % 4)
        };

        // 가장 우선순위가 높은 안전한 방향을 찾아서 반환
        for (Direction d : dirs) {
            SnakePiece next = getNextFromGate(y, x, d);
            if (map.getChar(next.getY(), next.getX()) == ' ') {
                return d;
            }
        }

        return inDir;  // fallback
    }

    SnakePiece getNextFromGate(int y, int x, Direction dir) {
        switch (dir) {
            case up: return SnakePiece(y - 1, x, '%');
            case down: return SnakePiece(y + 1, x, '%');
            case left: return SnakePiece(y, x - 1, '%');
            case right: return SnakePiece(y, x + 1, '%');
        }
        return SnakePiece(y, x, '%');  // fallback
    }
};