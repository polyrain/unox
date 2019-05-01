#include <stdio.h>


int main() {
    /* Clears the screen*/
    printf("\033[2J");
    printf("\033[H");
    /*Draws a card in the middle*/
    printf("\x1b[31m+----------+\n");
    printf("|");
    printf("\x1b[7m|\n");
    printf("|          |\n");
    printf("|          |\n");
    printf("|  ");
    printf("\x1b[0m DRAW");
    printf("\x1b[31m   |\n");
    printf("|  ");
    printf("\x1b[0m FOUR");
    printf("\x1b[3`m   |\n");
    printf("|          |\n");
    printf("|          |\n");
    printf("|          |\n");
    printf("|          |\n");
    printf("+----------+\n");
    printf("\x1b[0m");
    return 0;
}

void draw_card(char* cardType, int color) {

    











