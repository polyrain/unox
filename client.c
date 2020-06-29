#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
#define HALF_EMPTY "      "
#define NUM_CENTER "      %d     "
#define CARD_EDGE "+------------+"
#define CARD_HEIGHT 13
#define CARD_WIDTH 14
#define CHAT_PROMPT "Type your message: "
#define CHAT_LOG "%d:%d:%d %s"
#define MESSAGE_SIZE 80


void add_to_hand(Player* player, Card* card);
void remove_from_hand(Player* player, int cardNum);
void print_game(Game* game, int row, int col);
void play_card(Game* game, Player* player, int clientNum);
// TODO: Make magic numbers be reactive to terminal size 
/* Enum to describe the four colors, and border color, used by card
 * rendering */
enum CardColors {
    BLUE = 1,
    BORDER = 2,
    RED = 3,
    YELLOW = 4,
    GREEN = 5
};

enum CardTypes {
    SKIP = 'S',
    WILD = 'W',
    DRAW = 'F',
    REVERSE = 'R',
    NUMBER = 'N',
    TAKE = 'T'
};

enum InputFlags {
    QUIT = 0,
    CHAT = 1,
    SELECT = -1
};


int main() {
    setup_ncurse();

    Game* newGame = (Game*) malloc(sizeof(Game));
    //setup_game();
    output_display(newGame);
    refresh();
    endwin();

    return 0;
}

/* 
 * All backend ncurse content setup; 
 * Initialize screen, take raw input,
 * establish color pairings for use by drawing functions
 */
