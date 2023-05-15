#include <iostream>
#include "raylib.h"
using namespace std;

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define MAX_TREES 100
#define DINO_RADIUS 20
#define TREES_WIDTH 32

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Dino
{
    Vector2 position;
    int radius;
    Color color;
} Dino;

typedef struct Trees
{
    Rectangle rec;
    Color color;
    bool active;
} Trees;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 1200;
static const int screenHeight = 480;

static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int hiScore = 0;

static Dino dino = {0};
static Trees trees[MAX_TREES * 2] = {0};
static Vector2 treesPos[MAX_TREES] = {0};
static float treeSpeedX = 0;
static bool superfx = false;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Enes game");
    InitAudioDevice();

    InitGame();


    SetTargetFPS(60);

    Music background_music = LoadMusicStream("../assets/audio/background_music.mp3");
    PlayMusicStream(background_music);

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        UpdateMusicStream(background_music);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    CloseAudioDevice(); // Close audio device
    //--------------------------------------------------------------------------------------
    UnloadGame(); // Unload loaded data (textures, sounds, models...)

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
// Initialize game variables
void InitGame(void)
{
    dino.radius = DINO_RADIUS;
    dino.position = {80, screenHeight - 100};
    dino.color = WHITE;

    treeSpeedX = 6;
    for (int i = 0; i < MAX_TREES; i++)
    {
        treesPos[i].x = 800 * i + GetRandomValue(400, 800);
        treesPos[i].y = -GetRandomValue(60, 100);
        cout << "MF:" << treesPos[i].x << endl;
    }

    cout << "---------------------------------" << endl;

    for (int i = 0; i < MAX_TREES * 2; i += 2)
    {
        trees[i + 1].rec.x = treesPos[i / 2].x;
        trees[i + 1].rec.y = 600 + treesPos[i / 2].y - 200;
        trees[i + 1].rec.width = TREES_WIDTH;
        trees[i + 1].rec.height = 200;

        trees[i / 2].active = true;

        cout << trees[i + 1].rec.x << endl
             << trees[i + 1].rec.y << endl
             << endl;
    }

    score = 0;
    gameOver = false;
    superfx = false;
    pause = false;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P'))
            pause = !pause;

        if (!pause)
        {
            for (int i = 0; i < MAX_TREES; i++) // Move trees
                treesPos[i].x -= treeSpeedX;

            for (int i = 0; i < MAX_TREES * 2; i += 2) // Move colliders
            {
                trees[i].rec.x = treesPos[i / 2].x;
                trees[i + 1].rec.x = treesPos[i / 2].x;
            }

            if (IsKeyPressed(KEY_SPACE) && !gameOver && dino.position.y >= 280)
                dino.position.y -= 200; // Jump

            else if (dino.position.y < screenHeight - 100)
                dino.position.y += 5;

            // Check Collisions
            for (int i = 0; i < MAX_TREES * 2; i++)
            {
                if (CheckCollisionCircleRec(dino.position, dino.radius, trees[i].rec))
                {
                    gameOver = true;
                    pause = false;
                }
                else if ((treesPos[i / 2].x < dino.position.x) && trees[i / 2].active && !gameOver)
                {
                    score += 100;
                    if (treeSpeedX >= 14)
                        treeSpeedX += 0.05;
                    else
                        treeSpeedX += 0.7;
                    cout << "TSX: " << treeSpeedX << endl;

                    trees[i / 2].active = false;

                    superfx = true;

                    if (score > hiScore)
                        hiScore = score;
                }
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

    ClearBackground(BLUE);

    if (!gameOver)
    {
        DrawCircle(dino.position.x, dino.position.y, dino.radius, dino.color);
        DrawRectangle(0, screenHeight - 100 + dino.radius, screenWidth, 100, GREEN);
        // // Draw trees
        for (int i = 0; i < MAX_TREES; i++)
        {
            DrawRectangle(trees[i * 2].rec.x, trees[i * 2].rec.y, trees[i * 2].rec.width, trees[i * 2].rec.height, BLACK);
            DrawRectangle(trees[i * 2 + 1].rec.x, trees[i * 2 + 1].rec.y, trees[i * 2 + 1].rec.width, trees[i * 2 + 1].rec.height, BLACK);
        }

        // Draw flashing fx (one frame only)
        if (superfx)
        {
            // DrawRectangle(0, 0, screenWidth, screenHeight, BLACK); TODO : Sound effect here
            superfx = false;
        }

        DrawText(TextFormat("%04i", score), 20, 20, 40, WHITE);
        DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, WHITE);
        DrawFPS(20,100);

        if (pause)
            DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40, 40, BLACK);
    }
    else
        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, WHITE);

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}