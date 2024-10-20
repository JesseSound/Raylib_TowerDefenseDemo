#include <raylib.h>
#include <raygui.h>
#include "Math.h"

#include <cassert>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
constexpr float SCREEN_SIZE = 800;
constexpr int TILE_COUNT = 20;
constexpr float TILE_SIZE = SCREEN_SIZE / TILE_COUNT;

//don't have to pass to function this way
//Sound list
Sound damage{};
Sound death{};
Sound shot{};

//textures
Texture tower1{};
Texture gEnemyTexture1;
Texture gEnemyTexture2;
Texture gEnemyTexture3;
Texture gEnemyTexture4;
Texture gEnemyTexture5;
Texture enemyTexture;

// Ridiculous girl math
int levelInc = 0;



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
        { 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0 }, // 7
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 8
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 9
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 10
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 11
        { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 12
        { 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0 }, // 13
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // 14
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // 15
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }, // 16
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




//enum for what level we're in

enum Level  {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE
};
//level information structure before i cry

struct LevelInfo {

    Level currentLevel{}; 
    int maxEnemyCount{}; 
};


//just need a global variable for the spawndelay that we can alter
float spawnDelay = 0.0f;

// enum for gamestate for State Machine

enum GameState {
    PRE,
    GAMELOOP,
    END,
    SETUP
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

enum EnemyType : int {
    ENEMY,
    ENEMIER,
    ENEMIEST,
    ENEMIESTER,
    ENEMIESTIEST

};

//crazy ass function to swap colors of a texture???? why do I do these things to myself

Texture2D ChangeTextureColor(EnemyType enemyType, Texture2D enemyText) {

    //convert to image
    Image textToImage = LoadImageFromTexture(enemyText);
    //store an array of the colors in the image
    Color* imageColors = LoadImageColors(textToImage);

    //bit shift them to new colors
    for (int i = 0; i < textToImage.width * textToImage.height; i++) {
        Color color = imageColors[i];
        Color newColor = color;

        if (enemyType == ENEMY) {
            newColor.r = (color.r >> 5);
            newColor.g = (color.g << 5);
            newColor.b = (color.b << 5);
        }
        else if (enemyType == ENEMIER) {
            newColor.r = (color.r << 5);
            newColor.g = (color.g >> 5);
            newColor.b = (color.b << 5);
        }
        else if (enemyType == ENEMIEST) {
            newColor.r = (color.r >> 5);
            newColor.g = (color.g << 1);
            newColor.b = (color.b >> 3);
        }
        else if (enemyType == ENEMIESTER) {
            newColor.r = (color.r >> 3);
            newColor.g = (color.g >> 1);
            newColor.b = (color.b >> 1);
        }
        else if (enemyType == ENEMIESTIEST) {
            newColor.r = (color.r << 1);
            newColor.g = (color.g << 1);
            newColor.b = (color.b >> 2);
        }
        // Replace color only if it's not the same to avoid doing too much again lol
        if (color.r != newColor.r || color.g != newColor.g || color.b != newColor.b) {
            ImageColorReplace(&textToImage, color, newColor);
        }
    }

    //should be able to make a new texture based off the shit

    Texture2D newText = LoadTextureFromImage(textToImage);
    //unload shit to free memory
    UnloadImage(textToImage);
    UnloadImageColors(imageColors);
    //UnloadTexture(enemyText);
    //return our texture
    return newText;

}

struct Enemy {
    int health{};
    int damage{};
    float speed{};
    int pointValue{};
    Vector2 position{};
    int currWayp = 0;
    int nextWayp = currWayp + 1;
    float radius = 20.0f;
    bool alive = true;
    EnemyType type{};
    Color color{};
    Texture2D* texture = nullptr;
    //constructor to create unique enemies based on level
    Enemy(EnemyType enemyType) :type(enemyType) {
        typeInit();
    }

