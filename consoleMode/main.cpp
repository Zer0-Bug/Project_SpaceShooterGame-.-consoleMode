#include <stdio.h>                                                          // Using this header for: using basic input/output operations like printf, scanf, etc.
#include <time.h>                                                           // Using this header for: time
#include <windows.h>                                                        // Using this header for: working with windows
#include <pthread.h>                                                        // Using this header for: thread & multithread
#include <string.h>                                                         // Using this header for: string manipulation (It includes operations such as copying, comparing, merging, etc.)
#include <conio.h>                                                          // The console contains functions that enable input/output operations.


int bulletCount = 0;                                                        // Holds the total number of bullets in the game. Initially, it is initialized to 0 because there are no bullets in the game.
int FIRE = 0;                                                               // Holds the number of frames since the last fire. This is used to control when the next fire is fired.
int playerY = 8;                                                            // Player's Y position
int FIRERATE = 500;                                                         // It determines how many milliseconds a player can fire a bullet.
int SPEED = 75;                                                             // It determines the delay between frames, i.e. how fast the game runs. A lower value means a faster game.


typedef struct                                                              // The word "struct" allows you to define your own data type by combining variables of one or more different data types.
{
    int x;                                                                  // Enemy's X position
    int y;                                                                  // Enemy's Y position
} PIX;                                                                      // Our struct name is "PIX".

PIX bullets[100];                                                           // An array for the bullets.
PIX enemy;



// The pthread_t data type is used for defining and managing threads. A thread represents a separate workflow within a program.
// This allows a program to perform multiple tasks simultaneously.
// For example, one thread may listen for user input, while another thread may perform file reading.
pthread_t inputThread;                                                      // Variable for the input thread.
char input = '\0';                                                          // Variable for the user input. Initially initialized with the null character ('\0'),
                                                                            // which means that the user has not yet logged in. User input is processed by working on the value of this variable.


void *GetUserInput();                                                       // only get user input
void Move();                                                                // to move game elements (e.g. player, enemies, bullets)
void Draw();                                                                // to draw and show the current state of the game on the screen
void Start();                                                               // to set the game state at startup (like determining the starting position of the player or enemy)
void HitTest();                                                             // to control collisions (such as a bullet hitting an enemy)
void SpawnEnemy();                                                          // to spawn an enemy
void SpawnBullet();                                                         // to spawn a bullet
void DespawnBullet(int idx);                                                // to destroy the bullet if it hits the target or leaves the screen
int playerCollision();                                                             // to control the situation where the enemy touches the player
int IsBullet(int x, int y);                                                 // to test if there is a bullet at X,Y


void *GetUserInput(void *arg) {
    while (TRUE) {
        input = getch();
    }
    return NULL;
}


void Move() {

    enemy.x--;                                                              // moves the enemy to the left by subtracting 1 from the enemy's x position and then,
    HitTest();                                                              // the HitTest function is called to check the collision.

    for (int i = 0; i < bulletCount; i++) {                                 // checks the movement of the available bullets and checks if the they reach the right edge of the screen

        if (bullets[i].x >= 33) {                                           // If bullet x is greater then or equal to 33 (33 is the range we set as the far right of the screen),
            DespawnBullet(i);                                               // then despawn the bullet.
        }

        bullets[i].x += 2;                                                  // If the bullet has not reached the right edge of the screen,
                                                                            // the bullet is moved 2 units to the right (you can think of 2 units as bullet speed).

    }


    // Switch-Case for movement of the Player and Fire.
    switch (tolower(input)) {


        case 'w':
            if (playerY > 1) {                                              // 1 is the vertex point.
                playerY--;                                                  // Moves up 1 unit.
            }
            break;



        case 's':
            if (playerY < 15) {                                             // 15 is the bottom point.
                playerY++;                                                  // Moves down 1 unit.
            }
            break;



        case ' ':                                                           // SPACE bar
            if (FIRE == 0) {
                SpawnBullet();
            }
            break;



        case 'q':
            exit(0);                                                        // Exit game
            break;

    }



    input = '\0';                                                           // Set input to the null character (0x00).
}