void setup_ncurse(void) {
    initscr();
    raw();
    noecho();
    cbreak();
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
 * Initialize a new game. Allocate memory for players, chat
 * and a set of cards. 
 */
void init_game(Game* game) {
    for (int i = 0; i < CHAT_LOG_COUNT; i++) {
        game->chatLog[i] = (Message*) malloc(sizeof(Message)); // 30 char chat
        game->chatLog[i]->content = (char*) malloc(sizeof(char) * MESSAGE_SIZE);
    }

    game->numMessages = 0;

    // Malloc players here, then malloc the array that holds them 
    // As it's a VLA of one dimension, but it contains pointers
    // as it's elements, which hold onto player data
    // i.e -> game.players[0] -> player* -> <player info here>
    // while c99 supports VLA's, not backwards compatible
}


// Test method to construct a hand and player 
void test_game(Game* game) {
    init_game(game);
    Player* newPlayer = (Player*) malloc(sizeof(Player));
    Card newCard1, newCard2, newCard3, newCard4, newCard5;
    Card* cards = (Card*) malloc(sizeof(Card) * 5);
    newCard1.color = BLUE;
    newCard1.cardValue = 9;
    newCard1.cardType = 'N';
    newCard2.color = RED;
    newCard2.cardType = 'R';
    newCard3.cardType = 'F';
    newCard2.cardValue = 0;
    newCard4.color = GREEN;
    newCard4.cardType = 'S';
    newCard5.cardType = 'W';
    newCard4.cardValue = 0;
    newCard3.cardValue = 0;
    newCard5.cardValue = 0;
    cards[0] = newCard1;
    cards[1] = newCard2;
    cards[3] = newCard3;
    cards[2] = newCard4;
    cards[4] = newCard5;
    game->lastPlayed = (Card*) malloc(sizeof(Card));
    newPlayer->currentCard = 0;
    game->lastPlayer = -1;
    newPlayer->hand = cards;
    newPlayer->handSize = 5;
    newPlayer->numCards = 5; // 1 index'd
    game->clientNum = 0;
    game->players = (Player**) malloc(sizeof(Player*) * 2);
    game->players[0] = newPlayer;
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard relative to the size received.
 * */
void output_display(Game* game) {
    int row, col; // Size of screen in terms of rows and columns
    getmaxyx(stdscr, row,col); // Grab screen size to try and adapt drawing
    /* Test card until Game functionality begins */
    test_game(game);
    int key = 0;

    Card testCard;
    testCard.color = RED;
    testCard.cardValue = 4;
    testCard.cardType = 'N';

    print_game(game, row, col);
    Player* player = game->players[game->clientNum];
    while(1) { // TEST INTERACTION
        key = getch(); // What did they press?
        int status = handle_input(key, game);
        if (status == QUIT) { // Swap this to be switch
            return; 
        } else if (status == CHAT) {
            handle_chat(game, row, col);
        } else if (status == 'P') {
            add_to_hand(player, &testCard);
        } else if (status == 'x') {
            remove_from_hand(player, player->numCards - 1);
        } else if (status == 'n') {
            play_card(game, player, game->clientNum);
            remove_from_hand(game->players[game->clientNum], game->players[game->clientNum]->currentCard);
        }
        print_game(game, row, col);
        refresh(); // Keep drawing
    }
}

// Generate whole uno deck then rand indices to it
Card* generate_card(void) {
    // TODO: Make cards be pageable; min size = 1 card width + chat
    
}

// Performs a deep copy of a card to the game
void play_card(Game* game, Player* player, int clientNum) {
    Card* card = &(player->hand[player->currentCard]);
    game->lastPlayed->cardType = card->cardType;
    game->lastPlayed->cardValue = card->cardValue;
    game->lastPlayed->color = card->color;
    game->lastPlayer = clientNum;
}

/* Prints the current state of the game to the screen; to avoid flicker,
 * individual prints call cltoeol to only wipe the places changing */
void print_game(Game* game, int row, int col) {
    print_hand(game->players[game->clientNum], row - CARD_HEIGHT - 2, 0);
    print_chat_buffer(game, row, col);

    int midRow, midCol; // Find the middle of the screen, and print the last
    getmaxyx(stdscr, midRow, midCol);

    if (game->lastPlayer != -1) { // If we havent seen a move yet, no last player
        print_card(game->lastPlayed, 
                (midRow / 2) - CARD_HEIGHT / 2, 
                (midCol / 2)- CARD_WIDTH / 2, 1);
    }
}

/* Adds a card to a players hand. Checks if theres enough memory; if not,
 * doubles memory space for that player. Adds new card to hand and returns.
 */
void add_to_hand(Player* player, Card* card) {
 
    if (player->numCards >= player->handSize) { // Are we at the mem limit?
        Card* hand = player->hand; // Grab ptr out, not needed but clarity
        hand = (Card*) realloc(hand, sizeof(Card) * ((player->handSize) * 2));
        player->handSize *= 2; // Double hand size every time (amort to O(1))
        player->hand = hand;
    }

    player->hand[player->numCards++] = (*card);
}

/* Removes a card from a players hand and updates the array */
void remove_from_hand(Player* player, int cardNum) {

    if (cardNum > player->numCards - 1 || cardNum < 0) {
        fprintf(stderr, "Invalid card num to remove!\n");
        return;
    }
    
    if (cardNum == player->numCards - 1) {
        player->numCards--;
        player->currentCard--;
    } else { // Will be overwritten next time you add
        for (int i = cardNum; i < player->numCards - 1; i++) {
            player->hand[i] = player->hand[i + 1];
        }
        player->numCards--;
    }

}

/* Handles the chat log in game. If max messages reached, pop the earliest off*/
void handle_chat(Game* game, int row, int col) {
    
    if (++(game->numMessages) > CHAT_LOG_COUNT) { // Check we haven't filled
        game->numMessages = CHAT_LOG_COUNT; // the bank
         
        for (int i = 0; i < CHAT_LOG_COUNT - 1; i++) {
            strcpy(game->chatLog[i]->content, game->chatLog[i + 1]->content); // 5 goes to 4 and so on
        } // If so shift it all down one
        strcpy(game->chatLog[CHAT_LOG_COUNT - 1]->content, "");
    }

    mvprintw(row - 2, (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE, CHAT_PROMPT); // Display prompt
    capture_message(game); // -2 stops overflow on screen size, without it touches card
    move(row - 2, (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE); // Move cursor bacl
    clrtoeol(); // Wipe the prompt

    
}

/* Grabs a message from player via stdin, stores in the game */
void capture_message(Game* game) {
    Message* newMessage = game->chatLog[game->numMessages - 1];
    echo();
    getstr(newMessage->content); // Get input
    noecho();
    time_t rawTime; // Get the sys time
    time(&rawTime);
    struct tm* tm_struct = localtime(&rawTime); // localtime is static
    newMessage->hour = tm_struct->tm_hour; // so grab the current state of it
    newMessage->min = tm_struct->tm_min; // for later use
    newMessage->sec = tm_struct->tm_sec; 
}

/* Prints the messages stored in game to terminal */
void print_chat_buffer(Game* game, int row, int col) {
   
    int spacing = 3; // Used for spacing messages; should only be 1 char
    for (int i = CHAT_LOG_COUNT - 1; i >= 0; i--) {
        if (strlen(game->chatLog[i]->content) > 0) {
            move((row - spacing), (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE);
            clrtoeol(); // Above wipes old line off screen
            mvprintw((row - spacing), (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE, 
                CHAT_LOG, game->chatLog[i]->hour,
                game->chatLog[i]->min, 
                game->chatLog[i]->sec, game->chatLog[i]->content);
            spacing++; // Print message to screen and incr spacing
        } 
    }

}

int handle_input(int key, Game* game) {
    Player* player = game->players[game->clientNum];
    switch (key) {
        case KEY_LEFT:
            if (player->currentCard != 0) {
                player->currentCard--;
            }
            break;
        case KEY_RIGHT:
            if (player->currentCard != player->numCards - 1) {
                player->currentCard++;
            }
            break;
        case 'q':
        case 'Q':
            return QUIT;
        case 't':
        case 'T':
            return CHAT;
        case 'p':
        case 'P':
            return 'P';
        case 'x':
            return 'x';
        case '\n':
            return 'n';
        default:
            return SELECT;


    }
    return SELECT;
}


/*  Prints the clients hand to the screen */ 
void print_hand(Player* player, int row, int col) {
    
    // Above will increment forward in array by specified amount always
    // Where amount is 5 cards per page. 
    
    for (int i = 0; i < player->numCards; i++) {
        if (i == player->currentCard) { // Bold current selections
            print_card(&(player->hand[i]), row, col, 1);   
        } else {
            print_card(&(player->hand[i]), row, col, 0);
        }
        col += CARD_WIDTH + 4; // Jump over card we drew + 4 char gap TODO fix magic make reactive
    }

    for (int i = 0; i < CARD_HEIGHT; i++) {
        move(row++, col);
        clrtoeol();
    };

}


/* 
 * Helper function to draw top and bottom edge of cards, to ensure
 * a consistent border around all card edges. Updates current row
 * through pointer passed to it
 */
void print_card_edge(int* row, int col) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, CARD_EDGE);
    attroff(COLOR_PAIR(BORDER)); // Draw the border on edge
}



/*
 * Prints empty space on cards, which represent the top and bottom 4 
 * spaces of the cards size. Used by card drawing functions to 
 * fill the card space with relevant coloring
 */
void print_empty_space(Card* card, int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER)); // Turn on the color of the border
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(card->color));
        printw(EMPTY);
        attroff(COLOR_PAIR(card->color));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}


/* Method to print the text line of a card. Reverse,
 * skip and take two use this to print the relevant words
 * on their cards, int he correct space */
void print_card_text(Card* card, int* row, int col) {
   
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->color));

    switch (card->cardType) {
        case REVERSE:
            printw(REVERSE_TEXT);
            break;
        case SKIP:
            printw(SKIP_TEXT);
            break;
        case TAKE:
            printw(TAKE_TWO_TEXT);
            break;
        case NUMBER:
            printw(NUM_CENTER, card->cardValue);
            break;  
        
        default:
            fprintf(stderr, "Invalid card type detected at text\n");
    }

    attroff(COLOR_PAIR(card->color));
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

    attron(COLOR_PAIR(BORDER)); // Top colors, now draw special interior
    mvprintw((*row)++, col, "|"); // Text differs from WILD and DRAW4
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(RED));

    switch (card->cardType) {
        case DRAW:
            printw("   DRA");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("W4    ");
            attroff(COLOR_PAIR(BLUE));
            break;
        case WILD:
            printw("    WI");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("LD    ");
            attroff(COLOR_PAIR(BLUE));
            break;

        default:
            fprintf(stderr, "Invalid wild card found!");
    }

    attron(COLOR_PAIR(BORDER));
    printw("|"); // Close up this one line, and draw rest of bottom
    attroff(COLOR_PAIR(BORDER));

    print_bottom_wild(row, col);
}


