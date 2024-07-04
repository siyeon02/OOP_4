// ******** Snake Video Game ************
// Only for MS Windows

// UP, DOWN, RIGHT, LEFT keys for controlling snake

// Directions of X, Y axis
// ^ -y(row)
// |
// |
// ---> x(col)


#include <iostream>
#include <windows.h>
#include<bits/stdc++.h>
#include<windows.h>
#include <unordered_set>
#include <unordered_map>
#include <conio.h>


using namespace std;

// Classes declaration
class Board;
class Food;
class Snake;
class Recode;
class Recode;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void printGAMEOVER();
boolean CheckDup(string);  // 이름 중복 확인

// 화면상 점 위치 표현 
struct position {
    int x, y;
    position() {}
    position(int a, int b) : x(a), y(b) {}
    bool operator== (const position& other) const {
        return (x == other.x && y == other.y);
    }
};

enum FoodType { COKE, BURGER, PIZZA, JELLY, BOMB };

struct FoodItem {
    char symbol;
    position pos;
    FoodType type;

    FoodItem(char sym, int x, int y, FoodType t) : symbol(sym), pos(x, y), type(t) {}
};

// Class implementing hash function to store objects of class "position"
class MyHash {
public:
    size_t operator()(const position& p) const {
        return (hash<int>()(p.x)) ^ (hash<int>()(p.y)); // xor of two hashes as a new hash
    }
};

// 게임 보드판 
class Board {
private:
    int height, width, score, speed; // height(along y), width(along x) of Playable Board
    char symbol;
    char** field;     //2차원 배열 표현
public:
    // Constructor
    Board() {
        height = 27; // initializing height of board
        width = 60;  // initializing width of board
        score = 0;
        speed = 300;

        symbol = '*'; //보드판

        // dynamic memory allocation for board
        field = new char* [height + 2];
        for (int i = 0; i < height + 2; i++)
            field[i] = new char[width + 2];
    }


    ~Board() { //소멸자
       // deallocating dynamic memory allocated previously
        for (int i = 0; i < height + 2; i++)
            delete[] field[i];

        delete[] field;
    }

    //보드를 화면에 출력
    void show_board() {
        // Get the console window dimensions
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int console_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int console_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        // Calculate the position to move the board to the middle
        int board_start_x = (console_width - (width + 2)) / 2;
        int board_start_y = (console_height - (height + 2)) / 2;

        // Display the board starting from the calculated position
        for (int i = 0; i < height + 2; i++) {
            gotoxy(board_start_x, board_start_y + i);
            for (int j = 0; j < width + 2; j++) {
                cout << field[i][j];
            }
        }
    }


    // 보드 초기화
    void clear_board() {
        for (int i = 0; i < height + 2; i++) {
            for (int j = 0; j < width + 2; j++)
                field[i][j] = ' ';
        }

        //set boundary
        for (int i = 0; i < width + 2; i++)
            field[0][i] = symbol;
        for (int i = 0; i < width + 2; i++)
            field[height + 1][i] = symbol;
        for (int i = 0; i < height + 2; i++)
            field[i][0] = symbol;
        for (int i = 0; i < height + 2; i++)
            field[i][width + 1] = symbol;
    }

    // returns board height
    int get_board_height() {
        return height;
    }

    // returns board width
    int get_board_width() {
        return width;
    }

    // set value on board
    void set_on_board(int row, int col, char c) {
        field[row][col] = c;
    }

    // return boundary symbol
    char get_boundary_symbol() {
        return symbol;
    }

    void update_score(int points) {
        score += points;
        gotoxy(width + 2, 0); //board 오른쪽 맨 위 /위치 바꾸기
        cout << "Score: " << score;
    }

    int get_score() const {
        return score;
    }

    void update_speed() {
        int speed_increase_count = score / 200; //200점 올릴 때 마다 점수 10퍼씩 증가
        speed = 300 - (speed_increase_count * 30);
    }

    int get_speed() const {
        return speed;
    }



}field; //object with name "field" of class "Board"



// Food class definition
class Food {

public:
    vector<FoodItem> food_items;

private:
    clock_t last_food_time;
    int food_interval;

    const unordered_map<FoodType, int> food_scores = {
       {COKE, 50},
       {BURGER, 100},
       {PIZZA, 150},
       {JELLY, 200},
       {BOMB,-1}
    }; //음식 종류, 그에 따라 늘어나는 점수

