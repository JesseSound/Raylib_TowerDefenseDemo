#include <raylib.h>
#include <raygui.h>
#include "Math.h"

#include <cassert>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
const float SCREEN_SIZE = 800;
const int TILE_COUNT = 20;
const float TILE_SIZE = SCREEN_SIZE / TILE_COUNT;

//don't have to pass to function this way
//Sound list
Sound damage{};
Sound death{};
Sound shot{};

//textures
Texture tower1{};

//TO DO: Make Each Level Unique
//These Levels are GLOBAL VARIABLES
int tiles[TILE_COUNT][TILE_COUNT]
{
    //col:0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    row:
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 }, // 0
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 1
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 2
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 3
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 4
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 5
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, // 6
        { 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 4, 0, 0, 0, 0, 0 }, // 7
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 8
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 9
        { 0, 4, 0, 1, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 10
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 11
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 12
        { 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0 }, // 13
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // 14
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 4, 0 }, // 15
        { 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // 16
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0 }, // 17
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 18
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }  // 19
};

                                                                           
enum TileType : int
{
    GRASS,      // Marks unoccupied space, can be overwritten 
    DIRT,       // Marks the path, cannot be overwritten
    WAYPOINT,   // Marks where the path turns, cannot be overwritten
    COUNT,
    TURRET
};

struct Cell
{
    int row;
    int col;
};


enum TurretType : int {
    BASIC //add more later
};

//enum for what level we're in

enum Level : int {
    ONE,
    TWO,
    THREE
};
//level information structure before i cry

struct LevelInfo {

    Level currentLevel = ONE;
    int maxEnemyCount = (currentLevel +1) * 10; //fucking lol 
};


//just need a global variable for the spawndelay that we can alter
float spawnDelay = 0.0f;

// enum for gamestate for State Machine

enum GameState {
    PRE,
    GAMELOOP,
    END
};







constexpr std::array<Cell, 4> DIRECTIONS{ Cell{ -1, 0 }, Cell{ 1, 0 }, Cell{ 0, -1 }, Cell{ 0, 1 } };

inline bool InBounds(Cell cell, int rows = TILE_COUNT, int cols = TILE_COUNT)
{
    return cell.col >= 0 && cell.col < cols && cell.row >= 0 && cell.row < rows;
}

