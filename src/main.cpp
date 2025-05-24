#include <ncurses.h>
#include "SnakeGame.h"
#include <chrono> // 시간설정
using namespace std::chrono;

int main(void){
    initscr();
    refresh();
    noecho(); // 입력된 키 미표시
    curs_set(0);  // 커서 미표시
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);  // 키패드 활성화 추가

    SnakeGame game(21, 39);  // 높이 21, 너비 39로 게임 화면 설정
    game.run(); // 메인 게임 루프

    getch();
    endwin();

    return 0;
}