    const vector<char> food_symbols = { 'C', 'B', 'P', 'J', 'X' };

    //순서대로 COKE, BURGER, PIZZA, JELLY, BOMB 
    //COKE - 
    //BURGER - 
    //PIZZA - 
    //JELLY - 
    //BOMB - 죽음
    //하나 늘어나는거, 두개 늘어나는거, 두칸 작아지는거 등등 .....정하기


public:
    // Constructor
    Food() :food_interval(3000) { //3초
        last_food_time = clock();
    }

    const unordered_map<FoodType, int>& getFoodScores() const {
        return food_scores;
    }

    void add_new_food(Board& field, function<bool(int, int)> is_on_snake) {
        if ((clock() - last_food_time) >= food_interval) {
            int index = rand() % food_scores.size();
            FoodType type = static_cast<FoodType>(index);
            char symbol = food_symbols[type];
            position new_food;
            do {
                new_food.x = 1 + rand() % field.get_board_width();
                new_food.y = 1 + rand() % field.get_board_height();
            } while (is_on_snake(new_food.x, new_food.y));
            food_items.push_back(FoodItem(symbol, new_food.x, new_food.y, type));
            last_food_time = clock();
        }
    }


    void remove_eaten_food(const position& food_pos) {
        food_items.erase(
            remove_if(food_items.begin(), food_items.end(),
                [food_pos](const FoodItem& item) { return item.pos == food_pos; }),
            food_items.end());
    }

    void draw_food_on_board(Board& field) {
        for (auto& food_item : food_items) {
            field.set_on_board(food_item.pos.y, food_item.pos.x, food_item.symbol);
        }
    }

    //void update_food_time() {
    //    last_food_time = clock();
    //}


    bool is_food_at_position(const position& pos) {
        for (const auto& food_item : food_items) {
            if (food_item.pos == pos) {
                return true;
            }
        }
        return false;
    }

}eatable;

// Few important points w.r.t functioning :-
// Snake lenght increases after eating food,
// so here also it increases after it has passed food block i.e eaten food

// Snake class definition
//vector<position> body;

class Snake {
private:
    char body_head_symbol, body_part_symbol; // symbols for head and body(except) head of snake
    vector<position> body;                // storing body of snake body[0]->head, body[size-1]->tail
    int body_size;                     // size of snake's body
    position head;                     // position of snake's head on board
    enum direction { UP, DOWN, LEFT, RIGHT };   // directions of motion(w.r.t player in front of screen)
    direction dir;                     // current direction of motion of snake's head
    std::unordered_set <position, MyHash> uset;   // set stores unique body parts
public:
    // Constructor
    Snake() : body_head_symbol('@'), body_part_symbol('o'), dir(DOWN), body_size(1) {
        // set position of head
        position tmp(1 + rand() % (field.get_board_width() - 1), 1 + rand() % (field.get_board_height() - 1));
        body.push_back(tmp);

        head = body[0];
        uset.insert(head);
    }


    bool is_on_snake(int x, int y) {
        for (const auto& segment : body) {
            if (segment.x == x && segment.y == y) {
                return true;
            }
        }
        return false;
    }

    // returns x-coordinate of head
    int get_head_x() {
        return head.x;
    }

    // returns y-coordinate of head
    int get_head_y() {
        return head.y;
    }

    // returns symbol of snake's head
    char get_head_symbol() {
        return body_head_symbol;
    }

    // returns symbol of snake's non-head
    char get_nonhead_symbol() {
        return body_part_symbol;
    }

    // gets player input for direction of head and store in dir
    // windows.h --> GetAsyncKeyState --> checks if key was pressed after a previous call
    // VK_[key_name] refers to virtual key code
    void get_input() {
        if (GetAsyncKeyState(VK_UP) && dir != DOWN)
            dir = UP;
        else if (GetAsyncKeyState(VK_DOWN) && dir != UP)
            dir = DOWN;
        else if (GetAsyncKeyState(VK_LEFT) && dir != RIGHT)
            dir = LEFT;
        else if (GetAsyncKeyState(VK_RIGHT) && dir != LEFT)
            dir = RIGHT;
    }