void DrawTile(int row, int col, Color color)
{
    DrawRectangle(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
}

void DrawTile(int row, int col, int type)
{
    Color color;
    if (type == 4) {
        color = DARKGREEN;
    }
    else {
        color = type < 1 ? GREEN : BEIGE;
    }
    DrawTile(row, col, color);
}

Vector2 TileCenter(int row, int col)
{
    float x = col * TILE_SIZE + TILE_SIZE * 0.5f;
    float y = row * TILE_SIZE + TILE_SIZE * 0.5f;
    return { x, y };
}

Vector2 TileCorner(int row, int col)
{
    float x = col * TILE_SIZE;
    float y = row * TILE_SIZE;
    return { x, y };
}

// Returns a collection of adjacent cells that match the search value.
std::vector<Cell> FloodFill(Cell start, int tiles[TILE_COUNT][TILE_COUNT], TileType searchValue)
{
    // "open" = "places we want to search", "closed" = "places we've already searched".
    std::vector<Cell> result;
    std::vector<Cell> open;
    bool closed[TILE_COUNT][TILE_COUNT];
    for (int row = 0; row < TILE_COUNT; row++)
    {
        for (int col = 0; col < TILE_COUNT; col++)
        {
            // We don't want to search zero-tiles, so add them to closed!
            closed[row][col] = tiles[row][col] == 0;
        }
    }

    // Add the starting cell to the exploration queue & search till there's nothing left!
    open.push_back(start);
    while (!open.empty())
    {
        // Remove from queue and prevent revisiting
        Cell cell = open.back();
        open.pop_back();
        closed[cell.row][cell.col] = true;

        // Add to result if explored cell has the desired value
        if (tiles[cell.row][cell.col] == searchValue)
            result.push_back(cell);

        // Search neighbours
        for (Cell dir : DIRECTIONS)
        {
            Cell adj = { cell.row + dir.row, cell.col + dir.col };
            if (InBounds(adj) && !closed[adj.row][adj.col] && tiles[adj.row][adj.col] > 0)
                open.push_back(adj);
        }
    }

    return result;
}


struct Enemy {
    int health = 30;
    float speed = 200.0f;
    int pointValue = 1;
    Vector2 position{};
    int currWayp = 0;
    int nextWayp = currWayp + 1;
    float radius = 20.0f;
    bool alive = true;
    
};

struct Turret {
    int damage = 10;
    int cost = 10;
    float range = 250.0f;
    float rateOfFire = 0.6f;
    TurretType type = BASIC;
    Vector2 location{};
    Enemy* target = nullptr;
    
};


struct PlayerInfo { // a bit useless to start but may have some use down the road
    int coins = 50;
    int health = 50;
};


struct Bullet
{
    Vector2 position{};
    Vector2 direction{};
    float time = 0.0f;
    bool enabled = true;
     float bulletTime = 1.0f;
   float bulletSpeed = 500.0f;
     float bulletRadius = 15.0f;
};

void EnemySpawning(std::vector<Enemy>& enemies, Vector2& enemyPosition, LevelInfo& levelInfo) {

    
    Enemy enemy;
    enemy.position = enemyPosition;
    enemies.push_back(enemy);
    levelInfo.maxEnemyCount -= 1;
}


void CallSpawnLogic(Vector2& enemyPosition, std::vector<Enemy>& enemies, LevelInfo& levelInfo) {

    if (spawnDelay >= 1.0f && levelInfo.maxEnemyCount > 0) {
        spawnDelay = 0.0f;
        EnemySpawning(enemies,  enemyPosition, levelInfo);
    }
}

void ReDrawTurrets(int tiles[TILE_COUNT][TILE_COUNT], std::vector<Turret>& turrets) { //hehe
    for (int row = 0; row < TILE_COUNT; row++) {
        for (int col = 0; col < TILE_COUNT; col++) {
            if (tiles[row][col] == TURRET) {
                Turret turret;

                turret.location = TileCenter(row, col);

                turrets.push_back(turret);
            }
        }
    }
}




//GameState Functions for StateMachine
//Pregame = pre game state. Should Use RAYGUI for a level selector, maybe take player name if we're feelign scandalousse
//Should switch state to GameLoop based on level select
void PreGame(GameState& gameState) {
    //empty fer now

    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    DrawText("Hi Josh! Welcome to our game.", 0, 0,50, RED);
    DrawText("Hit Space to start lol", 0, 100,50, RED);
    EndDrawing();


}


//Main game. Will take a lot of variables....
//should auto advance through levels as well
void GameLoop( Vector2& enemyPosition, std::vector<Enemy>& enemies, float& shootCurrent, std::vector<Turret>& turrets, float& shootTotal, std::vector<Bullet>& bullets, Bullet& bulletInfo, std::vector<Cell>& waypoints, GameState& gameState, LevelInfo& levelInfo ) {
    // TODO - Spawn 10 enemies
    float dt = GetFrameTime();
    spawnDelay += dt;

    

    
    CallSpawnLogic(enemyPosition, enemies, levelInfo);
    
    //clean this up
    shootCurrent += dt;
    if (shootCurrent >= shootTotal) {  // Change to inside loop
        for (Turret& turret : turrets) {

            // Check if target is out of range
            //absolute bullshit way of doing this
            //please dont fail me for not using find_if
            if (turret.target && Distance(turret.target->position, turret.location) > turret.range) {
                turret.target = nullptr;

            }

            // If no target, find one
            if (!turret.target) {
                for (Enemy& enemy : enemies) {
                    if (Distance(enemy.position, turret.location) <= turret.range) {
                        turret.target = &enemy;
                        break;
                    }
                }
            }

            // Shoot at the current target
            //still maintains part of your code, yeah?
            if (turret.target) {
                Bullet bullet;
                bullet.position = turret.location;
                bullet.direction = Normalize(turret.target->position - bullet.position);
                bullets.push_back(bullet);
                PlaySound(shot);

            }

            // Reset shooting timers
            shootCurrent = 0.0f;
            shootTotal = turret.rateOfFire;

        }
    }

    // TODO - Loop through all bullets & enemies, handle collision & movement accordingly
    for (Bullet& bullet : bullets) {

        bullet.position = bullet.position + bullet.direction * bulletInfo.bulletSpeed * dt;
        bullet.time += dt;
        bool collision = false;
        bool expired = bullet.time >= bulletInfo.bulletTime;
        // bool collision = CheckCollisionCircles(enemyPosition, enemyRadius, bullet.position, bulletRadius); //largely useless but keeping for reference

        for (Enemy& enemy : enemies) {

            collision = CheckCollisionCircles(enemy.position, enemy.radius, bullet.position, bulletInfo.bulletRadius);
            if (collision) {
                std::cout << "coll";

                enemy.health -= 10;
                if (enemy.health <= 0) {
                    PlaySound(death);
                    enemy.alive = false;
                }
                else {
                    PlaySound(damage);
                }
                bullet.enabled = false;
                break;
            }

        }
        bullet.enabled = !expired && !collision;
    }

    //Targeting system enemy removal handling???
    for (Turret& turret : turrets) {
        for (Enemy& enemy : enemies) {
            if (!enemy.alive) {
                turret.target = nullptr;
            }
        }
    }




    // Bullet removal
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](Bullet bullet) {
            return !bullet.enabled;
        }), bullets.end());

    BeginDrawing();
    ClearBackground(BLACK);


    for (int row = 0; row < TILE_COUNT; row++) {
        for (int col = 0; col < TILE_COUNT; col++) {
            DrawTile(row, col, tiles[row][col]);
        }
    }

    // moved pathfinding to here
    for (Enemy& enemy : enemies) {
        bool atEnd = enemy.nextWayp >= waypoints.size();
        if (!atEnd) {
            Vector2 from = TileCenter(waypoints[enemy.currWayp].row, waypoints[enemy.currWayp].col);
            Vector2 to = TileCenter(waypoints[enemy.nextWayp].row, waypoints[enemy.nextWayp].col);
            Vector2 direction = Normalize(to - from);
            enemy.position = enemy.position + direction * enemy.speed * dt;

            if (CheckCollisionPointCircle(enemy.position, to, enemy.radius)) {
                enemy.currWayp++;
                enemy.nextWayp++;
                atEnd = enemy.nextWayp == waypoints.size();
                enemy.position = TileCenter(waypoints[enemy.currWayp].row, waypoints[enemy.currWayp].col);
                if (enemy.nextWayp >= waypoints.size())
                    enemy.alive = false;
            }
        }
    }

    // Erase dead enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy& enemy) {
        return !enemy.alive || enemy.health <= 0;
        }), enemies.end());

    // Render bullets
    for (const Bullet& bullet : bullets)
        DrawCircleV(bullet.position, bulletInfo.bulletRadius, BLUE);

    DrawText(TextFormat("Total bullets: %i", bullets.size()), 10, 10, 20, BLUE);

    // Render enemies
    for (const Enemy& enemy : enemies) {
        if (enemy.alive)
            DrawCircleV(enemy.position, enemy.radius, RED);
    }

    // Render turrets
    for (const Turret& turret : turrets) {
        DrawTexture(tower1, turret.location.x -31, turret.location.y -30, WHITE);
    }

    
    EndDrawing();
}

