#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MAX_N 16

int N;
int SRN;
int **board;
int **fixed;
int **solution;

// Alokacja pamięci

void allocBoard() {
    board = malloc(N * sizeof(int *));
    fixed = malloc(N * sizeof(int *));
    solution = malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        board[i] = calloc(N, sizeof(int));
        fixed[i] = calloc(N, sizeof(int));
        solution[i] = calloc(N, sizeof(int));
    }
}

// zwalnia zaalokowaną pamięć

void freeBoard() {
    for (int i = 0; i < N; i++) {
        free(board[i]);
        free(fixed[i]);
        free(solution[i]);
    }
    free(board);
    free(fixed);
    free(solution);
}

// Sprawdza czy dane są poprawne według zasad sudoku

int isSafe(int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (board[row][x] == num || board[x][col] == num)
            return 0;

    int startRow = row - row % SRN, startCol = col - col % SRN;
    for (int i = 0; i < SRN; i++)
        for (int j = 0; j < SRN; j++)
            if (board[i + startRow][j + startCol] == num)
                return 0;

    return 1;
}

// Wypełnia planszę sudoku

int fillBoard(int row, int col) {
    if (row == N - 1 && col == N)
        return 1;
    if (col == N) {
        row++;
        col = 0;
    }

    if (board[row][col] != 0)
        return fillBoard(row, col + 1);

    int nums[N];
    for (int i = 0; i < N; i++)
        nums[i] = i + 1;

    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }

    for (int i = 0; i < N; i++) {
        int num = nums[i];
        if (isSafe(row, col, num)) {
            board[row][col] = num;
            if (fillBoard(row, col + 1))
                return 1;
            board[row][col] = 0;
        }
    }

    return 0;
}

// Usuwa liczby z planszy, żeby gracz miał co wypełniać

void removeCells(int clues) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fixed[i][j] = 1;

    int cellsToRemove = N * N - clues;
    while (cellsToRemove > 0) {
        int i = rand() % N;
        int j = rand() % N;
        if (board[i][j] != 0) {
            board[i][j] = 0;
            fixed[i][j] = 0;
            cellsToRemove--;
        }
    }
}

// Wyświetla aktualny stan planszy

void printBoard() {
    printf("\n");
    for (int i = 0; i < N; i++) {
        if (i % SRN == 0 && i != 0)
            for (int k = 0; k < N * 3; k++) printf("-");
        printf("\n");
        for (int j = 0; j < N; j++) {
            if (j % SRN == 0 && j != 0)
                printf(" | ");
            if (board[i][j] == 0)
                printf(" . ");
            else
                printf("%2d ", board[i][j]);
        }
        printf("\n");
    }
}

// Zapisuje stan gry do pliku

void saveGame() {
    FILE *f = fopen("save.txt", "w");
    if (!f) {
        printf("Błąd zapisu\n");
        return;
    }
    fprintf(f, "%d\n", N);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fprintf(f, "%d %d %d\n", board[i][j], fixed[i][j], solution[i][j]);
    fclose(f);
    printf("Gra zapisana!\n");
}

// Wczytuje grę z pliku (działa po wyłączeniu programu i bez wyłączania programu)

int loadGame() {
    FILE *f = fopen("save.txt", "r");
    if (!f) {
        printf("Brak zapisu gry.\n");
        return 0;
    }

    fscanf(f, "%d", &N);
    SRN = sqrt(N);
    allocBoard();

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fscanf(f, "%d %d %d", &board[i][j], &fixed[i][j], &solution[i][j]);

    fclose(f);
    printf("Gra wczytana!\n");
    return 1;
}

// główna pętla gry. obsluguje ruchy gracza itp

void playGame() {
    int x, y, val;
    while (1) {
        printBoard();
        printf("Wprowadź ruch: wiersz kolumna wartość (np. 1 2 5), -1 aby zakończyć, -2 aby zapisać, -3 aby usunąć wartość: ");
        scanf("%d", &x);
        if (x == -1) break;
        if (x == -2) {
            saveGame();
            continue;
        }
        if (x == -3) {
            printf("Podaj wiersz i kolumnę do wyczyszczenia: ");
            scanf("%d %d", &x, &y);
            if (x >= 0 && x < N && y >= 0 && y < N) {
                if (fixed[x][y]) {
                    printf("Nie można usunąć stałej wartości.\n");
                } else {
                    board[x][y] = 0;
                }
            } else {
                printf("Błędne współrzędne.\n");
            }
            continue;
        }

        scanf("%d %d", &y, &val);
        if (x >= 0 && x < N && y >= 0 && y < N && val >= 1 && val <= N) {
            if (fixed[x][y]) {
                printf("Nie można zmienić już wypełnionej komórki.\n");
            } else if (val == solution[x][y]) {
                board[x][y] = val;
            } else {
                printf("Niepoprawny ruch\n");
            }
        } else {
            printf("Błędne dane wejściowe\n");
        }
    }
}

// Na podstawie poziomu trudności generuje planszę sudoku z danym procentem komórek do wypełnienia

void generateSudoku(int difficulty) {
    allocBoard();
    fillBoard(0, 0);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            solution[i][j] = board[i][j];

    int clues;
    switch (difficulty) {
        case 1: clues = N*N * 0.6; break;
        case 2: clues = N*N * 0.45; break;
        case 3: clues = N*N * 0.3; break;
        default: clues = N*N * 0.5; break;
    }
    removeCells(clues);
}

// Wyświetla menu i obsługuje wybór użytkownika.

void menu() {
    int choice;
    do {
        printf("\n==== SUDOKU ====\n");
        printf("1. Nowa gra\n2. Wczytaj grę\n3. Koniec programu\nWybór: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                printf("Rozmiar planszy (1 - 4x4, 2 - 9x9, 3 - 16x16): ");
                int sizeOpt; scanf("%d", &sizeOpt);
                if (sizeOpt == 1) N = 4;
                else if (sizeOpt == 2) N = 9;
                else N = 16;
                SRN = sqrt(N);

                printf("Poziom trudności (1 - łatwy, 2 - średni, 3 - trudny): ");
                int diff; scanf("%d", &diff);
                generateSudoku(diff);
                playGame();
                freeBoard();
                break;
            }
            case 2: {
                if (loadGame()) {
                    playGame();
                    freeBoard();
                }
                break;
            }
            case 3:
                printf("Koniec\n");
                break;
            default:
                printf("Nieprawidłowy wybór.\n");
        }
    } while (choice != 3);
}

// Funkcja główna. obsługuje menu i tworzy losową planszę sudoku na podstawie aktualnego czasu systemowego.

int main() {
    srand(time(NULL));
    menu();
    return 0;
}