    //지렁이의 움직임
    void move() {
        // Determine new head position
        position head_modify(0, 0);
        if (dir == UP) head_modify.y = -1;
        else if (dir == DOWN) head_modify.y = 1;
        else if (dir == RIGHT) head_modify.x = 1;
        else if (dir == LEFT) head_modify.x = -1;

        position new_head = { head.x + head_modify.x, head.y + head_modify.y };

        // Check for collisions with the body, excluding the last segment (tail)
        for (int i = 0; i < body_size - 1; ++i) {
            if (body[i].x == new_head.x && body[i].y == new_head.y) {
                printGAMEOVER();
                throw (string)"SNAKE DEAD.....!!!!";
                return;
            }
        }

        // 지렁이 몸 업데이트
        for (int i = body_size - 1; i > 0; --i) {
            body[i] = body[i - 1];
        }
        head = new_head;
        body[0] = head;

        // Check for wall collisions
        if (head.x <= 0 || head.y <= 0 || head.x >= field.get_board_width() + 1 || head.y >= field.get_board_height() + 1) {
            printGAMEOVER();
            throw (string)"SNAKE DEAD.....!!!!";
            return;
        }
    }

    bool food_found(Food& food, Board& board) {
        position head_pos = { get_head_x(), get_head_y() };
        for (auto it = food.food_items.begin(); it != food.food_items.end(); ++it) {
            if (it->pos == head_pos) {
                if (it->type == BOMB) {
                    printGAMEOVER();
                    throw (string)"SNAKE DEAD.....!!!!";
                }
                else {
                    int length_increase = determine_length_increase(it->type);
                    increase_snake_length(length_increase);
                    update_score_based_on_food(it->type, board);
                    food.food_items.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    void increase_snake_length(int length_increase) {
        position tail = body.back();
        for (int i = 0; i < length_increase; ++i) {
            body.push_back(tail); // Add new segments at the position of the tail
        }
        body_size += length_increase;
    }

    void update_score_based_on_food(FoodType type, Board& board) {
        auto scoreIterator = eatable.getFoodScores().find(type);
        if (scoreIterator != eatable.getFoodScores().end()) {
            int points = scoreIterator->second;
            board.update_score(points);
        }
    }

    int determine_length_increase(FoodType type) {
        switch (type) {
        case COKE:
            return 1;
        case BURGER:
            return 2;
        case PIZZA:
            return 3;
        case JELLY:
            return 4;
        default:
            return 0;
        }
    }


    // set the snake's symbols on board at it's position

    void set_snake_onboard(Board& draw_here) {

        if (head.x != 0 || head.y != 0) {
            field.set_on_board(head.y, head.x, body_head_symbol);
        }

        for (int i = 1; i < body.size(); i++) {

            if (body[i].x != 0 || body[i].y != 0) {
                field.set_on_board(body[i].y, body[i].x, body_part_symbol);
            }
        }
    }


}player;

class Record { //신기록
public:
    vector<pair<string, int>> high_scores;

    void load_scores() {
        ifstream file("high_scores.txt");
        string name;
        int score;
        while (file >> name >> score) {
            high_scores.push_back(make_pair(name, score));
        }
        file.close();

        // 점수 정렬
        sort(high_scores.begin(), high_scores.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
            });
        //한 플레이어의 아이디 당 가장 높은 점수만 저장
        high_scores.erase(unique(high_scores.begin(), high_scores.end(),
            [](const pair<string, int>& a, const pair<string, int>& b) {
                return a.first == b.first;
            }),
            high_scores.end());
    }

    void save_score(const string& name, int score) {

        // 저장되어있는 점수 확인
        auto it = find_if(high_scores.begin(), high_scores.end(),
            [&name](const pair<string, int>& element) {
                return element.first == name;
            });

        if (it != high_scores.end()) {
            // 새로운 점수가 더 높다면 그 점수 저장
            if (score > it->second) {
                it->second = score;
            }
        }
        else {
            // 저장되어있지 않은 플레이어 점수 새롭게 저장
            high_scores.push_back(make_pair(name, score));
        }


        sort(high_scores.begin(), high_scores.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
            });


        // 점수 정렬
        sort(high_scores.begin(), high_scores.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
            });



        // 파일에 저장
        ofstream file("high_scores.txt");
        for (const auto& hs : high_scores) {
            file << hs.first << " " << hs.second << "\n";
        }
        file.close();
    }
    void display_high_scores() {
        const int boxWidth = 24;
        const int maxDisplayScores = 15;
        const int boxHeight = min(static_cast<int>(high_scores.size()), maxDisplayScores) + 4;

        const char topLeft = '*';
        const char topRight = '*';
        const char horizontalBorder = '*';
        const char verticalBorder = '*';

        const int startX = 1; // X-coordinate
        const int startY = 1; // Y-coordinate
        //박스위
        gotoxy(startX, startY);
        for (int i = 0; i < boxWidth; ++i) {
            cout << topLeft;
        }
        cout << endl;

        //탑스코어 출력
        const string topScoresLabel = "Top Scores";
        const int topScoresLabelStartX = startX + (boxWidth - topScoresLabel.length()) / 2;
        gotoxy(topScoresLabelStartX, startY + 1);
        cout << topScoresLabel << endl;

        //highScore출력
        int rank = 1;
        int displayCount = 0;
        const int scoresStartY = startY + 3;
        const int scoresStartX = startX + 2;

        for (const auto& score : high_scores) {
            if (displayCount >= maxDisplayScores) {
                break;
            }

            gotoxy(scoresStartX, scoresStartY + displayCount);
            cout << "Rank " << rank << ": " << score.first << ": " << score.second << endl;

            ++rank;
            ++displayCount;
        }

        // 박스 아래
        const int endY = startY + boxHeight - 1;
        gotoxy(startX, endY);
        for (int i = 0; i < boxWidth; ++i) {
            cout << horizontalBorder;
        }
        cout << endl;
    }

};

// hide cursor function only in windowed mode (i.e not full screen)
// Reference : https://superuser.com/questions/1496322/how-do-i-remove-hide-the-cursor-the-blinking-underscore-character-in-cmd-exe
void hide_cursor() {
    HANDLE hStdOut = NULL;
    CONSOLE_CURSOR_INFO curInfo;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &curInfo);
}