//After game is over. Maybe when All levels are done or player health <= 0 it goes back to Level select, or maybe it quickly goes to an info screen and gives option to choose new level.
void PostGame() {

}





int main()
{
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Tower Defense - By Josh And Jesse");
    SetTargetFPS(60);
    InitAudioDevice();

    //assign audio and textures
    damage = LoadSound("src/tick.mp3"); //https://pixabay.com/sound-effects/retro-hurt-2-236675/
    death = LoadSound("src/damage.mp3"); //https://pixabay.com/sound-effects/hurt-c-08-102842/
    shot = LoadSound("src/turretShot.mp3"); //https://pixabay.com/sound-effects/086409-retro-gun-shot-81545/
   
    tower1 = LoadTexture("src/towerlvl1.png"); //https://opengameart.org/content/tower-defense


    //set initial gamestate to pregame
    GameState gameState = PRE;

   



    std::vector<Cell> waypoints = FloodFill({ 0, 12 }, tiles, WAYPOINT);
    size_t curr = 0;
    size_t next = curr + 1;
    bool atEnd = false;

    Vector2 enemyPosition = TileCenter(waypoints[curr].row, waypoints[curr].col);
    

    //Generate a bullet info object to access later
    Bullet bulletInfo{};

    std::vector<Bullet> bullets;
    float shootCurrent = 0.0f;
    float shootTotal = 0.05f;

    

    std::vector<Enemy> enemies;
    std::vector<Turret> turrets;

    for (int row = 0; row < TILE_COUNT; row++) {
        for (int col = 0; col < TILE_COUNT; col++) {
            if (tiles[row][col] == TURRET) {
                Turret turret;

                turret.location = TileCenter(row, col);

                turrets.push_back(turret);
            }
        }
    }

    //Precursor to level select logic 
    
    LevelInfo levelInfo{};
   

    bool canModify = false;


    while (!WindowShouldClose())
    {
       
       

        switch (gameState) {
            case PRE:
                canModify = false;
                PreGame(gameState);
                break;
            case GAMELOOP:
                canModify = true;
                GameLoop(enemyPosition, enemies, shootCurrent, turrets, shootTotal, bullets, bulletInfo,  waypoints, gameState, levelInfo );
                break;
            case END:
                canModify = false;
                break;
            default:
                gameState = PRE; // safety net
                break;

        }

         
        if (IsKeyPressed(KEY_SPACE) && gameState == PRE) {
            gameState = GAMELOOP;
        }
    }
    // can store pointers to texture, 8 byte address per enemy
    CloseAudioDevice();
    CloseWindow();
    return 0;
}