    void typeInit() {
        switch (type) {
            case ENEMY:
                health = 50;
                speed = 100.0f;
                pointValue = 5;
                damage = 5;
                color = RED;
                texture = &gEnemyTexture1;
                break;
            case ENEMIER:
                health = 80;
                speed = 120.0f;
                pointValue = 10;
                color = BLUE;
                damage = 10;
                texture = &gEnemyTexture2;
                break;
            case ENEMIEST:
                health = 100;
                speed = 150.0f;
                pointValue = 20;
                color = PURPLE;
                damage = 20;
                texture = &gEnemyTexture3;
                break;
            case ENEMIESTER:
                health = 110;
                speed = 170.0f;
                pointValue = 30;
                color = PURPLE;
                damage = 30;
                texture = &gEnemyTexture4;
                break;
            case ENEMIESTIEST:
                health = 200;
                speed = 80.0f;
                pointValue = 40;
                color = PURPLE;
                damage = 50;
                texture = &gEnemyTexture5;
                break;
            default:
                health = 30;
                speed = 100.0f;
                pointValue = 10;
                color = PURPLE;
                damage = 5;
                //texture = enemyTexture;
                break;
        }

    }
};
enum TurretType : int {
    BASIC, //add more later
    TURRETER,
    TURRETIEST
};



struct Turret {
    int damage = 10;
    int cost = 20;
    int upgradeCost = 25;
    float range = 250.0f;
    float rateOfFire = 0.6f;
    float shootTimer = 0.6f;
    
    TurretType type = BASIC;
    Vector2 location{};
    Enemy* target = nullptr;
    Texture2D turretTexture = tower1;
    
    Color turretBulletColor = BLUE;

    bool enabled = true; // remove_if to remove with right click
};


struct PlayerInfo { // a bit useless to start but may have some use down the road
    int coins = 100;
    int health = 50;
};


struct Bullet
{
    Vector2 position{};
    Vector2 direction{};
    float time = 0.0f;
    bool enabled = true;
     float bulletTime = 1.0f;
   float bulletSpeed = 700.0f;
     float bulletRadius = 15.0f;
     Color bulletColor = BLUE;
};

void EnemySpawning(std::vector<Enemy>& enemies, Vector2& enemyPosition, LevelInfo& levelInfo) {

    EnemyType enemyType{};
    switch (levelInfo.currentLevel) {
        case ONE:
            enemyType = ENEMY;
            break;
        case TWO:
            enemyType = ENEMIER;
            break;
        case THREE:
            enemyType = ENEMIEST;
            break;
        default:
            enemyType = ENEMY;
            break;
    }
    Enemy enemy(enemyType);
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


//bonus points for goto and inline assembly
//GameState Functions for StateMachine
//Pregame = pre game state. Should Use RAYGUI for a level selector, maybe take player name if we're feelign scandalousse
//Should switch state to GameLoop based on level select
void PreGame(GameState& gameState) {
    //empty fer now

    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    DrawText("Hi Connor! Welcome to our game.", 0, 0,50, RED);
    DrawText("Hit Space to start lol", 0, 100,50, RED);
    EndDrawing();


}



//Main game. Will take a lot of variables....
//should auto advance through levels as well
void GameLoop( Vector2& enemyPosition, std::vector<Enemy>& enemies, float& shootCurrent, std::vector<Turret>& turrets, float& shootTotal, std::vector<Bullet>& bullets, Bullet& bulletInfo, std::vector<Cell>& waypoints, GameState& gameState, LevelInfo& levelInfo,PlayerInfo& playerInfo ) {
    // TODO - Spawn 10 enemies
    float dt = GetFrameTime();
    spawnDelay += dt;
    
    
    
    CallSpawnLogic(enemyPosition, enemies, levelInfo);
    
    //clean this up
    shootCurrent += dt;
      // Change to inside loop
    for (Turret& turret : turrets) {
        turret.shootTimer += dt;  // Update the turret's individual shoot timer

        // Check if the turret's shoot timer exceeds its rate of fire
        if (turret.shootTimer >= turret.rateOfFire) {

            // Check if target is out of range or no enemies exist
            if (turret.target && (Distance(turret.target->position, turret.location) > turret.range || enemies.size() <= 0)) {
                turret.target = nullptr;
            }

            // If no target, find one within range
            if (!turret.target) {
                for (Enemy& enemy : enemies) {
                    if (Distance(enemy.position, turret.location) <= turret.range) {
                        turret.target = &enemy;
                        break;
                    }
                }
            }

            // Shoot at the current target, if there's one
            if (turret.target) {
                Bullet bullet;
                bullet.position = turret.location;
                bullet.direction = Normalize(turret.target->position + 30 - bullet.position);
                bullet.bulletColor = turret.turretBulletColor;
                bullets.push_back(bullet);
                PlaySound(shot);

                // Reset the turret's shooting timer after firing
                turret.shootTimer = 0.0f;
            }
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
                if (enemy.health == 0) {
                    PlaySound(death);
                    playerInfo.coins += enemy.pointValue; //  yay josh
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

    //Targeting system enemy removal handling
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
                if (enemy.nextWayp >= waypoints.size()) {
                    enemy.alive = false;
                    playerInfo.health -= enemy.damage;
                }
            }
        }
    }

    // Erase dead enemies
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy& enemy) {
        return !enemy.alive || enemy.health <= 0;
        }), enemies.end());

    // Render bullets
    for (const Bullet& bullet : bullets) {
        DrawCircleV(bullet.position, bulletInfo.bulletRadius, bullet.bulletColor);
    }




    //Keep Track of player coins
    
    DrawText(TextFormat("Player Coins: %i", playerInfo.coins), 10, 10, 20, BLUE);
    DrawText(TextFormat("Player Health: %i", playerInfo.health), 10, 30, 20, RED);
    DrawText(TextFormat("Remaining Enemies: %i", levelInfo.maxEnemyCount), 10, 60, 20, BLACK);

    // Render enemies
    for (const Enemy& enemy : enemies) {

        if (enemy.alive) {
            //Texture2D newTexture = ChangeTextureColor(enemy, enemyTexture); **don't uncomment
            //DrawCircleV(enemy.position, enemy.radius, enemy.color);

            DrawTextureV(*enemy.texture, { enemy.position.x - 15, enemy.position.y - 15 }, WHITE);
           
        }
    }

    // Render turrets
    for (const Turret& turret : turrets) {
        //if turret level = 0,1,2
        //Texture tower = 
        DrawTexture(turret.turretTexture, turret.location.x -31, turret.location.y -30, WHITE);
    }


    if (levelInfo.maxEnemyCount == 0 && enemies.size() == 0) {
        gameState = SETUP;
    }
    
    EndDrawing();
}