void displayMenu() {

    system("cls");
    setColor(14); // Yellow color code for Windows console
    cout << "     \n\n\n\n\n        " << endl;
    cout << "        :::::::::       ::::::::::        :::   :::        ::::::::           :::          :::   :::       :::::::::" << endl;
    cout << "      :+:    :+:      :+:               :+:+: :+:+:      :+:    :+:        :+: :+:       :+:+: :+:+:      :+:         " << endl;
    cout << "     +:+    +:+      +:+              +:+ +:+:+ +:+     +:+              +:+   +:+     +:+ +:+:+ +:+     +:+          " << endl;
    cout << "    +#++:++#+       +#++:++#        +#+  +:+  +#+     :#:             +#++:++#++:    +#+  +:+  +#+     +#++:++#      " << endl;
    cout << "   +#+    +#+      +#+             +#+       +#+     +#+   +#+#      +#+     +#+    +#+       +#+     +#+           " << endl;
    cout << "  #+#    #+#      #+#             #+#       #+#     #+#    #+#      #+#     #+#    #+#       #+#     #+#             " << endl;
    cout << "  #########       ##########     ###       ###      ########       ###     ###    ###       ###     ##########      " << endl;
    setColor(15);
    cout << "\033[1m\n\n"; // Set text to bold]
    cout << "\033[31m                                                 0. Game Tutorial                   " << endl;
    cout << "\033[33m                                                   1. Play Game                   " << endl;
    cout << "\033[32m                                                2. View High Scores            " << endl;
    cout << "\033[36m                                                     3. Exit                        " << endl;
    cout << "\033[0m\n"; // Reset text attributes to default

    cout << "                                                 Choose an option: ";

}
void viewHighScore(Record& record) {
    system("cls");

    record.display_high_scores();

    cout << "                                                    _________________________________" << endl;
    cout << "                                                   ||                               ||" << endl;
    cout << "                                                   ||       press M to return       ||" << endl;
    cout << "                                                   ||_______________________________||" << endl;
    cout << "                                                   |/_______________________________\\|" << endl;
    char returnKey;
    do {
        returnKey = _getch();
    } while (toupper(returnKey) != 'M');

    system("cls");
}


