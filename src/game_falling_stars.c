#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#define ROWS 15
#define COLS 20
#define PADDLE_WIDTH 3

// Global variables
struct termios original_termios;
int paddle_pos = COLS / 2 - 1;

// Function prototypes
void initializeGrid(char grid[ROWS][COLS]);
void printGrid(char grid[ROWS][COLS]);
void updateGrid(char grid[ROWS][COLS]);
void movePaddle(char direction);
void dropStar(char grid[ROWS][COLS]);
int checkCollision(char grid[ROWS][COLS]);
void setInputMode();
void restoreInputMode();
void signalHandler(int signo);

int main() {
    char grid[ROWS][COLS];
    int score = 0, running = 1;
    char input;

    srand(time(NULL));
    setInputMode();
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    initializeGrid(grid);

    while (running) {
        system("clear");
        printGrid(grid);
        printf("Score: %d\n", score);
        printf("Use 'a' to move left, 'd' to move right, 'q' to quit.\n");

        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0;
            } else if (input == 'a' || input == 'd') {
                movePaddle(input);
            }
        }

        dropStar(grid);
        if (checkCollision(grid)) {
            score++;
        }
        updateGrid(grid);
        usleep(200000); // Adjust game speed
    }

    restoreInputMode();
    printf("\nGame over! Final Score: %d\n", score);
    return 0;
}

void initializeGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = ' ';
        }
    }
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        grid[ROWS - 1][i] = '=';
    }
}

void printGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

void updateGrid(char grid[ROWS][COLS]) {
    for (int i = ROWS - 2; i >= 0; i--) {
        for (int j = 0; j < COLS; j++) {
            grid[i + 1][j] = grid[i][j];
        }
    }
    for (int i = 0; i < COLS; i++) {
        grid[0][i] = ' ';
    }
    for (int i = 0; i < COLS; i++) {
        grid[ROWS - 1][i] = ' ';
    }
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        grid[ROWS - 1][i] = '=';
    }
}

void movePaddle(char direction) {
    if (direction == 'a' && paddle_pos > 0) {
        paddle_pos--;
    } else if (direction == 'd' && paddle_pos + PADDLE_WIDTH < COLS) {
        paddle_pos++;
    }
}

void dropStar(char grid[ROWS][COLS]) {
    int col = rand() % COLS;
    grid[0][col] = '*';
}

int checkCollision(char grid[ROWS][COLS]) {
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        if (grid[ROWS - 2][i] == '*') {
            return 1;
        }
    }
    return 0;
}

void setInputMode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void signalHandler(int signo) {
    restoreInputMode();
    printf("\nGame exited due to signal %d. Goodbye!\n", signo);
    exit(0);
}