//After game is over. Maybe when All levels are done or player health <= 0 it goes back to Level select, or maybe it quickly goes to an info screen and gives option to choose new level.
void PostGame(PlayerInfo playerInfo) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Sorry You Died", 10, 10, 20, BLUE);
    DrawText(TextFormat("Coins Gathered: %i", playerInfo.coins), 10, 50, 20, BLUE);
   
    EndDrawing();
}

void Setup(PlayerInfo& playerInfo,  GameState& gameState, LevelInfo& level, std::vector<Turret>& turretArray)
{
   
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    //map draw
    for (int row = 0; row < TILE_COUNT; row++) {
        for (int col = 0; col < TILE_COUNT; col++) {
            DrawTile(row, col, tiles[row][col]);
        }
    }
    if (IsMouseButtonPressed(0)) {
        Vector2 pos = GetMousePosition();
        Turret turret;
      
        int tileX = (int)(pos.x/40);
        int tileY = (int)(pos.y/40);
        std::cout << tiles[tileY][tileX];
        std::cout << "Tile X: " << tileX;
        std::cout << "Tile Y: " << tileY;
       
        if (tiles[tileY][tileX] == GRASS && playerInfo.coins >= 10) {
            tiles[tileY][tileX] = TURRET;
           Turret newTurret;
           newTurret.location = TileCenter(tileY, tileX);
            playerInfo.coins -= newTurret.cost;
            turretArray.push_back(newTurret);
            
         } else  if (tiles[tileY][tileX] == TURRET && playerInfo.coins >= turret.upgradeCost) {
             
             for (int i = 0; i < turretArray.size(); i++) {
                 if (Equals(turretArray[i].location, TileCenter(tileY, tileX))) {

                     switch (turretArray[i].type) {
                     case BASIC:
                         std::cout << "YES";
                         turretArray[i].rateOfFire = 0.4f;
                         turretArray[i].damage= 20;
                         turretArray[i].cost = 20;
                         turretArray[i].type = TURRETER;
                         turretArray[i].turretBulletColor = DARKBLUE;
                         playerInfo.coins -= 25;

                         break;
                     case TURRETER:
                         std::cout << "TURRETTTERIEST";
                         turretArray[i].rateOfFire = 0.2f;
                         turretArray[i].cost = 30;
                         turretArray[i].damage = 30;
                         turretArray[i].type = TURRETIEST;
                         turretArray[i].turretBulletColor = RED;
                         playerInfo.coins -= 25;
                         break;
                     case TURRETIEST:
                         std::cout << "CANT UPGRADE FURTHER";
                         turretArray[i].rateOfFire = 0.2f;
                         turretArray[i].cost = 40;
                         turretArray[i].damage = 40;
                         turretArray[i].turretBulletColor = DARKPURPLE;
                         break;
                     default:
                         std::cout << "YES";
                         turretArray[i].rateOfFire = 0.4f;
                         turretArray[i].cost = 20;
                         turretArray[i].type = TURRETER;
                         playerInfo.coins -= 25;
                         break;
                     }
             }
             }
         }
        
    }
    if (IsMouseButtonPressed(1)) {
        Vector2 pos = GetMousePosition();


        int tileX = (int)(pos.x / 40);
        int tileY = (int)(pos.y / 40);
        std::cout << tiles[tileY][tileX];
        std::cout << "Tile X: " << tileX;
        std::cout << "Tile Y: " << tileY;

        if (tiles[tileY][tileX] == TURRET) {
            tiles[tileY][tileX] = GRASS;
            playerInfo.coins += 5;
            for (int i = 0; i < turretArray.size(); i++) {
                if (Equals(turretArray[i].location, TileCenter(tileY, tileX))) {
                    turretArray[i].enabled = false;
                }
            }
        }
    }
    if (IsKeyPressed(KEY_A)) {
       
        switch (levelInc) {
            case 0:
                level.currentLevel = ONE;
                levelInc += 1;
                level.maxEnemyCount = 10;
                gameState = GAMELOOP;
                break;
            case 1:
                level.currentLevel = TWO;
                levelInc += 1;
                level.maxEnemyCount = 20;
                gameState = GAMELOOP;
                
                break;
            case 2:
                level.currentLevel = THREE;
                levelInc += 1;
                level.maxEnemyCount = 30;
                gameState = GAMELOOP;

                break;
            case 3:
                level.currentLevel = FOUR;
                levelInc += 1;
                level.maxEnemyCount = 40;
                gameState = GAMELOOP;
                break;
            case 4:
                level.currentLevel = FIVE;
                levelInc += 1;
                level.maxEnemyCount = 50;
                gameState = GAMELOOP;
                break;
            case 5:
                gameState = END;
                break;
            default:
                gameState = PRE;
                break;
        }
    }
    for (const Turret& turret : turretArray) {
        //if turret level = 0,1,2
        //Texture tower = 
        DrawTexture(turret.turretTexture, turret.location.x - 30 , turret.location.y - 30, WHITE);
    }
    DrawText(TextFormat("Player Coins: %i", playerInfo.coins), 10, 50, 20, BLUE);
    DrawText(TextFormat("Player Health: %i", playerInfo.health), 10, 20, 20, RED);
    EndDrawing();
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
    enemyTexture = LoadTexture("src/enemyTexture.png"); //self made by Jesse

    gEnemyTexture1 = ChangeTextureColor(ENEMY, enemyTexture);
    gEnemyTexture2 = ChangeTextureColor(ENEMIER, enemyTexture);
    gEnemyTexture3 = ChangeTextureColor(ENEMIEST, enemyTexture);
    gEnemyTexture3 = ChangeTextureColor(ENEMIESTER, enemyTexture);
    gEnemyTexture3 = ChangeTextureColor(ENEMIESTIEST, enemyTexture);
       


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

   
    //init with 0 level for testing
    
    LevelInfo levelInfo{};
    levelInfo.currentLevel = ZERO;
    levelInfo.maxEnemyCount = (levelInfo.currentLevel + 1) * 10;
    
    //Init PlayerInfo
    PlayerInfo playerInfo{};
    //Turret   turretCost{};

     
    


    while (!WindowShouldClose())
    {
        std::cout << turrets.size();
        //logic for switching to Postgame based on player health

        if (playerInfo.health <= 0) {
            gameState = END;
        }

        switch (gameState) {
            case PRE:
                
                PreGame(gameState);
                break;
            case GAMELOOP:
                
                GameLoop(enemyPosition, enemies, shootCurrent, turrets, shootTotal, bullets, bulletInfo,  waypoints, gameState, levelInfo, playerInfo );
                break;
            case END:
                PostGame(playerInfo);
                break;
            case SETUP:
                Setup(playerInfo, gameState, levelInfo, turrets);
                break;

            default:
                gameState = PRE; // safety net
                break;

        }

         
        if (IsKeyPressed(KEY_SPACE) && gameState == PRE) {
            gameState = SETUP;
        }

        //removing turrets from array 
         // Erase dead enemies
        turrets.erase(std::remove_if(turrets.begin(), turrets.end(), [](Turret& turret) {
            return !turret.enabled;
            }), turrets.end());


    }
    // can store pointers to texture, 8 byte address per enemy
    UnloadTexture(enemyTexture);
    UnloadTexture(gEnemyTexture1);
    UnloadTexture(gEnemyTexture2);
    UnloadTexture(gEnemyTexture3);
    UnloadTexture(gEnemyTexture4);
    UnloadTexture(gEnemyTexture5);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}