// 튜토리얼
void showTutorial()
{
    system("cls");
    setColor(11); //연한 파란 민트..?
    std::cout << "                       ######   ##  ##   ######    ####    #####     ####      ##     ##" << std::endl;
    std::cout << "                         ##     ##  ##     ##     ##  ##   ##  ##     ##      ####    ##" << std::endl;
    std::cout << "                         ##     ##  ##     ##     ##  ##   ##  ##     ##     ##  ##   ##" << std::endl;
    std::cout << "                         ##     ##  ##     ##     ##  ##   #####      ##     ######   ##" << std::endl;
    std::cout << "                         ##     ##  ##     ##     ##  ##   ####       ##     ##  ##   ##" << std::endl;
    std::cout << "                         ##     ##  ##     ##     ##  ##   ## ##      ##     ##  ##   ##" << std::endl;
    std::cout << "                         ##      ####      ##      ####    ##  ##    ####    ##  ##   ######" << std::endl;

    cout << "\n                       01110100 01110101 01110100 01101111 01110010 01101001 01100001 01101100 \n\n       " << endl;
    setColor(15);
    cout << "                            ♣ Earthworms grow when they eat snacks          " << endl;
    cout << "                            ♣ Types of snacks : Each snack has a different score *^^*           " << endl;
    cout << "                                           C (Coke)   : 50점            " << endl;
    cout << "                                           B (Burger) : 100점            " << endl;
    cout << "                                           P (Pizza)  : 150점            " << endl;
    cout << "                                           J (Jelly)  : 200점            " << endl;
    cout << "                            ♣ Eating a bomb (X) kills worms!         " << endl;
    cout << "                            ♣ Toching thw walls kills worms!!           " << endl;
    cout << "\n\n";
    cout << "                            ♣ Arrow keys :\n\n";                            // press M 출력부분 코드상으론 싱크가 안 맞아도 출력은 정상적
    cout << "                                    ┏━━━━┓                             _________________________________" << endl;
    cout << "                                    ┃  ^ ┃                            ||                               ||" << endl;
    cout << "                               ┏━━━━╋━━━━╋━━━━┓                       ||       press M to return       ||" << endl;
    cout << "                               ┃  < ┃  v ┃  > ┃                       ||_______________________________||" << endl;
    cout << "                               ┗━━━━┻━━━━┻━━━━┛                       |/_______________________________\\|\n\n" << endl;

    // M 키 받기
    char returnKey;
    do {
        returnKey = _getch();
    } while (toupper(returnKey) != 'M');

    //메인메뉴 가기 직전
    system("cls");
    return;
}

void displayGameEnd()
{
    system("cls");
    cout << "\n\n\n\n\n";
    cout << "        :::::::::       ::::::::::        :::   :::        ::::::::           :::          :::   :::       :::::::::" << endl;
    cout << "      :+:    :+:      :+:               :+:+: :+:+:      :+:    :+:        :+: :+:       :+:+: :+:+:      :+:         " << endl;
    cout << "     +:+    +:+      +:+              +:+ +:+:+ +:+     +:+              +:+   +:+     +:+ +:+:+ +:+     +:+          " << endl;
    cout << "    +#++:++#+       +#++:++#        +#+  +:+  +#+     :#:             +#++:++#++:    +#+  +:+  +#+     +#++:++#      " << endl;
    cout << "   +#+    +#+      +#+             +#+       +#+     +#+   +#+#      +#+     +#+    +#+       +#+     +#+           " << endl;
    cout << "  #+#    #+#      #+#             #+#       #+#     #+#    #+#      #+#     #+#    #+#       #+#     #+#             " << endl;
    cout << "  #########       ##########     ###       ###      ########       ###     ###    ###       ###     ##########      " << endl;

    cout << "\033[1m\n\n"; // Set text to bold
    cout << "                                                see you next time~                 " << endl;
    std::cout << "  \n";



    // M 키 받기
    char returnKey;
    do {
        returnKey = _getch();
    } while (toupper(returnKey) != 'M');

    //메인메뉴 가기 직전
    system("cls");
    return;
}

// 게임시작 
void startGame(string player_name, Record& record) {
    system("cls");

    field.update_score(0);
    player = Snake();
    eatable.food_items.clear();
    field.clear_board();

    while (1) {
        field.clear_board();
        player.get_input();

        try {
            player.move();

            if (player.food_found(eatable, field)) {
                field.update_speed();
            }

            eatable.add_new_food(field, [](int x, int y) { return player.is_on_snake(x, y); });

            eatable.draw_food_on_board(field);
            player.set_snake_onboard(field);

            record.display_high_scores();

            // 현재 점수 출력
            gotoxy(0, field.get_board_height() + 5); // Y-coordinate 스코어 
            cout << "\nCurrent Score: " << field.get_score();
            gotoxy(0, field.get_board_height() + 6); // Y-coordinate 플레이어
            cout << "\nPlayer: " << player_name;
            field.show_board();
        }
        catch (string err) {
            //보드 클리어
            for (int i = 0; i < field.get_board_height() + 2; i++) {
                gotoxy(0, i);
                for (int j = 0; j < field.get_board_width() + 2; j++) {
                    cout << " ";
                }
            }

            //점수 저장, 높은점수 출력
            record.save_score(player_name, field.get_score());
            record.display_high_scores();
            cout << "\n                                       Your name : " << player_name;
            cout << endl;
            cout << "\n                                    << Your Final Score: " << field.get_score() << " >>";
            field.update_speed(); //속도 해결 
            Sleep(3000);  // 최고점 출력 후 3초 대기
            system("cls");

            for (int i = 0; i < field.get_board_height() + 2; i++) {
                gotoxy(0, i);
                for (int j = 0; j < field.get_board_width() + 2; j++) {
                    cout << " ";
                }
            }

            system("cls"); // Clear screen
            displayMenu();
            break;
        }

        Sleep(field.get_speed());
    }
}

