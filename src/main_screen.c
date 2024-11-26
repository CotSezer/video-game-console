#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>

#define MAX_GAMES 100
#define MAX_NAME_LENGTH 256

// Global terminal attributes
struct termios original_termios;

// Function prototypes
void setInputMode();
void restoreInputMode();
void signalHandler(int signo);
int scanGames(char games[MAX_GAMES][MAX_NAME_LENGTH]);
void printMenu(char games[MAX_GAMES][MAX_NAME_LENGTH], int gameCount, int selectedGame, int exitSelected);
void startGame(char* gameName);

int main() {
    char games[MAX_GAMES][MAX_NAME_LENGTH];
    int gameCount = scanGames(games);
    int selectedGame = 0;
    int exitSelected = 0;
    char input;
    int running = 1;

    if (gameCount == 0) {
        printf("No games found in the current directory.\n");
        return 1;
    }

    setInputMode(); // Set non-canonical input mode
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    while (running) {
        system("clear");
        printMenu(games, gameCount, selectedGame, exitSelected);

        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0; // Exit the main screen
            } else if (input == 'w') {
                if (exitSelected) {
                    // If currently on Exit, move focus to the last game
                    exitSelected = 0;
                    selectedGame = gameCount - 1;
                } else {
                    // Move up in the game list
                    if (selectedGame == 0) {
                        exitSelected = 1; // If at the first game, move to Exit
                    } else {
                        selectedGame--;
                    }
                }
            } else if (input == 's') {
                if (!exitSelected && selectedGame == gameCount - 1) {
                    // If currently on the last game, move focus to Exit
                    exitSelected = 1;
                } else if (exitSelected) {
                    // If currently on Exit, move focus to the first game
                    exitSelected = 0;
                    selectedGame = 0;
                } else {
                    // Move down in the game list
                    selectedGame++;
                }
            } else if (input == 'a' || input == 'd') {
                // Toggle focus between game list and Exit button
                exitSelected = !exitSelected;
            } else if (input == '\n') {
                if (exitSelected) {
                    running = 0; // Exit the main screen
                } else {
                    startGame(games[selectedGame]); // Launch selected game
                }
            }
        }
    }

    restoreInputMode();
    printf("\nThank you for using the video game console! Goodbye!\n");
    return 0;
}

// Set terminal input mode for non-canonical input
void setInputMode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios); // Get current terminal attributes
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios); // Apply new settings
}

// Restore the original terminal settings
void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// Signal handler for graceful exit
void signalHandler(int signo) {
    restoreInputMode(); // Restore terminal settings
    printf("\nMain screen exited due to signal %d. Goodbye!\n", signo);
    exit(0);
}

// Scan the current directory for games with names starting with "game_"
int scanGames(char games[MAX_GAMES][MAX_NAME_LENGTH]) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "game_", 5) == 0 && strstr(entry->d_name, ".c") == NULL) {
            strncpy(games[count], entry->d_name, MAX_NAME_LENGTH);
            games[count][MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
            count++;
            if (count >= MAX_GAMES) break; // Limit reached
        }
    }
    closedir(dir);
    return count;
}

// Print the main menu
void printMenu(char games[MAX_GAMES][MAX_NAME_LENGTH], int gameCount, int selectedGame, int exitSelected) {
    printf("=== Video Game Console ===\n");
    printf("Use 'w' and 's' to navigate, 'a' and 'd' to toggle, 'Enter' to select, and 'q' to quit.\n");
    printf("---------------------------\n");

    for (int i = 0; i < gameCount; i++) {
        if (!exitSelected && i == selectedGame) {
            printf(" > %s <\n", games[i]); // Highlight selected game
        } else {
            printf("   %s\n", games[i]);
        }
    }

    if (exitSelected) {
        printf(" > Exit <\n"); // Highlight Exit when selected
    } else {
        printf("   Exit\n");
    }

    printf("---------------------------\n");
}

// Start the selected game using fork()
void startGame(char* gameName) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork");
        return;
    }

    if (pid == 0) {
        // Child process: Execute the game
        char command[256];
        snprintf(command, sizeof(command), "./%s", gameName); // Add "./" prefix
        execlp(command, gameName, (char *)NULL);

        // If execlp fails, print an error and exit
        perror("Failed to start game");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Wait for the child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}