void Draw() {

    char buff[5000];

    strcpy(buff, "\e");                                                 // Copy "move up 20 lines" into the buffer
    strcat(buff, "\e");                                                 // Append "move back (left) 34 characters" to the buffer.




    for (int y = 0; y < 19; y++) {                                          // This loop will run 19 times on the y-axis and process the rows.

        for (int x = 0; x < 34; x++) {                                      // This loop will run 34 times on the x-axis and will process the characters that make up each line.

                                                                            // So our playing field is 34x19.

            if (y == 0 || y == 18) {
                strcat(buff, "__");                                  // It draws the boundary lines that mark the borders of the playing field.
            }


            else if (y == playerY && (x == 2 || x == 3)) {

                if (x == 2) {
                    strcat(buff, " /");
                }
                                                                            // Modeling of the space shuttle (first line).
                else if (x == 3) {
                    strcat(buff, "]\\");
                }

            }



            else if (y == playerY + 1  &&  x >= 2  &&  x <= 5) {

                switch(x) {

                    case 2:
                        strcat(buff, "<]");
                        break;

                    case 3:
                        strcat(buff, "()");
                        break;
                                                                            // Modeling of the space shuttle (second line).
                    case 4:
                        strcat(buff, "]]");
                        break;

                    case 5:
                        strcat(buff, "C>");
                        break;
                }
            }



            else if (y == playerY + 2  &&  (x == 2 || x == 3)) {            // Modeling of the space shuttle (third line).

                if (x == 2) {
                    strcat(buff, " \\");
                }

                else if (x == 3) {
                    strcat(buff, "]/");
                }
            }



            else if (IsBullet(x, y)) {
                strcat(buff, "==");                                     // Bullet firing.
            }



            else if (x >= enemy.x  &&  x <= enemy.x + 3  &&  y == enemy.y) {

                if (x == enemy.x) {
                    strcat(buff, " <");
                }

                else if (x == enemy.x + 1) {
                    strcat(buff, "( ");
                }
                                                                            // Modeling the enemy space shuttle (to draw the main body part).
                else if (x == enemy.x + 2) {
                    strcat(buff, "[>");
                }

                else if (x == enemy.x + 3) {
                    strcat(buff, "C>");
                }

            }



            else if (( x == enemy.x + 1  ||  x == enemy.x + 2)  &&  y == enemy.y - 1) {

                if (x == enemy.x + 1) {
                    strcat(buff, " /");
                }
                                                                            // Modeling the enemy space shuttle (to draw the upper body part).
                else if (x == enemy.x + 2) {
                    strcat(buff, "-|");
                }
            }



            else if ((x == enemy.x + 1  ||  x == enemy.x + 2)  &&  y == enemy.y + 1) {

                if (x == enemy.x + 1) {
                    strcat(buff, " \\");
                }
                                                                            // Modeling the enemy space shuttle (to draw the lower body part).
                else if (x == enemy.x + 2) {
                    strcat(buff, "-|");
                }
            }



            else {
                strcat(buff, "  ");                                      // Adds a space character where no condition is met.
            }


        } // End of inner for loop.

        strcat(buff, "\n");                                                 // Moves to the next line.

    } // End of outer for loop.

    printf(buff);                                                           // The game scene is printed on the screen.

    printf("\e[1;1H");
}





void Start() {                                                              // Setup for the game


    for (int i = 0; i < 20; i++) {                                          // This is used to clear the top of the console screen before the game starts.

        printf("\n");
        system("cls");                                                       // Clears the current view, adding 20 new lines to the console screen.
    }

    pthread_create(&inputThread, NULL, GetUserInput, NULL);                 // I created a pthread to listen for user input in the background.
                                                                            // The "IN" is the function I defined above. (It was a function that listens for the user to input from the keyboard.)

    srand(time(0));
    // Since the return value of the time(0) function is always different, srand(time(0)) sets the starting value of the random number generator to a different timestamp each time.
    // This means that each time the program runs, it will have a different starting point for generating random numbers.
    // This is especially useful in situations like games, where you want to get different results each time.

    SpawnEnemy();                                                           // Spawn a enemy

    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);

}





