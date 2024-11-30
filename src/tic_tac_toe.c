#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#define SIZE 3

// Global terminal settings
struct termios original_termios;

// Function prototypes
void setInputMode();
void restoreInputMode();
void signalHandler(int signo);
void initializeBoard(char board[SIZE][SIZE]);
void printBoard(char board[SIZE][SIZE]);
int checkWin(char board[SIZE][SIZE]);
int isDraw(char board[SIZE][SIZE]);
void makeMove(char board[SIZE][SIZE], int player);

int main() {
    char board[SIZE][SIZE];
    int player = 1; // Player 1 starts
    int running = 1;

    setInputMode(); // Set non-canonical input mode
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    initializeBoard(board);

    while (running) {
        system("clear");
        printf("Tic-Tac-Toe\n");
        printf("Player 1: X | Player 2: O\n");
        printf("Press 'q' to quit at any time.\n\n");
        printBoard(board);

        if (checkWin(board)) {
            printf("Player %d wins!\n", player == 1 ? 2 : 1);
            printf("\nGame Over. Thank you for playing!\n");
            sleep(1);
            break;
        } else if (isDraw(board)) {
            printf("It's a draw!\n");
            printf("\nGame Over. Thank you for playing!\n");
            sleep(1);
            break;
        }

        printf("Player %d's turn.\n", player);
        makeMove(board, player);

        player = (player == 1) ? 2 : 1; // Switch player
    }

    restoreInputMode(); // Restore terminal settings
    return 0;
}


// Set terminal input mode for non-canonical input
void setInputMode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Restore terminal settings
void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// Signal handler for graceful exit
void signalHandler(int signo) {
    restoreInputMode();
    printf("\nGame exited due to signal %d. Goodbye!\n", signo);
    sleep(1);
    exit(0);
}

// Initialize the board with empty spaces
void initializeBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

// Print the board
void printBoard(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf(" %c ", board[i][j]);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("---+---+---\n");
    }
    printf("\n");
}

// Check if a player has won
int checkWin(char board[SIZE][SIZE]) {
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return 1;
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return 1;
    }
    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return 1;
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return 1;

    return 0;
}

// Check if the game is a draw
int isDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') return 0;
        }
    }
    return 1;
}



void makeMove(char board[SIZE][SIZE], int player) {
    int row = -1, col = -1;
    char symbol = (player == 1) ? 'X' : 'O';
    char input;

    while (1) {
        system("clear");
        printBoard(board);
        printf("Player %d's turn (%c). Press 'q' to quit.\n", player, symbol);

        // Prompt for row
        printf("Enter row (1-3): ");
        fflush(stdout);

        if (read(STDIN_FILENO, &input, 1) == 1) {
            if (input == 'q' || input == 'Q') {
                printf("\nPlayer %d has quit the game. Goodbye!\n", player);
                restoreInputMode(); // Restore terminal settings
                sleep(1);
                exit(0);
            }

            if (input >= '1' && input <= '3') {
                row = input - '1'; // Convert to 0-based index
            } else {
                printf("\nInvalid input. Row must be between 1 and 3. Try again.\n");
                usleep(300000); // 300,000 microseconds = 0.3 seconds
                continue;
            }
        }

        // Prompt for column
        system("clear");
        printBoard(board);
        printf("Player %d's turn (%c). Press 'q' to quit.\n", player, symbol);
        printf("Enter column (1-3): ");
        fflush(stdout);

        if (read(STDIN_FILENO, &input, 1) == 1) {
            if (input == 'q' || input == 'Q') {
                printf("\nPlayer %d has quit the game. Goodbye!\n", player);
                restoreInputMode(); // Restore terminal settings
                exit(0);
            }

            if (input >= '1' && input <= '3') {
                col = input - '1'; // Convert to 0-based index
            } else {
                printf("\nInvalid input. Column must be between 1 and 3. Try again.\n");
                sleep(1); // Wait briefly before retrying
                continue;
            }
        }

        // Validate move
        if (board[row][col] == ' ') {
            board[row][col] = symbol;
            break; // Exit loop when move is valid
        } else {
            printf("\nInvalid move. Cell is already occupied. Try again.\n");
            sleep(1); // Wait briefly before retrying
        }
    }
}


