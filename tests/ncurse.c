#include <ncurses.h>
#include <stdio.h>
#include <string.h>

int main() {
    int row, col;
    char message[]  = "This is a test of ncurses. Hi!";

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, row,col);
    mvprintw(row/2, (col-strlen(message))/2, "%s", message);
    mvprintw(row-2, 0, "This screen has %d rows and %d cols\n", row, col);
    
    printw("Try it again after resizing!\n");
    refresh();
    char c = getch();
    attron(A_BOLD);
    printw(" Key pressed was: %c\n", c);
    attroff(A_BOLD);
    getch();
    endwin();

    return 0;
}
