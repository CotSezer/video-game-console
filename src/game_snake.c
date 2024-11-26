#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define ROWS 15
#define COLS 15
#define INITIAL_SNAKE_LENGTH 1

// Global terminal settings
struct termios original_termios;

// Snake structure
typedef struct {
    int row, col;
} SnakePart;

// Function prototypes
void setInputMode();
void restoreInputMode();
void initializeGrid(char grid[ROWS][COLS]);
void printGrid(char grid[ROWS][COLS]);
void placeBait(char grid[ROWS][COLS], int* baitRow, int* baitCol, SnakePart* snake, int length);
void updateGrid(char grid[ROWS][COLS], SnakePart* snake, int length, int baitRow, int baitCol);
int moveSnake(SnakePart* snake, int* length, char direction, int baitRow, int baitCol);
int checkCollision(SnakePart* snake, int length);
void waitForNewInput();

int main() {
    char grid[ROWS][COLS];
    SnakePart snake[ROWS * COLS]; // Maximum possible length
    int snakeLength = INITIAL_SNAKE_LENGTH;
    int baitRow, baitCol;
    char input = 'd'; // Start moving to the right
    int running = 1;

    // Initialize terminal
    setInputMode();
    atexit(restoreInputMode);

    // Initialize game
    srand(time(NULL));
    initializeGrid(grid);
    snake[0].row = ROWS / 2;
    snake[0].col = COLS / 2; // Snake starts in the middle
    placeBait(grid, &baitRow, &baitCol, snake, snakeLength);
    updateGrid(grid, snake, snakeLength, baitRow, baitCol);

while (running) {
        system("clear");
        printGrid(grid);
        printf("Use 'w', 'a', 's', 'd' to move. Press 'q' to quit.\n");

        usleep(200000); // Adjust game speed

        // Read user input
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0; // Exit the game
                continue;
            } else if (input != 'w' && input != 'a' && input != 's' && input != 'd') {
                continue; // Ignore invalid inputs
            }
        }

        // Attempt to move the snake
        if (!moveSnake(snake, &snakeLength, input, baitRow, baitCol)) {
            printf("\nInvalid move. Snake hit the border or itself. Waiting for new input...\n");

            // Wait for valid input after an invalid move
            while (1) {
                if (read(STDIN_FILENO, &input, 1) > 0) {
                    if (input == 'q') {
                        running = 0; // Exit on 'q'
                        break;
                    } else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
                        // Check if the new input is valid and proceed
                        if (moveSnake(snake, &snakeLength, input, baitRow, baitCol)) {
                            break; // Valid move found, exit wait loop
                        } else {
                            printf("\nInvalid move. Try again.\n");
                        }
                    }
                }
            }

            if (!running) {
                break; // Exit the main loop if the game is terminated
            }
        }

        // Update the grid and check for bait
        updateGrid(grid, snake, snakeLength, baitRow, baitCol);

        // Check if the snake eats the bait
        if (snake[0].row == baitRow && snake[0].col == baitCol) {
            snakeLength++;
            placeBait(grid, &baitRow, &baitCol, snake, snakeLength);
        }
    }



    printf("\nGame Over. Thank you for playing!\n");
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

// Wait for new valid input after an invalid move
void waitForNewInput() {
    char input;
    while (1) {
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
                break; // Exit loop on valid input
            }
        }
    }
}

// Initialize the grid with empty spaces
void initializeGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = '.';
        }
    }
}

// Print the grid
void printGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Place bait at a random location
void placeBait(char grid[ROWS][COLS], int* baitRow, int* baitCol, SnakePart* snake, int length) {
    int valid = 0;
    while (!valid) {
        *baitRow = rand() % ROWS;
        *baitCol = rand() % COLS;
        valid = 1;
        // Ensure bait does not spawn on the snake
        for (int i = 0; i < length; i++) {
            if (snake[i].row == *baitRow && snake[i].col == *baitCol) {
                valid = 0;
                break;
            }
        }
    }
    grid[*baitRow][*baitCol] = 'X';
}

// Update the grid with the snake and bait
void updateGrid(char grid[ROWS][COLS], SnakePart* snake, int length, int baitRow, int baitCol) {
    // Clear the grid
    initializeGrid(grid);

    // Place the bait
    grid[baitRow][baitCol] = 'X';

    // Place the snake on the grid
    for (int i = 0; i < length; i++) {
        if (i == 0) {
            grid[snake[i].row][snake[i].col] = 'O'; // Snake's head
        } else {
            grid[snake[i].row][snake[i].col] = '#'; // Snake's body
        }
    }
}


// Move the snake
int moveSnake(SnakePart* snake, int* length, char direction, int baitRow, int baitCol) {
    SnakePart nextHead = snake[0]; // Calculate the next head position

    // Update the next head position based on the direction
    switch (direction) {
        case 'w': nextHead.row--; break;
        case 'a': nextHead.col--; break;
        case 's': nextHead.row++; break;
        case 'd': nextHead.col++; break;
    }

    // Check for border collision
    if (nextHead.row < 0 || nextHead.row >= ROWS || nextHead.col < 0 || nextHead.col >= COLS) {
        return 0; // Invalid move
    }

    // Check for self-collision
    for (int i = 0; i < *length; i++) {
        if (nextHead.row == snake[i].row && nextHead.col == snake[i].col) {
            return 0; // Invalid move
        }
    }

    // Move the snake's body
    for (int i = *length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Update the snake's head
    snake[0] = nextHead;

    return 1; // Valid move
}

