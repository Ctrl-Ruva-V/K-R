#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

const int width = 30;  
const int height = 20;

int x, y;
int fruitX, fruitY;
int specialFruitX, specialFruitY;
int tailX[200], tailY[200];
int dlina;
int score;
int highScore = 0;
bool gameOver;
bool specialFruit;
int mode;
int speed = 100;

string playerName;
string highScorePlayer = "Нет";

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

int snakeColor = 10;

// === Загрузка рекорда ===
void LoadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScorePlayer >> highScore;
        file.close();
    }
    else {
        highScore = 0;
        highScorePlayer = "Нет";
    }
}

// === Сохранение рекорда ===
void SaveHighScore() {
    if (score > highScore) {
        ofstream file("highscore.txt");
        if (file.is_open()) {
            file << playerName << " " << score;
            file.close();

            highScore = score;
            highScorePlayer = playerName;

            cout << "Новый рекорд сохранён от игрока: " << playerName << "!" << endl;
        }
    }
}


// === Запись в лог ===
void LogScore() {
    ofstream logFile("gamelog.txt", ios::app);
    if (logFile.is_open()) {
        time_t now = time(0);
        char dt[26];
        ctime_s(dt, sizeof(dt), &now);
        logFile << "Игрок: " << playerName << " | Очки: " << score << " | Дата и время: " << dt;
        logFile.close();
    }
}

// === Настройка игры ===
void Setup() {
    gameOver = false;
    dir = RIGHT;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    dlina = 0;
    specialFruit = false;
    speed = 100;
}

// === Отрисовка поля ===
void Draw() {
    system("cls");

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "#";

            if (i == y && j == x) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), snakeColor);
                cout << "O";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            else if (i == fruitY && j == fruitX) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                cout << "1";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            else if (specialFruit && i == specialFruitY && j == specialFruitX) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
                cout << "@";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            else {
                bool print = false;
                for (int k = 0; k < dlina; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), snakeColor);
                        cout << "o";
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        print = true;
                        break;
                    }
                }
                if (!print) cout << " ";
            }

            if (j == width - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    cout << "Игрок: " << playerName << " | Очки: " << score
        << " | Рекорд: " << highScore << " (от " << highScorePlayer << ")" << endl;
    if (mode == 3) cout << "Скорость: " << (110 - speed) << endl;
}

// === Управление ===
void Input() {
    if (_kbhit()) {
        int key = _getch();
        if (key == 224) {
            key = _getch();
            switch (key) {
            case 72: if (dir != DOWN) dir = UP; break;
            case 80: if (dir != UP) dir = DOWN; break;
            case 75: if (dir != RIGHT) dir = LEFT; break;
            case 77: if (dir != LEFT) dir = RIGHT; break;
            }
        }
        else {
            switch (key) {
            case 'w': case 'ц': if (dir != DOWN) dir = UP; break;
            case 's': case 'ы': if (dir != UP) dir = DOWN; break;
            case 'a': case 'ф': if (dir != RIGHT) dir = LEFT; break;
            case 'd': case 'в': if (dir != LEFT) dir = RIGHT; break;
            case 'x': case 'ч': gameOver = true; break;
            }
        }
    }
}

// === Логика игры ===
void Logic() {

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < dlina; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    }

    if (mode == 1) {
        if (x >= width) x = 0; else if (x < 0) x = width - 1;
        if (y >= height) y = 0; else if (y < 0) y = height - 1;
    }
    else {
        if (x >= width || x < 0 || y >= height || y < 0)
            gameOver = true;
    }

    for (int i = 0; i < dlina; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score++;
        fruitX = rand() % width;
        fruitY = rand() % height;
        dlina++;
        snakeColor = 9 + rand() % 7;

        if (mode == 3 && speed > 30)
            speed -= 5;

        if (rand() % 5 == 0) {
            specialFruit = true;
            specialFruitX = rand() % width;
            specialFruitY = rand() % height;
        }
    }

    if (specialFruit && x == specialFruitX && y == specialFruitY) {
        score += 5;
        specialFruit = false;
        dlina++;
        snakeColor = 9 + rand() % 7;

        if (mode == 3 && speed > 30)
            speed -= 5;
    }
}

// === Главная функция ===
int main() {
    srand(time(0));

    LoadHighScore();

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char repeat;
    do {
        cout << "Введите ваше имя (до 20 символов): ";
        cin >> playerName;
        if (playerName.length() > 20)
            playerName = playerName.substr(0, 20);

        cout << "\nВыберите режим:\n";
        cout << "1 - Легкий (проходит сквозь стены)\n";
        cout << "2 - Сложный (смерть от стены)\n";
        cout << "3 - Экстрим (ускорение + смерть от стены)\n";
        cin >> mode;

        Setup();

        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(speed);
        }

        SaveHighScore();
        LogScore();

        cout << "Игра окончена! Очки: " << score << endl;
        cout << "Рекорд: " << highScore << " (от " << highScorePlayer << ")" << endl;
        cout << "Хотите сыграть снова? (1 - да / 2 - нет): ";
        cin >> repeat;

    } while (repeat == '1');

    cout << "\nСпасибо за игру, " << playerName << "!" << endl;
    return 0;
}