int main() {


    string player_name;
    Record record;
    record.load_scores();
    hide_cursor();
    srand(time(0));
    int choice;
    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
        case 0:
            // Show Tutorial
            showTutorial();
            break;
        case 1:
            // Start new game
            do {
                cout << "                                                Enter player name: ";
                cin >> player_name;
            } while (CheckDup(player_name));
            startGame(player_name, record);
            break;
        case 2:
            viewHighScore(record);
            break;
        case 3:
            displayGameEnd();
            return 0;
        default:
            cout << "Invalid choice. Please choose again.\n";
            break;
        }

    } while (choice != 3);

    return 0;
}

void printGAMEOVER()
{
    for (int i = 0; i < 5; i++) {
        system("cls");
        Sleep(150);
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        setColor(14); // 노랑
        std::cout << "        #########        ####          ####     ####      #########                          " << std::endl;
        std::cout << "       ##       ##     ##    ##       ##  ##   ##  ##     ##                           " << std::endl;
        std::cout << "       ##       ##    ##      ##      ##  ##   ##  ##     ##                 " << std::endl;
        std::cout << "       ##             ##########     ##    ## ##    ##    #########                       " << std::endl;
        std::cout << "       ##   ######    ##      ##     ##    ## ##    ##    ##                   " << std::endl;
        std::cout << "       ##       ##    ##      ##    ##      ##       ##   ##                    " << std::endl;
        std::cout << "        ######  ##    ##      ##    ##      ##       ##   #########                      " << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        setColor(12); // 빨강
        std::cout << "                                         ####     ##       ##  #########   #######                      " << std::endl;
        std::cout << "                                        ##   ##   ##       ##  ##          ##    ##       " << std::endl;
        std::cout << "                                       ##     ##   ##     ##   ##          ##    ##         " << std::endl;
        std::cout << "                                       ##     ##   ##     ##   #########   ######                    " << std::endl;
        std::cout << "                                       ##     ##    ##   ##    ##          ##   ##           " << std::endl;
        std::cout << "                                        ##    ##    ##   ##    ##          ##    ##            " << std::endl;
        std::cout << "                                          ####        ##       #########   ##     ##                   " << std::endl;
        Sleep(300);
    }
    Sleep(500);
    setColor(7); // 원래 색
    system("cls");
}

boolean CheckDup(string player_name)
{
    char answer;
    const std::string filePath = "high_scores.txt";
    // 파일 열기
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "file cannot be oppend!" << std::endl;
        return 0;
    }
    std::string line;
    // 각 줄을 읽어서 처리
    while (std::getline(file, line)) {
        // 각 줄을 공백을 기준으로 나누기
        std::istringstream iss(line);
        std::string firstWord;
        iss >> firstWord;

        if (firstWord == player_name)
        {
            cout << endl;
            cout << "                                              -----------------------\n";
            cout << "                                               !Name already exists!\n";
            cout << "                                              -----------------------\n";
            cout << "                  If you play with the same name, your score will be renewed with the higher of the two.\n";
            cout << "                                         Do you want to play with this name?\n";

            while (1)
            {

                cout << "                                       enter your answer if Yes -> y / No -> n : ";
                cin >> answer;
                cout << endl;
                if (answer == 'y' || answer == 'Y')
                    return false;
                else if (answer == 'n' || answer == 'N')
                {
                    cout << "                                              Please enter a new name.\n";
                    return true;
                }
                else
                    cout << "                                                 ! Invalid input !\n";
            }
        }
    }
    return false;
}