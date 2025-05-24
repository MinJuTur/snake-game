#include <ncurses.h>
#pragma once
#include "Map.h"
#include "Snake.h"
#include "ItemManager.h"
#include "Item.h"
#include <chrono>
using namespace std::chrono;

class SnakeGame{
    Map map;
    Snake snake;
    ItemManager itemManager;
    bool game_over = false;
    steady_clock::time_point last_move;
public:
    SnakeGame(int height, int width) : map(height, width), snake() {
        map = Map(height, width);
        initialize(0);  // 초기시작 0단계
    }

    // 메인 게임 루프
    void run() {
        redraw();
        while (!checkOver()) {
            userInput();
            if (isTimeToMove()) {
                last_move = steady_clock::now();
                updateGame();
            }
            redraw();
        }
    }

    void initialize(int stagenum){
        map.initialize(stagenum);
        snake.initialize();
        map.addChar(1, 4, '%');  // snack 초기 위치 설정
        map.addChar(1, 3, '#');
        map.addChar(1, 2, '#');
        map.addChar(1, 1, '#');
        last_move = steady_clock::now();  // 초기화 당시 시간 측정
    }

    // 게임오버 체크
    bool checkOver()
    {
        return game_over;
    }

    // 사용자 입력 처리
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
        return duration_cast<milliseconds>(now - last_move).count() >= 1000;
    }

    // 게임 상태 업데이트
    void updateGame() { 
        SnakePiece next = snake.nextHead();
        auto item = itemManager.checkCollision(next);

        if (item) {
            handleItemCollision(next, *item);
        } else {
            moveSnakeToNext(next);
        }
        itemManager.update(map, snake);
    }

    // 아이템 충돌 처리
    void handleItemCollision(const SnakePiece& next, ItemType itemType) {
        if (itemType == ItemType::GROWTH) {
            snake.head().setIcon('#');
            map.addChar(snake.head().getY(), snake.head().getX(), '#');

            snake.addPiece(next);
            map.addChar(next.getY(), next.getX(), '%');
        } else if (itemType == ItemType::POISON) {
            if (snake.getSize() <= 3) {
                game_over = true;
                return;
            }
            map.addChar(snake.tail().getY(), snake.tail().getX(), ' ');
            snake.removePiece();

            snake.head().setIcon('#');
            map.addChar(snake.head().getY(), snake.head().getX(), '#');

            snake.addPiece(next);
            map.addChar(next.getY(), next.getX(), '%');
        }
    }

     // 일반 이동 처리
    void moveSnakeToNext(SnakePiece next){
        int nextRow = next.getY();
        int nextCol = next.getX();

        if (map.getChar(nextRow, nextCol) == ' '){
            // snake의 꼬리 위치에 있는 icon을 ' '로 설정
            map.addChar(snake.tail().getY(), snake.tail().getX(), ' ');
            // snake의 꼬리 부분을 없앤다
            snake.removePiece();

            // snake의 머리 부분의 icon을 '#'로 설정
            snake.head().setIcon('#');
            // 게임 창에도 똑같이 반영
            map.addChar(snake.head().getY(), snake.head().getX(), '#');

            // snake의 머리 부분을 next로 설정
            snake.addPiece(next);
            // snake의 머리 부분의 icon을 '%'로 설정
            snake.head().setIcon('%');
            // 게임 창에도 똑같이 반영
            map.addChar(snake.head().getY(), snake.head().getX(), '%');
        }
    }

    // 스테이지에 따라 맵 설정
    void setStage(int stageNum){
        map.initialize(stageNum);
    }

    // 스테이지 반환
    int getStage(){
        return map.getStageNum();
    }

    // 게임 새로고침
    void redraw(){   
        map.updateScore(snake.getSize());
        map.refresh();
    }
};