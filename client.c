#include <ncurses.h>

#include <stdio.h>
#include <string.h>
#include "client.h"

#define EMPTY "            " // Define empty space
#define TAKE_TWO_TOP "[][]        " // Details for card edges
#define TAKE_TWO_BOTTOM "        [][]"
#define NUMBER_TOP "%d           "
#define NUMBER_BOTTOM "           %d"
#define REVERSE_TOP "<->         "
#define REVERSE_BOTTOM "         <->"
#define SKIP_TOP "<X>         "
#define SKIP_BOTTOM "         <X>"
#define REVERSE_TEXT "   REVERSE  "
#define SKIP_TEXT "    SKIP    "
#define TAKE_TWO_TEXT "  TAKE TWO  "



enum CardColors {
    BLUE = 1,
    BORDER = 2,
    RED = 3,
    YELLOW = 4,
    GREEN = 5
};

int main() {
    setup_ncurse();
    //setup_game();
    output_display(NULL);
    refresh();
    getch();
    endwin();

    return 0;
}

/* 
 * All backend ncurse content setup; 
 * Initialize screen, take raw input,
 * establish color pairings
 */
void setup_ncurse(void) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    // Setup color attrs 
    init_pair(BLUE, COLOR_WHITE, COLOR_BLUE); 
    init_pair(BORDER, COLOR_WHITE, COLOR_WHITE);
    init_pair(RED, COLOR_WHITE, COLOR_RED);
    init_pair(YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(GREEN, COLOR_WHITE, COLOR_GREEN);
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard*/
void output_display(Game* game) {
    int row, col; // Size of screen in terms of rows and columns
    getmaxyx(stdscr, row,col); // Move all these inits into another func
    // Grabbing the screensize
    //output_card(NULL, row, col); // We pass this due to consistency
   // output_hand(game->players[game->clientNum], row, col);
    Card newCard;
    newCard.colorOne = BLUE;
    newCard.cardValue = 7;
    newCard.cardType = 'S';
    print_card(&newCard, row, col);
    
}


/*
 * Outputs last played card on pile. Draws in centre of screen,
 * determined from values passed by value to function.
 * CARD DIMENSIONS: 13 characters wide, 19 characters tall
 */
void output_card(Card* lastPlayed, int row, int col) {
    int initialY = (row/2) - 10; // Place the start 6 offsert
    int initialX = (col/2) - 6; 

    attron(COLOR_PAIR(BORDER)); // Begin drawing
    mvprintw(initialX, initialY, "+-----------+");

    for (int i = 0; i < 5; i++) {
        mvprintw(initialY++, initialX, "+-----------+");
    }
    for (int i = 0; i < 5; i++) {
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(RED));
        mvprintw(initialY++, initialX, EMPTY);
    }

}
void print_card_edge(int* row, int col) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "+------------+");
    attroff(COLOR_PAIR(BORDER)); // Draw the border on edge

}

void print_number_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    print_empty_space(card, row, col); // Number cards have no inner text
    print_card_detail(card, row, col, 0);
}

void print_empty_space(Card* card, int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(card->colorOne));
        printw(EMPTY);
        attroff(COLOR_PAIR(card->colorOne));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}

void print_reverse_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));
    printw(REVERSE_TEXT);
    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);
}

void print_wild_card(Card* card, int* row, int col) {
    print_top_wild(row, col);
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(RED));

    switch (card->cardType) {
        case 'F':
            printw("   DRA");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("W4    ");
            attroff(COLOR_PAIR(BLUE));
            break;
        case 'W':
            printw("   WI");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("LD    ");
            attroff(COLOR_PAIR(BLUE));
            break;
    }

    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));

    print_bottom_wild(row, col);
}

void print_top_wild(int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(RED));
        printw("      ");
        attroff(COLOR_PAIR(RED));
        attron(COLOR_PAIR(YELLOW));
        printw("      ");
        attroff(COLOR_PAIR(YELLOW));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}

void print_bottom_wild(int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(GREEN));
        printw("      ");
        attroff(COLOR_PAIR(GREEN));
        attron(COLOR_PAIR(BLUE));
        printw("      ");
        attroff(COLOR_PAIR(BLUE));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}

void print_skip_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");  
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));
    printw(SKIP_TEXT);
    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);    
}

void print_take_two(Card* card, int* row, int col) { 
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));
    printw(TAKE_TWO_TEXT);
    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);
}

void print_card_detail(Card* card, int* row, int col, int top) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));
    switch (card->cardType) {
        case 'N':
            if (top) {
                printw(NUMBER_TOP, card->cardValue);
            } else {
                printw(NUMBER_BOTTOM, card->cardValue);
            }
            break;
        case 'T':
            if (top) {
                printw(TAKE_TWO_TOP); 
            } else {
                printw(TAKE_TWO_BOTTOM);
            }
            break;
        
        case 'S':
            if (top) {
                printw(SKIP_TOP);
            } else {
                printw(SKIP_BOTTOM);
            }
            break;
            
        case 'R':
            if (top) {
                printw(REVERSE_TOP);
            } else {
                printw(REVERSE_BOTTOM);
            }
            break;
        default:
            //error
            printf("AHH");
    }

    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
}

void print_card(Card* card, int row, int col) {
    int initialY = (row/2) - 10; // Place the start 6 offsert
    int initialX = (col/2) - 6; 
    print_card_edge(&initialY, initialX);
    attron(A_BOLD);
    // SWITCH NOW BASED ON THE CARD
    switch (card->cardType) {
        case 'N':
            print_number_card(card, &initialY, initialX);
            break;

        case 'F':
        case 'W':
            print_wild_card(card, &initialY, initialX);
            break;
        case 'T':
            print_take_two(card, &initialY, initialX);
            break;
        case 'S':
            print_skip_card(card, &initialY, initialX);
            break;
        case 'R':
            print_reverse_card(card, &initialY, initialX);
            break;
        default:
            // Error out, free memory, kill client
            printf("Hello");
    }
    print_card_edge(&initialY, initialX);
}