/* Method to print the top two rainbow parts of a rainbow card */
void print_top_wild(int* row, int col) {
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(RED));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(RED));
        attron(COLOR_PAIR(YELLOW));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(YELLOW));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}

/* Method to print the bottom two rainbow parts of a rainbow card */
void print_bottom_wild(int* row, int col) {
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(GREEN));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(GREEN));
        attron(COLOR_PAIR(BLUE));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(BLUE));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}


/* Prints characters in top left or bottom right corner of card
 * depending on what value the "top" flag is set to. 
 */
void print_card_detail(Card* card, int* row, int col, int top) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->color));
    switch (card->cardType) {
        case NUMBER:
            if (top) {
                printw(NUMBER_TOP, card->cardValue);
            } else {
                printw(NUMBER_BOTTOM, card->cardValue);
            }
            break;
        case TAKE:
            if (top) {
                printw(TAKE_TWO_TOP); 
            } else {
                printw(TAKE_TWO_BOTTOM);
            }
            break;
        
        case SKIP:
            if (top) {
                printw(SKIP_TOP);
            } else {
                printw(SKIP_BOTTOM);
            }
            break;
            
        case REVERSE:
            if (top) {
                printw(REVERSE_TOP);
            } else {
                printw(REVERSE_BOTTOM);
            }
            break;
        default:
            // error, shouldn't be reached
            fprintf(stderr, "Invalid card detected in detail ");
            fprintf(stderr, "Card info: %c %d\n\n\n\n", card->cardType, card->cardValue);
    }

    attroff(COLOR_PAIR(card->color));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
}

/*
 * Takes a card pointer and prints off the relevant card at the designated
 * position on the screen.
 */
void print_card(Card* card, int row, int col, int bold) {
    if(bold) {
        attron(A_BOLD);
    }

    print_card_edge(&row, col);
    
    if (card->cardType == DRAW || card->cardType == WILD) {
        print_wild_card(card, &row, col);
    } else {
        print_card_detail(card, &row, col, 1);
        print_empty_space(card, &row, col);
        print_card_text(card, &row, col);
        print_empty_space(card, &row, col);
        print_card_detail(card, &row, col, 0);
    }

    print_card_edge(&row, col);
    
    if (bold) {
        attroff(A_BOLD);
    }
    
    refresh();
}