void HitTest() {

    if (enemy.x == -1) {                                                    // All enemies have been destroyed and there are no more enemies.

        exit(0);                                                            //  The game ends.
    }

    if (playerCollision()) {                                                       // The player has failed to destroy the enemy and the enemy has entered the player's area.

        exit(0);                                                            // The player has lost and the game ends.
    }

    for (int i = 0; i < bulletCount; i++) {                                 // It tests whether each bullet has collided with the enemy.

        if (bullets[i].y >= enemy.y - 1   &&   bullets[i].y <= enemy.y + 1   &&   bullets[i].x >= enemy.x   &&   bullets[i].x <= enemy.x + 2) {
            // The bullet hits the enemy if these conditions are met.

            SpawnEnemy();                                                   // If a bullet hits an enemy, a new enemy is created.

            DespawnBullet(i);                                               // If a bullet hits the enemy, it destroys the bullet.

        }
    }
}




void SpawnEnemy() {

    enemy.y = ((rand() % 15) + 2);                                          // rand() % 15 => Generates random numbers between 0 and 14.
                                                                            // Adding 2 to this number produces numbers between 2 and 16.
                                                                            // Set enemy y between 2 and 16. [2,16]

    enemy.x = 32;                                                           // Set enemy x to 32
}




void SpawnBullet() {

    FIRE = FIRERATE / SPEED;                                                // FIRERATE represents the time in milliseconds per shot, while SPEED represents the speed of the game.
                                                                            // Thus, the fire variable determines the number of frames between shots.

    bullets[bulletCount].x = 6;                                             // I set a specific point, 6, where the bullet will fire in front of the player's character.
    bullets[bulletCount].y = playerY + 1;                                   // Adds 1 unit to the player's line (playerY), placing the bullet directly above the player's character.
    bulletCount++;                                                          // Ensures that each time a new bullet is created, it will be written to the next bullet index.
}




void DespawnBullet(int idx) {                                               // The value "idx" is used to determine which bullet is to be removed.

    for (int i = idx; i < bulletCount; ++i) {

        bullets[i] = bullets[i + 1];                                        // This line replaces the current bullet with the next bullet. The bullet at index i is replaced by the bullet at index i + 1.
                                                                            // The previous bullet is replaced by the next bullet and is thus deleted.
    }

    bulletCount--;                                                          // The total number of bullets is reduced by one.
}




int playerCollision() {

    if (enemy.x == 4  &&  enemy.y == playerY + 1) { return 1; }
    // It means that the enemy is in contact with the player's character.


    if (enemy.x == 3  &&  (enemy.y == playerY  ||  enemy.y == playerY + 2)) { return 1; }
    // It means that the enemy is in contact with the player's character.


    if (enemy.x == 1  &&  (enemy.y == playerY + 3  ||  enemy.y == playerY - 1))  { return 1; }
    // It means that the enemy is in contact with the player's character.


    return 0;
    // It means that the enemy is not in contact with the player's character.

}




int IsBullet(int x, int y) {

    for (int i = 0; i < bulletCount; i++) {                                 // If there are no bullets at the specified point, it advances step by step.

        if (bullets[i].x == x  &&  bullets[i].y == y) {

            return 1;
        }

    }

    return 0;
}




int main() {

    Start();

    while (TRUE) {

        Move();
        Draw();

        if (FIRE != 0) {

            FIRE--;             // If the fire is non-zero, subtract 1 from the fire variable to indicate that a bullet has been fired and the number of bullets has decreased.

        }

        Sleep(SPEED);           // Wait SPEED milliseconds.

    }

    Draw();

    return 0;                   // FINISH.

}
