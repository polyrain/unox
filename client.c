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


/* Enum to describe the four colors, and border color, used by card
 * rendering */
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
    getmaxyx(stdscr, row,col); // Grab screen size to try and adapt drawing
    /* Test card until Game functionality begins */
    Card newCard;
    newCard.colorOne = BLUE;
    newCard.cardValue = 7;
    newCard.cardType = 'T';
    int initialY = (row/2) - 10; // Place the start 6 offsert
    int initialX = (col/2) - 6; 
    print_card(&newCard, initialY, initialX);
}


/*
 * Outputs last played card on pile. Draws in centre of screen,
 * determined from values passed by value to function.
 * CARD DIMENSIONS: 15 characters wide, 10 characters tall
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

/* 
 * Helper function to draw top and bottom edge of cards, to ensure
 * a consistent border around all card edges. Updates current row
 * through pointer passed to it
 */
void print_card_edge(int* row, int col) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "+------------+");
    attroff(COLOR_PAIR(BORDER)); // Draw the border on edge

}


/*
 * Method to draw a number card. Takes a card pointer, an int pointer
 * represeting the current card y value to be drawn on, and the column.
 */
void print_number_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    print_empty_space(card, row, col); // Number cards have no inner text
    print_card_detail(card, row, col, 0);
}

/*
 * Prints empty space on cards, which represent the top and bottom 4 
 * spaces of the cards size. Used by card drawing functions to 
 * fill the card space with relevant coloring
 */
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

/*
 * Method to print a reverse card design on the screen. Takes
 * a card, an int pointer, and an integer to determine where
 * to draw, and what to draw color wise.
 */
void print_reverse_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    print_card_text(card, row, col);
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);
}

/* Method to print the text line of a card. Reverse,
 * skip and take two use this to print the relevant words
 * on their cards, int he correct space */
void print_card_text(Card* card, int* row, int col) {
   
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));

    switch (card->cardType) {
        case 'R':
            printw(REVERSE_TEXT);
            break;
        case 'S':
            printw(SKIP_TEXT);
            break;
        case 'T':
            printw(TAKE_TWO_TEXT);
            break;
        
        default:
            printf("AAAA");
    }

    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
}


/* Method for printing rainbow cards. These require a different
 * printing method due to the more intricate design process
 * that goes into them. Switches text based on if you have a wild
 * or a draw four card */
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


/* Method to print the top two rainbow parts of a rainbow card */
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

/* Method to print the bottom two rainbow parts of a rainbow card */
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

/* Method ot print a skip card, same as others */
void print_skip_card(Card* card, int* row, int col) {
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    print_card_text(card, row, col);
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);    
}

/* Method to print a take two card, same as others */
void print_take_two(Card* card, int* row, int col) { 
    print_card_detail(card, row, col, 1);
    print_empty_space(card, row, col);
    print_card_text(card, row, col);
    print_empty_space(card, row, col);
    print_card_detail(card, row, col, 0);
}

/* Prints characters in top left or bottom right corner of card
 * depending on what value the "top" flag is set to. 
 */
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

/*
 * Takes a card pointer and prints off the relevant card at the designated
 * position on the screen.
 */
void print_card(Card* card, int row, int col) {
    
    print_card_edge(&row, col);
    attron(A_BOLD);
    // SWITCH NOW BASED ON THE CARD
    switch (card->cardType) {
        case 'N':
            print_number_card(card, &row, col);
            break;

        case 'F':
        case 'W':
            print_wild_card(card, &row, col);
            break;
        case 'T':
            print_take_two(card, &row, col);
            break;
        case 'S':
            print_skip_card(card, &row, col);
            break;
        case 'R':
            print_reverse_card(card, &row, col);
            break;
        default:
            // Error out, free memory, kill client
            printf("Hello");
    }
    print_card_edge(&row, col);
}
