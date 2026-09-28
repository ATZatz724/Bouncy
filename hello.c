#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*==============================================================================
||                          CALCULATIONS & CONSTANTS                          ||
==============================================================================*/
#define BASE_W 1280                   
#define BASE_H 720                    
#define GRAVITY 1500.0f            
#define BOUNCINESS 0.4f           
#define BOUNCE_STOP 150.0f            
#define MAX_ACC 900.0f                
#define MAX_SPEED 260.0f              
#define MAX_FALL_SPEED 1200.0f

#define WATER_DRAG         0.99f    
#define WATER_GRAVITY      120.0f   
#define WATER_BUOYANCY     400.0f   
#define WATER_MAX_FALL     150.0f   
#define WATER_SWIM_IMPULSE -180.0f

#define FRICTION 0.9f                 
#define JUMP -650.0f                  
#define SPRING_JUMP -900.0f           
#define SPIDER_SPEED 60.0f            
#define MAX_SPIDERS 16                
#define MAX_LEN 16                    
#define SCORES "highscore.txt"       
#define MAX_SCORES 100                

#define TILE_SIZE 32.0f               
#define LEVEL_ONE_ROW 8               
#define LEVEL_ONE_COL 105             
#define LEVEL_TWO_ROW 22              
#define LEVEL_TWO_COL 149             
#define LEVEL_THREE_ROW 36            
#define LEVEL_THREE_COL 134           

/*==============================================================================
||                          OBJECTS && STRUCTURES                             ||
==============================================================================*/
typedef Vector2 v2;


typedef enum BallType{
    Ball_normal,
    Ball_deflated,
    Ball_pumped
}BallType;


typedef struct Ball{
    v2 position;
    v2 velocity;
    float radius;
    float rotation;
    bool grounded;
    BallType type;
    Texture2D texture;
}Ball;


typedef struct ScoreRecord{
    char name[MAX_LEN+1];
    int score;
}ScoreRecord;





typedef enum Gamestate{
    PLAYING,
    DEAD,
    WIN,
    PAUSED,
    MAIN_MENUE,
    LEVEL_SELECT,
    NAME_INPUT,
    HOW_TO_PLAY,
    LEADERBOARD,
    CREDITS
}Gamestate;


typedef struct Levelasset{
    Texture2D brick;
    Texture2D spike;
    Texture2D spring;
    Texture2D ring;
    Texture2D goal;
    Texture2D pumper;
}Levelasset;

/*==============================================================================
||                                 SPIDERS                                    ||
==============================================================================*/
typedef struct spider{
    Rectangle rect;
    float speed;
}spider;

//spider position
#define SPIDER_AT(col, row) { .rect = { (col)*TILE_SIZE + 2, (row)*TILE_SIZE + 2, 2*TILE_SIZE - 2, 2*TILE_SIZE - 2 }, .speed = SPIDER_SPEED }


const spider level_two_spiders[] = {
    SPIDER_AT(2, 15), SPIDER_AT(4, 15), SPIDER_AT(51, 15), SPIDER_AT(57, 15),
    SPIDER_AT(63, 15), SPIDER_AT(97, 15), SPIDER_AT(91, 15)
};

const spider level_three_spiders[] = {
    SPIDER_AT(27,3),SPIDER_AT(40,3),SPIDER_AT(50,8),SPIDER_AT(56,8)
};


spider active_spiders[MAX_SPIDERS];
int spider_count = 0;

/*==============================================================================
||                               LEVELMAPS                                    ||
==============================================================================*/
const char *level_one_map[LEVEL_ONE_ROW]={
    "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
    "110001111110000000000111100000000011000111000000000000000000111000000110000001100000000000000110000000011",
    "110001111110000000000111100000000000000110000000000000000000011000000110000001100000000000000110000000011",
    "110001111110001100000111100000000000000110000000000000000000011001100110011001100110000000000110000000011",
    "110001111110001100000111100111100111100110000000000000000000011001100110011001100110000000114114110000011",
    "110000000000001100000000000110000001100000000000000000000000000001100400011004000110000112110000112110055",
    "110000000400001102000000000110020001100000000020020002000200000001100000011000000110000111110000111110055",
    "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
};

const char *level_two_map[LEVEL_TWO_ROW] = {
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111111111111111",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000400000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111110000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000055",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000055",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110001111111111111111111",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111100000011001100000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000110000110000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000001100000011000011",
    "11000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000011000200001100011",
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111114011",
    "11000000000000000000004000110000001100000400000000000000000000000110000000000000000000011000000000000004000000000011000000000400011000000000110000011",
    "11000000000000000000000000110000001100000000000000000000000000000110000000000000000000011000000000000000000000000011000000000000011000000000110000011",
    "11000000000000011111111100110011000000000111100000000000000000000110000000001100000000040000000000000001100000000040000000000110011000011000110011111",
    "11000000000000011000001100110011000000000111100000000000000000000110000000011110000000000000000000000001100000000000000000000110011001111000000000011",
    "11000000000000011000000000000011000000000111100000000000000000000400000000111111000000011000000000000001100000000011000000000110000000011000000000011",
    "11000000000002011000000000000011000000000111100000000000000000000000000001111111100000011000000000000001100000000011000000000110000000011000000002011",
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
};

const char *level_three_map[LEVEL_THREE_ROW] = { 
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
    "11404000001120000000000000000000000000000001110000000000001100000011100000011111110000000000200000000000000000000000000000000000000011",
    "11000000001100000000000000000000000000000000110000000000001100000011000000001111100000000000000000000000000000001100110000000000000011",
    "11000000001100000000001100000000000000000000000000000000001100000000000000000000000000000000000000000000000000000000000000011000000011",
    "11000000001110000000011100000000000000000000000000000000001100000000000000000000000000000000000000000000000000000011000000000000110011",
    "11000000001111000000111100000000000000000000110000000000015500000011000000001111100000001100000001111111111111110000000110000001100011",
    "11000000001111100001111100000000000000000001110000000000115500000011100000011111110000011110000011111111111111110000000000114400004011",
    "11111000001111110011111111111111111111111111111111111111111111111111111111111111111111111111666111111111111111111111111111111111111111",
    "11000000000000000000000000000001100001100000000000004000004000011111104001111111111111111111666110000000000000110000000000000000000000",
    "11000000000000000000000000000001100001100000000000000000000000001111000000111111111111111111666110000000000000110000000000000000000000",
    "11000011000000000000000000000000000000000000000000000000000000000111000000111111111111111111666110000000000000110000000000000000000000",
    "11000040001166666666661111000000000000000000110000000000000000000011000000111111111111111111666110000000000000110000000000000000000000",
    "11000000001116666666611111000001100001100000111000000000000000000000000000111111111111111111666110000000000000110000000000000000000000",
    "11000000001111666666111111000001100001140004111100000004000000000000000000111111111111111111666110000000000000110000000000000000000000",
    "11111111111111111111111111111111100001111111111111111111111111111111000000111111111111111111666110000000000000110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111666111000000001111110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111666111000000001111110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111666110000000000111110000000000000000000000",
    "00000000000000000000001133333333333333330000110000000000000000000011100000111111111111111111666110000000000011110000000000000000000000",
    "00000000000000000000001120000000000000000000110000000000000000000011100000111111111111111111666111000000000001110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111000111100000000000110000000000000000000000",
    "00000000000000000000001111111111111111111111110000000000000000000011100000111111111111111111000111110000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111110000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666111100000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666111000000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666110000000000011110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666111111111111111111111111111111111111111",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666110000000000000110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666110000000000000110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666110000000000000110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111666111111111111111110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000666666666666666666666666666666666666660000000000200000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000666666666666666666666666666666666666660000000000110000007011",
    "00000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111111",
};


const char **levels[3] = {level_one_map, level_two_map, level_three_map};


char map1[LEVEL_ONE_ROW][LEVEL_ONE_COL];
char map2[LEVEL_TWO_ROW][LEVEL_TWO_COL];
char map3[LEVEL_THREE_ROW][LEVEL_THREE_COL];
char *maps[3] = {&map1[0][0], &map2[0][0], &map3[0][0]};

/*==============================================================================
||                          LEVEL BOARD & VARIABLES                          ||
==============================================================================*/
typedef struct LevelInfo{
    int rows;
    int cols;
    v2 start;
    const spider *spiders;
    int spiderCount;
}LevelInfo;


const LevelInfo level_info[3] = {
    { LEVEL_ONE_ROW,   LEVEL_ONE_COL,   { 2.5f*TILE_SIZE, 1.5f*TILE_SIZE }, NULL, 0 },
    { LEVEL_TWO_ROW,   LEVEL_TWO_COL,   { 2.5f*TILE_SIZE + 160, 13.5f*TILE_SIZE + 100 },
      level_two_spiders, (int)(sizeof(level_two_spiders)/sizeof(level_two_spiders[0])) },
    { LEVEL_THREE_ROW, LEVEL_THREE_COL, { 38.5f*TILE_SIZE, 1.5f*TILE_SIZE }, 
    level_three_spiders, (int)(sizeof(level_three_spiders)/sizeof(level_three_spiders[0])) },
};

int current_level = 1;
int current_row = LEVEL_ONE_ROW;
int current_col = LEVEL_ONE_COL;


int score = 0;
bool scoresaved = false;

bool coin_collected = false;   
bool ball_popped = false;      
bool level_passed = false;     

/*==============================================================================
||                          TILING & LEVEL RESET                               ||
==============================================================================*/
char GetTile(int r, int c){
    if(r < 0 || r >= current_row || c < 0 || c >= current_col) return '1';
    return maps[current_level - 1][r * current_col + c];
}


void SetTile(int r, int c, char t){
    if(r < 0 || r >= current_row || c < 0 || c >= current_col) return;
    maps[current_level - 1][r * current_col + c] = t;
}


void Reset(Ball *b, Gamestate *state){
    const LevelInfo *L = &level_info[current_level - 1];
    current_row = L->rows;
    current_col = L->cols;

    b->position = L->start;
    b->velocity = (v2){0.0f, 0.0f};
    b->rotation = 0.0f;
    b->grounded = false;

    for(int r = 0; r < current_row; r++){
        for(int c = 0; c < current_col; c++){
            maps[current_level - 1][r * current_col + c] = levels[current_level - 1][r][c];
        }
    }

    spider_count = (L->spiderCount < MAX_SPIDERS) ? L->spiderCount : MAX_SPIDERS;
    for(int i = 0; i < spider_count; i++){
        active_spiders[i] = L->spiders[i];
    }

    score = 0;
    scoresaved = false;
    *state = PLAYING;
}

/*==============================================================================
||                                  BALL PHYSICS                               ||
==============================================================================*/
void BallPlatformCollision(Ball *b, Rectangle tilerect, char tiletype, Gamestate *state){
    float ClosestX = Clamp(b->position.x, tilerect.x, tilerect.x + tilerect.width);
    float ClosestY = Clamp(b->position.y, tilerect.y, tilerect.y + tilerect.height);

    float dx = b->position.x - ClosestX;
    float dy = b->position.y - ClosestY;

    float dis_sq = dx*dx + dy*dy;
    if(b->radius*b->radius <= dis_sq){
        return;
    }
    float dis = sqrtf(dis_sq);
    v2 normal = (dis > 0.00001f) ? (v2){dx/dis, dy/dis} : (v2){0, -1};
    float penetration = b->radius - dis;
    b->position.x += normal.x*penetration;
    b->position.y += normal.y*penetration;

    if(tiletype == '2') {*state = DEAD; ball_popped = true; return;}
    if(tiletype == '5') {*state = WIN; level_passed = true; return;}

    float Normalvel = b->velocity.x*normal.x + b->velocity.y*normal.y;

    if(Normalvel < 0){
        if(tiletype == '3' && normal.y < -0.5f){
            b->velocity.y = SPRING_JUMP;
        }else{

            float bounce = (-Normalvel > BOUNCE_STOP) ? BOUNCINESS : 0.0f;
            b->velocity.x -= (1 + bounce)*Normalvel*normal.x;
            b->velocity.y -= (1 + bounce)*Normalvel*normal.y;
        }
    }

    if(normal.y < -0.5f){
        b->grounded = true;
    }
}

/*==============================================================================
||                          Check if it is in water                            ||
==============================================================================*/

bool IsSubmerged(Ball b) {

    int centerR = (int)(b.position.y / TILE_SIZE);
    int centerC = (int)(b.position.x / TILE_SIZE);

    int bottomR = (int)((b.position.y + b.radius * 0.5f) / TILE_SIZE);
    int bottomC = (int)(b.position.x / TILE_SIZE);

    return (GetTile(centerR, centerC) == '6' || GetTile(bottomR, bottomC) == '6');
}

/*==============================================================================
||                          BALL UPDATE & TILE SETUP                            ||
==============================================================================*/
void UpdateBall(Ball *b, float dt, Gamestate *state){
    if(IsKeyDown(KEY_RIGHT)){
        b->velocity.x += MAX_ACC*dt;
    }
    if(IsKeyDown(KEY_LEFT)){
        b->velocity.x -= MAX_ACC*dt;
    }
    if(!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)){

        b->velocity.x *= powf(FRICTION, dt*60.0f);
    }

    if(IsSubmerged(*b) && b->type == Ball_pumped) {
        b->velocity.x *= powf(WATER_DRAG, dt * 60.0f);
        b->velocity.y *= powf(WATER_DRAG, dt * 60.0f);
        float netWaterAcc = WATER_GRAVITY - WATER_BUOYANCY; 
        b->velocity.y += netWaterAcc * dt;


        if(b->velocity.y > WATER_MAX_FALL) {
            b->velocity.y = WATER_MAX_FALL;
        }


        // if(IsKeyPressed(KEY_UP)){
        //     b->velocity.y = WATER_SWIM_IMPULSE;
        // }
        b->grounded = false;
    } else {
        b->velocity.x = Clamp(b->velocity.x, -MAX_SPEED, MAX_SPEED);
        b->velocity.y += GRAVITY*dt;
        if(b->velocity.y > MAX_FALL_SPEED) b->velocity.y = MAX_FALL_SPEED;   // FIX: fall-speed cap
        if(IsKeyDown(KEY_UP) && b->grounded){
            b->velocity.y = fminf(b->velocity.y, JUMP);
            b->grounded = false;
        }
    }
    b->position.x += b->velocity.x*dt;
    b->position.y += b->velocity.y*dt;

    b->rotation += (b->velocity.x/b->radius)*RAD2DEG*dt;
    b->grounded = false;

    int c0 = (int)floorf((b->position.x - b->radius)/TILE_SIZE) - 1;
    int c1 = (int)floorf((b->position.x + b->radius)/TILE_SIZE) + 1;
    int r0 = (int)floorf((b->position.y - b->radius)/TILE_SIZE) - 1;
    int r1 = (int)floorf((b->position.y + b->radius)/TILE_SIZE) + 1;
    if(c0 < 0) c0 = 0;
    if(r0 < 0) r0 = 0;
    if(c1 > current_col - 1) c1 = current_col - 1;
    if(r1 > current_row - 1) r1 = current_row - 1;

    for(int r = r0; r <= r1; r++){
        for(int c = c0; c <= c1; c++){
            char tile = GetTile(r, c);
            if(tile == '0' || tile == '6') continue;
            Rectangle tilerect = {c*TILE_SIZE, r*TILE_SIZE, TILE_SIZE, TILE_SIZE};

            if(tile == '4'){
                if(CheckCollisionCircleRec(b->position, b->radius, tilerect)){
                    SetTile(r, c, '0');
                    score += 500;
                    coin_collected = true;
                }
                continue;
            }
            else if(tile == '2'){
                Rectangle spikehit = {
                    tilerect.x + (TILE_SIZE * 0.25f),
                    tilerect.y + (TILE_SIZE * 0.25f),
                    TILE_SIZE * 0.45f,
                    TILE_SIZE * 0.45f
                };
                if(CheckCollisionCircleRec(b->position, b->radius, spikehit)){
                    *state = DEAD;
                    ball_popped = true;
                    return;
                }
                continue;
            }
            else if(tile == '7') {
                b->type = Ball_pumped;
                b->radius = 20.0f;
            }
            else if(tile == '8')
            {
                b->type = Ball_normal;
                b->radius = 14.0f;
            }

            BallPlatformCollision(b, tilerect, tile, state);
            if(*state != PLAYING) return;
        }
    }


    

    float mapW = current_col*TILE_SIZE;
    float mapH = current_row*TILE_SIZE;
    if(b->position.x < b->radius){
        b->position.x = b->radius;
        if(b->velocity.x < 0) b->velocity.x = 0;
    }
    if(b->position.x > mapW - b->radius){
        b->position.x = mapW - b->radius;
        if(b->velocity.x > 0) b->velocity.x = 0;
    }
    if(b->position.y < b->radius){
        b->position.y = b->radius;
        if(b->velocity.y < 0) b->velocity.y = 0;
    }
    if(b->position.y - b->radius > mapH){
        *state = DEAD;
        ball_popped = true;
    }
    
}

/*==============================================================================
||                         DRAWING TILES                                       ||
==============================================================================*/
void DrawTile(Levelasset *lvl){
    for(int r = 0; r < current_row; r++){
        for(int c = 0; c < current_col; c++){
            char tile = maps[current_level - 1][r * current_col + c];
            Texture2D *tex = NULL;

           


            switch(tile){
                case '1': tex = &lvl->brick;  break;
                case '2': tex = &lvl->spike;  break;
                case '3': tex = &lvl->spring; break;
                case '4': tex = &lvl->ring;   break;
                case '5': tex = &lvl->goal;   break;
                case '7': tex = &lvl->pumper; break;
                default: break;
            }
            
            if(tex != NULL && tile != '7'){
                DrawTexturePro(*tex, (Rectangle){0.0f, 0.0f, (float)tex->width, (float)tex->height},
                               (Rectangle){c*TILE_SIZE, r*TILE_SIZE, TILE_SIZE, TILE_SIZE},
                               (v2){0.0f, 0.0f}, 0.0f, WHITE);
            } else if(tex != NULL && tile == '7')
            {
                DrawTexturePro(*tex, (Rectangle){0.0f, 0.0f, (float)tex->width, (float)tex->height},
                               (Rectangle){c*TILE_SIZE, r*TILE_SIZE, TILE_SIZE, TILE_SIZE},
                               (v2){0.0f, 0.0f}, 0.0f, WHITE);
            }
             if(tile == '6')
            {
                DrawRectangle(c*TILE_SIZE, r*TILE_SIZE, TILE_SIZE,TILE_SIZE, DARKBLUE);
                
            }
        }
    }
}

Texture2D BgRemover(const char *path){
    Image img = LoadImage(path);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8); 
    Color *px = (Color *)img.data;                        
    Color bg = px[0];                                     
    int count = img.width * img.height;

    for(int i = 0; i < count; i++){
        
        int diff = abs(px[i].r - bg.r) + abs(px[i].g - bg.g) + abs(px[i].b - bg.b);

        if(diff < 60)       px[i].a = 0;                                          
        else if(diff < 120) px[i].a = (unsigned char)(px[i].a * (diff - 60) / 60); 
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

/*==============================================================================
||                          SPIDER MOVEMENTS                                  ||
==============================================================================*/
void DrawSpider(Texture2D spidersprite){
    for(int i = 0; i < spider_count; i++){
        DrawTexturePro(spidersprite, (Rectangle){0.0f, 0.0f, (float)spidersprite.width, (float)spidersprite.height},
                       active_spiders[i].rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
    }
}


void UpdateSpider(Ball *b, float dt, Gamestate *state){
    for(int i = 0; i < spider_count; i++){
        spider *s = &active_spiders[i];
        s->rect.y += s->speed*dt;

        int c0 = (int)(s->rect.x / TILE_SIZE);
        int c1 = (int)((s->rect.x + s->rect.width - 1.0f) / TILE_SIZE);

         if(s->speed > 0){
            int r = (int)floorf((s->rect.y + s->rect.height) / TILE_SIZE);
            for(int c = c0; c <= c1; c++){
                if(GetTile(r, c) == '1'){
                    s->rect.y = r*TILE_SIZE - s->rect.height;
                    s->speed = -s->speed;
                    break;
                }
            }
        }else{
            int r = (int)floorf(s->rect.y / TILE_SIZE);
            for(int c = c0; c <= c1; c++){
                if(GetTile(r, c) == '1'){
                    s->rect.y = (r + 1)*TILE_SIZE;
                    s->speed = -s->speed;
                    break;
                }
            }
        }

        if(CheckCollisionCircleRec(b->position, b->radius, s->rect)){
            *state = DEAD;
            ball_popped = true;
        }
    }
}

/*==============================================================================
||                               CAMERA UPDATE                                ||
==============================================================================*/
void UpdateGameCamera(Camera2D *camera, v2 target){
    float halfW = BASE_W / (2.0f * camera->zoom);
    float halfH = BASE_H / (2.0f * camera->zoom);
    float mapW = current_col * TILE_SIZE;
    float mapH = current_row * TILE_SIZE;

    camera->target.x = (mapW <= 2.0f*halfW) ? mapW/2.0f : Clamp(target.x, halfW, mapW - halfW);
    camera->target.y = (mapH <= 2.0f*halfH) ? mapH/2.0f : Clamp(target.y, halfH, mapH - halfH);
}

/*==============================================================================
||                                SCORE RECORDS                              ||
==============================================================================*/
//file theke score read kora
void LoadScoreHistory(ScoreRecord history[], int *count){
    *count = 0;
    FILE *file = fopen(SCORES, "r");
    if(file != NULL){
        while(*count < MAX_SCORES && fscanf(file, "%16s %d", history[*count].name, &history[*count].score) == 2){
            (*count)++;
        }
        fclose(file);
    }
}
//file e score lekha
void SaveScoreHistory(ScoreRecord history[], int count){
    FILE *file = fopen(SCORES, "w");
    if(file != NULL){
        for(int i = 0; i < count; i++){
            fprintf(file, "%s %d\n", history[i].name, history[i].score);
        }
        fclose(file);
    }
}

// leaderboard e add kora
void AddScoreRecord(ScoreRecord history[], int *count, const char *name, int newScore){
    if(*count < MAX_SCORES){
        strncpy(history[*count].name, (strlen(name) > 0) ? name : "Player", MAX_LEN);
        history[*count].name[MAX_LEN] = '\0';
        history[*count].score = newScore;
        (*count)++;
    }else{

        if(newScore > history[*count - 1].score){
            strncpy(history[*count - 1].name, (strlen(name) > 0) ? name : "Player", MAX_LEN);
            history[*count - 1].name[MAX_LEN] = '\0';
            history[*count - 1].score = newScore;
        }
    }

    for(int i = 0; i < *count - 1; i++){
        for(int j = i + 1; j < *count; j++){
            if(history[j].score > history[i].score){
                ScoreRecord temp = history[i];
                history[i] = history[j];
                history[j] = temp;
            }
        }
    }

    SaveScoreHistory(history, *count);
}

/*==============================================================================
||                          NAME INPUT                                         ||
==============================================================================*/
void UpdateNameInput(char *nameBuffer, int *letterCount, Gamestate *state){
    int key = GetCharPressed();

    while(key > 0){

        if((key > 32) && (key <= 125) && (*letterCount < MAX_LEN)){
            nameBuffer[*letterCount] = (char)key;
            nameBuffer[*letterCount + 1] = '\0';
            (*letterCount)++;
        }
        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        (*letterCount)--;
        if(*letterCount < 0) *letterCount = 0;
        nameBuffer[*letterCount] = '\0';
    }

    if(IsKeyPressed(KEY_ENTER) && *letterCount > 0){
        *state = LEVEL_SELECT;
    }
}

/*==============================================================================
||                                   PAUSE BUTTONS                            ||
==============================================================================*/
typedef enum ButtonState{
    NORMAL,
    PRESSED,
}ButtonState;

typedef struct Button{
    Rectangle rect;
    ButtonState state;
}Button;

int sound = 1;

Rectangle MenueRect = (Rectangle){0.27*BASE_W, 0.08*BASE_H, 0.46*BASE_W, 0.75*BASE_H};


Button PauseButton = {
    .rect = (Rectangle){0.94*BASE_W, 0.05*BASE_H, 0.04*BASE_W, 0.04*BASE_W},
    .state = NORMAL
};

Button ResumeButton = {
    .rect = (Rectangle){0.46*BASE_W, 0.24*BASE_H, 0.23*BASE_W, 0.21*BASE_H},
    .state = NORMAL
};

Button RetryButton = {
    .rect = (Rectangle){0.30*BASE_W, 0.24*BASE_H, 0.21*BASE_H, 0.21*BASE_H},
    .state = NORMAL
};

Button HomeButton = {
    .rect = (Rectangle){0.30*BASE_W, 0.54*BASE_H, 0.21*BASE_H, 0.21*BASE_H},
    .state = NORMAL
};




Button SoundOnOffButton = {
    .rect = (Rectangle){0.46*BASE_W, 0.54*BASE_H, 0.23*BASE_W, 0.21*BASE_H},
    .state = NORMAL,
};

bool Clicked(Button *btn, v2 mouse){
    return CheckCollisionPointRec(mouse, btn->rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}


void DrawButtonSprite(Texture2D tex, Button *btn){
    DrawTexturePro(tex, (Rectangle){0.0f, 0.0f, (float)tex.width, (float)tex.height}, btn->rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
}
/*==============================================================================
||                           MAIN MENU                                        ||
==============================================================================*/
#define MENU_FONT_PATH "assets/fonts/menu.ttf"// optional .ttf; built-in font is used if missing


typedef struct CreditLine{
    const char *heading;
    const char *text;
}CreditLine;

const CreditLine credits[] = {
    { "GAME DESIGN & PROGRAMMING", "Ahanaf Tahamid & Arshad Akter Kalpo" },
    { "SUPERVISED BY",        "Md. Mostofa Akbar Sir" },
    { "MADE WITH",                 "C and raylib" },
    { "INSPIRED BY",               "Bounce, the classic Nokia phone game" },
};
#define CREDIT_COUNT ((int)(sizeof(credits)/sizeof(credits[0])))

/*==============================================================================
||                                     COLORS                                 ||
==============================================================================*/
static const Color SKY1             = {  70, 140, 220, 255 };
static const Color SKY2             = { 178, 220, 250, 255 };
static const Color HILL_FAR         = { 150, 205, 190, 255 };
static const Color HILL_NEAR        = { 104, 178, 140, 255 };
static const Color BUTTON1          = { 224,  64,  52, 255 };
static const Color BUTTON1_HI       = { 250, 100,  74, 255 };
static const Color BUTTON2          = {  46,  86, 150, 255 };
static const Color BUTTON2_HI       = {  66, 120, 200, 255 };
static const Color BTN_EDGE         = {  28,  36,  64, 255 };
static const Color GOLD_C           = { 255, 204,  64, 255 };
static const Color SILVER_C         = { 190, 196, 208, 255 };
static const Color BRONZE_C         = { 208, 130,  60, 255 };
static const Color PANEL_C          = { 255, 251, 242, 255 };
static const Color INK              = {  46,  42,  64, 255 };
static const Color INK_SOFT         = { 120, 116, 140, 255 };
static const Color RED_C            = { 214,  58,  48, 255 };


#define MENU_BTN_W 340.0f             
#define MENU_BTN_H 54.0f             
#define MENU_BTN_GAP 12.0f            
#define MENU_BTN_TOP 272.0f          
#define GROUND_H 64.0f                
#define GROUND_TOP (BASE_H - GROUND_H)


typedef enum MenuItem{
    menu_START,
    menu_HOWTO,
    menu_LEADERBOARD,
    menu_CREDITS,
    menu_QUIT,
    menu_COUNT
}MenuItem;

const char *menuLabels[menu_COUNT] = { "START", "HOW TO PLAY", "LEADERBOARD", "CREDITS", "QUIT" };

/*==============================================================================
||                              MENU VARIABLES                               ||
==============================================================================*/
Font uiFont;
bool uiFontIsDefault = true;
float fadeAlpha = 1.0f;
bool quitRequested = false;
bool cursorOverButton = false;

float menuHover[menu_COUNT] = {0};
int menuSelected = 0;
float menuMarkerY = 0.0f;
float backHover = 0.0f;

//
typedef struct Cloud{
    float x, y, scale, speed;
}Cloud;
#define CLOUD_COUNT 6                 
Cloud clouds[CLOUD_COUNT];
float menuTime = 0.0f;
float menuBallX = -60.0f;

//hover animation stuff
float Approach(float value, float target, float speed, float dt){
    float t = speed*dt;
    if(t > 1.0f) t = 1.0f;
    return value + (target - value)*t;
}

//coloring
Color MixColor(Color a, Color b, float t){//t=0 --> a and t=1 --> b sob color a and b er majhe
    return (Color){
        (unsigned char)(a.r + (b.r - a.r)*t),
        (unsigned char)(a.g + (b.g - a.g)*t),
        (unsigned char)(a.b + (b.b - a.b)*t),
        (unsigned char)(a.a + (b.a - a.a)*t)
    };
}


void GoTo(Gamestate *state, Gamestate next){
    *state = next;
    fadeAlpha = 1.0f;
}

/*==============================================================================
||                                   FONTS                                    ||
==============================================================================*/
void LoadMenuFont(void){
    // if(FileExists(MENU_FONT_PATH)){
    //     uiFont = LoadFontEx(MENU_FONT_PATH, 64, 0, 0);
    //     if(uiFont.texture.id != 0){
    //         SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
    //         uiFontIsDefault = false;
    //         return;
    //     }
    // }
    uiFont = GetFontDefault();
    uiFontIsDefault = true;
}

float TextSpacing(float size){ return uiFontIsDefault ? size/10.0f : size/20.0f; }

v2 MeasureUI(const char *text, float size){
    return MeasureTextEx(uiFont, text, size, TextSpacing(size));
}

void DrawUI(const char *text, float x, float y, float size, Color color){
    DrawTextEx(uiFont, text, (v2){ roundf(x), roundf(y) }, size, TextSpacing(size), color);
}

void DrawUICentered(const char *text, float centerX, float y, float size, Color color){
    v2 m = MeasureUI(text, size);
    DrawUI(text, centerX - m.x/2.0f, y, size, color);
}

void DrawUIShadowCentered(const char *text, float centerX, float y, float size, Color color){
    DrawUICentered(text, centerX + 2, y + 3, size, Fade(BLACK, 0.30f));
    DrawUICentered(text, centerX, y, size, color);
}


void DrawTexOr(Texture2D tex, Rectangle dest, Color fallback){
    if(tex.id != 0) DrawTexturePro(tex, (Rectangle){0, 0, (float)tex.width, (float)tex.height}, dest, (v2){0, 0}, 0.0f, WHITE);
    else DrawRectangleRec(dest, fallback);
}

//ball spinning design hudai
void DrawSpinningBall(Texture2D ballTex, float x, float y, float r, float rotation){
    if(ballTex.id != 0){
        DrawTexturePro(ballTex, (Rectangle){0, 0, (float)ballTex.width, (float)ballTex.height},
                       (Rectangle){x, y, r*2, r*2}, (v2){r, r}, rotation, WHITE);
    }else{
        DrawCircleV((v2){x, y}, r, RED_C);
    }
}


/*==============================================================================
||                          BACKGROUND                                        ||
==============================================================================*/

void InitMenuBackground(void){
    for(int i = 0; i < CLOUD_COUNT; i++){
        clouds[i].x = (float)GetRandomValue(0, BASE_W);
        clouds[i].y = (float)GetRandomValue(40, 340);
        clouds[i].scale = 0.5f + GetRandomValue(0, 70)/100.0f;
        clouds[i].speed = 8.0f + clouds[i].scale*18.0f;
    }
    menuMarkerY = MENU_BTN_TOP + MENU_BTN_H/2.0f;
}

void UpdateMenuBackground(float dt){
    menuTime += dt;
    for(int i = 0; i < CLOUD_COUNT; i++){
        clouds[i].x += clouds[i].speed*dt;
        if(clouds[i].x - 60.0f*clouds[i].scale > BASE_W){
            clouds[i].x = -130.0f*clouds[i].scale;
            clouds[i].y = (float)GetRandomValue(40, 340);
        }
    }
    menuBallX += 170.0f*dt;
    if(menuBallX > BASE_W + 60) menuBallX = -60.0f;
}


void DrawCloud(Cloud c){
    float s = c.scale;
    Color col = MixColor(SKY2, (Color){255, 255, 255, 255}, 0.55f + 0.45f*(s - 0.5f)/0.7f);
    DrawCircleV((v2){c.x,          c.y},          34*s, col);
    DrawCircleV((v2){c.x + 40*s,   c.y - 16*s},   42*s, col);
    DrawCircleV((v2){c.x + 84*s,   c.y},          32*s, col);
    DrawRectangleRounded((Rectangle){c.x - 20*s, c.y, 128*s, 34*s}, 1.0f, 12, col);
}

void DrawMenuBackground(Levelasset *lvl, Texture2D ballTex,bool playing){
    DrawRectangleGradientV(0, 0, BASE_W, BASE_H, SKY1, SKY2);
    for(int i = 5; i >= 1; i--){
        DrawCircleV((v2){ BASE_W - 170.0f, 130.0f }, 50.0f + i*22.0f, Fade((Color){255, 240, 180, 255}, 0.06f));
    }
    DrawCircleV((v2){ BASE_W - 170.0f, 130.0f }, 52.0f, (Color){255, 236, 150, 255});

    for(int i = 0; i < CLOUD_COUNT; i++) DrawCloud(clouds[i]);

    float farOff = fmodf(menuTime*10.0f, 300.0f);
    for(int i = -1; i <= 5; i++){
        DrawCircleV((v2){ i*300.0f - farOff + 150.0f, GROUND_TOP + 170.0f }, 230.0f, HILL_FAR);
    }
    float nearOff = fmodf(menuTime*22.0f, 420.0f);
    for(int i = -1; i <= 4; i++){
        DrawCircleV((v2){ i*420.0f - nearOff + 60.0f, GROUND_TOP + 150.0f }, 200.0f, HILL_NEAR);
    }

    if(playing)return;

    float groundOff = fmodf(menuTime*40.0f, TILE_SIZE);
    for(float x = -groundOff; x < BASE_W; x += TILE_SIZE){
        for(int row = 0; row < 2; row++){
            Rectangle t = { x, GROUND_TOP + row*TILE_SIZE, TILE_SIZE, TILE_SIZE };
            DrawTexOr(lvl->brick, t, (Color){150, 72, 52, 255});
            if(lvl->brick.id == 0) DrawRectangleLinesEx(t, 1.0f, (Color){110, 50, 36, 255});
        }
    }
    DrawRectangle(0, (int)GROUND_TOP, BASE_W, 3, Fade(BLACK, 0.25f));

    float r = 22.0f;
    float h = fabsf(sinf(menuTime*3.0f));
    float by = GROUND_TOP - r - h*150.0f;

    DrawEllipse((int)menuBallX, (int)GROUND_TOP + 3, r*(1.0f - 0.5f*h), 5.0f, Fade(BLACK, 0.28f*(1.0f - 0.6f*h)));
    DrawSpinningBall(ballTex, menuBallX, by, r, (menuBallX + menuTime*40.0f)/r*RAD2DEG);
}


Rectangle MenuButtonRect(int i){
    return (Rectangle){ (BASE_W - MENU_BTN_W)/2.0f, MENU_BTN_TOP + i*(MENU_BTN_H + MENU_BTN_GAP), MENU_BTN_W, MENU_BTN_H };
}

/*==============================================================================
||                                 PANELS                                    ||
==============================================================================*/
Rectangle PanelRect(void){ return (Rectangle){ BASE_W/2.0f - 450.0f, 96.0f, 900.0f, 452.0f }; }
Rectangle BackButtonRect(void){ return (Rectangle){ BASE_W/2.0f - 120.0f, 574.0f, 240.0f, 54.0f }; }


void DrawMenuButton(Rectangle r, const char *label, float hover, bool primary){
    float grow = hover*12.0f;
    float lift = hover*3.0f;
    Rectangle b = { r.x - grow/2, r.y - grow/4 - lift, r.width + grow, r.height + grow/2 };

    Color face = primary ? MixColor(BUTTON1, BUTTON1_HI, hover)
                         : MixColor(BUTTON2, BUTTON2_HI, hover);
    Color edge = MixColor(BTN_EDGE, GOLD_C, hover);

    DrawRectangleRounded((Rectangle){ b.x, b.y + 6 + lift, b.width, b.height }, 0.45f, 12, Fade(BLACK, 0.22f)); // shadow
    DrawRectangleRounded((Rectangle){ b.x - 3, b.y - 3, b.width + 6, b.height + 6 }, 0.45f, 12, edge);          // outline
    DrawRectangleRounded(b, 0.45f, 12, face);
    DrawRectangleRounded((Rectangle){ b.x + 8, b.y + 5, b.width - 16, b.height*0.40f }, 0.6f, 12,
                         Fade(WHITE, 0.16f + 0.10f*hover));                                                    // shine

    float size = 30.0f;
    float ty = b.y + (b.height - size)/2.0f + 1;
    DrawUIShadowCentered(label, b.x + b.width/2.0f, ty, size, WHITE);
}

int UpdateMainMenu(v2 mouse, float dt){
    int activated = -1;

    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuSelected = (menuSelected + 1) % menu_COUNT;
    if(IsKeyPressed(KEY_UP)   || IsKeyPressed(KEY_W)) menuSelected = (menuSelected + menu_COUNT - 1) % menu_COUNT;
    if(IsKeyPressed(KEY_ESCAPE)) menuSelected = menu_QUIT;
    v2 delta = GetMouseDelta();
    bool mouseMoved = (delta.x != 0.0f || delta.y != 0.0f);
    for(int i = 0; i < menu_COUNT; i++){
        if(CheckCollisionPointRec(mouse, MenuButtonRect(i))){
            cursorOverButton = true;
            if(mouseMoved) menuSelected = i;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){ menuSelected = i; activated = i; }
        }
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) activated = menuSelected;

    for(int i = 0; i < menu_COUNT; i++){
        menuHover[i] = Approach(menuHover[i], (i == menuSelected) ? 1.0f : 0.0f, 14.0f, dt);
    }
    Rectangle sel = MenuButtonRect(menuSelected);
    menuMarkerY = Approach(menuMarkerY, sel.y + sel.height/2.0f, 18.0f, dt);

    return activated;
}
//back to prev
bool UpdateBackButton(v2 mouse, float dt){
    bool over = CheckCollisionPointRec(mouse, BackButtonRect());
    if(over) cursorOverButton = true;
    backHover = Approach(backHover, over ? 1.0f : 0.0f, 14.0f, dt);
    return (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ||
           IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ENTER);
}

void DrawMainMenu(Texture2D logo, Texture2D ballTex, ScoreRecord history[], int count){
    float bob = sinf(menuTime*2.0f)*6.0f;
    if(logo.id != 0){
        DrawTexturePro(logo,
            (Rectangle){0.0f, 0.0f, (float)logo.width, (float)logo.height - 50},
            (Rectangle){(BASE_W - 400.0f)/2, 40.0f + bob, 400.0f, 200.0f},
            (v2){0.0f, 0.0f}, 0.0f, WHITE);
    }else{
        DrawUIShadowCentered("BOUNCE", BASE_W/2.0f, 100.0f + bob, 80, RED_C);
    }

    for(int i = 0; i < menu_COUNT; i++){
        DrawMenuButton(MenuButtonRect(i), menuLabels[i], menuHover[i], i == menu_START);
    }

    Rectangle sel = MenuButtonRect(menuSelected);
    DrawSpinningBall(ballTex, sel.x - 34.0f - menuHover[menuSelected]*6.0f, menuMarkerY, 15.0f, menuTime*240.0f);

    if(count > 0){
        const char *best = TextFormat("%s  %d", history[0].name, history[0].score);
        v2 m1 = MeasureUI("BEST", 20);
        v2 m2 = MeasureUI(best, 20);
        float w = m1.x + m2.x + 40;
        Rectangle chip = { BASE_W - w - 20, 20, w, 38 };
        DrawRectangleRounded(chip, 0.5f, 12, Fade(BTN_EDGE, 0.55f));
        DrawUI("BEST", chip.x + 14, chip.y + 10, 20, GOLD_C);
        DrawUI(best, chip.x + 26 + m1.x, chip.y + 10, 20, WHITE);
    }

}


void DrawTitlebar(const char *title, float centerX, float y, Color face){
    v2 m = MeasureUI(title, 40);
    Rectangle rib = { centerX - (m.x + 80)/2.0f, y, m.x + 80, 60 };
    DrawRectangleRounded((Rectangle){ rib.x, rib.y + 5, rib.width, rib.height }, 0.5f, 12, Fade(BLACK, 0.2f));
    DrawRectangleRounded((Rectangle){ rib.x - 3, rib.y - 3, rib.width + 6, rib.height + 6 }, 0.5f, 12, BTN_EDGE);
    DrawRectangleRounded(rib, 0.5f, 12, face);
    DrawUIShadowCentered(title, centerX, rib.y + 10, 40, WHITE);
}


void DrawPanelScreen(const char *title){
    DrawRectangle(0, 0, BASE_W, BASE_H, Fade(BLACK, 0.12f));

    Rectangle p = PanelRect();
    DrawRectangleRounded((Rectangle){ p.x, p.y + 10, p.width, p.height }, 0.06f, 16, Fade(BLACK, 0.20f));
    DrawRectangleRounded((Rectangle){ p.x - 4, p.y - 4, p.width + 8, p.height + 8 }, 0.06f, 16, Fade(WHITE, 0.55f));
    DrawRectangleRounded(p, 0.06f, 16, PANEL_C);

    DrawTitlebar(title, BASE_W/2.0f, p.y - 30, BUTTON1);

    DrawMenuButton(BackButtonRect(), "BACK", backHover, false);
}

//keyboards key
float DrawKeyCap(float x, float y, const char *label){
    float size = 20.0f;
    v2 m = MeasureUI(label, size);
    float w = (m.x + 24 > 40) ? m.x + 24 : 40;
    DrawRectangleRounded((Rectangle){ x, y + 4, w, 36 }, 0.3f, 8, (Color){ 22, 22, 36, 255 });
    DrawRectangleRounded((Rectangle){ x, y, w, 36 }, 0.3f, 8, (Color){ 72, 74, 100, 255 });
    DrawRectangleRounded((Rectangle){ x + 3, y + 3, w - 6, 14 }, 0.5f, 8, Fade(WHITE, 0.12f));
    DrawUI(label, x + (w - m.x)/2.0f, y + 8, size, WHITE);
    return w;
}

//how to play description
void DrawHowToPlay(Levelasset *lvl, Texture2D ballTex, Texture2D spiderTex){
    DrawPanelScreen("HOW TO PLAY");
    Rectangle p = PanelRect();

    float lx = p.x + 40, ty = p.y + 56;
    DrawUI("CONTROLS", lx, ty, 30, RED_C);
    DrawRectangle((int)lx, (int)(ty + 36), 380, 3, Fade(RED_C, 0.35f));

    const char *keyA[] = { "LEFT", "UP", "P",   "R",           "H"       };
    const char *keyB[] = { "RIGHT", NULL, "ESC", NULL,         NULL      };
    const char *what[] = { "Roll the ball", "Jump", "Pause", "Retry level", "Go home" };
    for(int i = 0; i < 5; i++){
        float y = ty + 58 + i*62;
        float x = lx;
        x += DrawKeyCap(x, y, keyA[i]) + 8;
        if(keyB[i]) x += DrawKeyCap(x, y, keyB[i]) + 8;
        DrawUI(what[i], lx + 190, y + 8, 20, INK);
    }

    float rx = p.x + 480;
    DrawUI("OBJECTS", rx, ty, 30, RED_C);
    DrawRectangle((int)rx, (int)(ty + 36), 380, 3, Fade(RED_C, 0.35f));

    Texture2D icons[] = { lvl->ring, lvl->spike, lvl->spring, lvl->goal, spiderTex };
    Color fallback[]  = { GOLD_C, GRAY, GREEN, SKYBLUE, DARKGRAY };
    const char *names[] = { "Ring", "Spike", "Spring", "Goal", "Spider" };
    const char *descs[] = { "Collect for +500 points", "One touch and you pop!", "Launches you up high",
                            "Reach it to clear the level", "Moves up & down - avoid it" };
    for(int i = 0; i < 5; i++){
        float y = ty + 58 + i*62;
        DrawRectangleRounded((Rectangle){ rx - 2, y - 4, 48, 48 }, 0.3f, 8, Fade(INK, 0.07f));
        DrawTexOr(icons[i], (Rectangle){ rx + 2, y, 40, 40 }, fallback[i]);
        DrawUI(names[i], rx + 62, y, 20, INK);
        DrawUI(descs[i], rx + 62, y + 22, 20, INK);
    }

    float h = fabsf(sinf(menuTime*4.0f));
    DrawSpinningBall(ballTex, p.x + p.width/2.0f, p.y + p.height - 34 - h*30, 10, menuTime*200);
}

/*==============================================================================
||                                 LEADERBOARD                                    ||
==============================================================================*/
void DrawLeaderboard(ScoreRecord history[], int count, const char *playerName){
    DrawPanelScreen("LEADERBOARD");
    Rectangle p = PanelRect();
    float top = p.y + 58;

    DrawUI("RANK", p.x + 56, top, 20, INK_SOFT);
    DrawUI("NAME", p.x + 170, top, 20, INK_SOFT);
    v2 sm = MeasureUI("SCORE", 20);
    DrawUI("SCORE", p.x + p.width - 56 - sm.x, top, 20, INK_SOFT);
    DrawRectangle((int)(p.x + 30), (int)(top + 28), (int)(p.width - 60), 2, Fade(INK, 0.15f));

    if(count == 0){
        DrawUICentered("No scores yet - be the first!", p.x + p.width/2.0f, p.y + p.height/2.0f, 30, INK_SOFT);
        return;
    }

    int shown = (count < 8) ? count : 8;
    for(int i = 0; i < shown; i++){
        float y = top + 38 + i*42;
        Rectangle row = { p.x + 30, y, p.width - 60, 38 };
        if(i % 2 == 0) DrawRectangleRounded(row, 0.3f, 8, Fade(INK, 0.05f));
        if(playerName[0] != '\0' && strcmp(history[i].name, playerName) == 0){
            DrawRectangleRounded(row, 0.3f, 8, Fade(GOLD_C, 0.25f));   // highlight the current player
        }

        float cx = p.x + 78, cy = y + 19;
        if(i < 3){
            Color medal = (i == 0) ? GOLD_C : (i == 1) ? SILVER_C : BRONZE_C;
            DrawCircleV((v2){ cx, cy + 2 }, 15, Fade(BLACK, 0.2f));
            DrawCircleV((v2){ cx, cy }, 15, medal);
            DrawUICentered(TextFormat("%d", i + 1), cx, cy - 10, 20, WHITE);
        }else{
            DrawUICentered(TextFormat("%d", i + 1), cx, cy - 10, 20, INK_SOFT);
        }

        DrawUI(history[i].name, p.x + 170, y + 9, 20, INK);
        const char *s = TextFormat("%d", history[i].score);
        v2 m = MeasureUI(s, 20);
        DrawUI(s, p.x + p.width - 56 - m.x, y + 9, 20, INK);
    }
}


void DrawCredits(void){
    DrawPanelScreen("CREDITS");
    Rectangle p = PanelRect();
    float cx = p.x + p.width/2.0f;
    float y = p.y + 60;

    DrawUICentered("BOUNCE CLASSIC", cx, y, 40, RED_C);
    y += 64;
    for(int i = 0; i < CREDIT_COUNT; i++){
        DrawUICentered(credits[i].heading, cx, y, 20, INK_SOFT);
        DrawUICentered(credits[i].text, cx, y + 26, 30, INK);
        y += 72;
    }
    DrawUICentered("Thanks for playing!", cx, y + 8, 20, RED_C);
}
/*==============================================================================
||                               LEVEL SELECT                                 ||
==============================================================================*/

const char *levelNames[3] = { "Easy", "Medium", "Hard" };


float levelHover[3] = {0};   
int levelSelected = 0;       


Rectangle LevelCardRect(int i){
    return (Rectangle){ BASE_W/2.0f - 380.0f + i*280.0f, 200.0f, 200.0f, 200.0f };
}


int UpdateLevelSelect(v2 mouse, float dt){
    int result = -1;

    if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) levelSelected = (levelSelected + 1) % 3;
    if(IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) levelSelected = (levelSelected + 2) % 3;

  
    v2 delta = GetMouseDelta();
    bool mouseMoved = (delta.x != 0.0f || delta.y != 0.0f);
    for(int i = 0; i < 3; i++){
        if(CheckCollisionPointRec(mouse, LevelCardRect(i))){
            cursorOverButton = true;
            if(mouseMoved) levelSelected = i;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) result = i;
        }
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) result = levelSelected;


    bool goBack = CheckCollisionPointRec(mouse, BackButtonRect());
    if(goBack) cursorOverButton = true;
    backHover = Approach(backHover, goBack ? 1.0f : 0.0f, 14.0f, dt);
    if((goBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ||
       IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) result = 3;

    for(int i = 0; i < 3; i++){
        levelHover[i] = Approach(levelHover[i], (i == levelSelected) ? 1.0f : 0.0f, 14.0f, dt);
    }
    return result;
}

void DrawLevelSelect(Texture2D ballTex){
    DrawTitlebar("SELECT LEVEL", BASE_W/2.0f, 50.0f, BUTTON1);

    for(int i = 0; i < 3; i++){
        float hv = levelHover[i];
        Rectangle r = LevelCardRect(i);


        float grow = hv*14.0f, lift = hv*6.0f;
        Rectangle c = { r.x - grow/2, r.y - grow/2 - lift, r.width + grow, r.height + grow };
        float cx = c.x + c.width/2.0f;

        DrawRectangleRounded((Rectangle){ c.x, c.y + 8 + lift, c.width, c.height }, 0.2f, 12, Fade(BLACK, 0.22f)); // shadow
        DrawRectangleRounded((Rectangle){ c.x - 4, c.y - 4, c.width + 8, c.height + 8 }, 0.2f, 12,
                             MixColor(BTN_EDGE, GOLD_C, hv));                                                    // outline
        DrawRectangleRounded(c, 0.2f, 12, MixColor(BUTTON2, BUTTON2_HI, hv));                                   // face
        DrawRectangleRounded((Rectangle){ c.x + 10, c.y + 8, c.width - 20, c.height*0.35f }, 0.4f, 12,
                             Fade(WHITE, 0.12f + 0.08f*hv));                                                     // shine

        DrawUIShadowCentered("LEVEL", cx, c.y + 32, 20, Fade(WHITE, 0.85f));
        DrawUIShadowCentered(TextFormat("%d", i + 1), cx, c.y + 70, 90, WHITE);

       
        DrawUIShadowCentered(levelNames[i], cx, r.y + r.height + 26, 30, WHITE);
      
    }

  
    Rectangle s = LevelCardRect(levelSelected);
    float bounce = fabsf(sinf(menuTime*4.0f))*26.0f;
    DrawSpinningBall(ballTex, s.x + s.width/2.0f, s.y - 6.0f*levelHover[levelSelected] - 30.0f - bounce, 14.0f, menuTime*200.0f);

    DrawMenuButton(BackButtonRect(), "BACK", backHover, false);
}
/*==============================================================================
||                                 RESULT BOX                                  ||
==============================================================================*/


#define END_BOX_DELAY 0.5f            
#define END_BOX_DROP 0.35f            
#define END_COUNT_UP 0.8f             
static const Color WIN_GREEN = { 60, 168, 92, 255 };


float endTimer = 0.0f;
int bestBeforeRun = 0;
float endHover[2] = {0};
int endSelected = 0;


void StartEndBox(void){
    endTimer = 0.0f;
    endSelected = 0;
    endHover[0] = endHover[1] = 0.0f;
}

/*==============================================================================
||                                 BOX ANIMATIONS                                    ||
==============================================================================*/
float Box_animate(float t){
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3*powf(t - 1.0f, 3.0f) + c1*powf(t - 1.0f, 2.0f);
}


Rectangle EndBoxRect(float yOffset){
    return (Rectangle){ BASE_W/2.0f - 270.0f, 150.0f + yOffset, 540.0f, 400.0f };
}

Rectangle EndButtonRect(int i, float yOffset){
    Rectangle b = EndBoxRect(yOffset);
    return (Rectangle){ b.x + 40.0f + i*240.0f, b.y + b.height - 88.0f, 220.0f, 58.0f };
}

int UpdateEndBox(v2 mouse, float dt){
    endTimer += dt;
    for(int i = 0; i < 2; i++){
        endHover[i] = Approach(endHover[i], (i == endSelected) ? 1.0f : 0.0f, 14.0f, dt);
    }

    if(endTimer < END_BOX_DELAY + END_BOX_DROP) return -1;

    if(IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) endSelected = 0;
    if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) endSelected = 1;
    if(IsKeyPressed(KEY_R)) return 0;
    if(IsKeyPressed(KEY_H) || IsKeyPressed(KEY_ESCAPE)) return 1;

    v2 delta = GetMouseDelta();
    bool mouseMoved = (delta.x != 0.0f || delta.y != 0.0f);
    for(int i = 0; i < 2; i++){
        if(CheckCollisionPointRec(mouse, EndButtonRect(i, 0.0f))){
            cursorOverButton = true;
            if(mouseMoved) endSelected = i;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return i;
        }
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) return endSelected;
    return -1;
}


void DrawEndBox(bool won, int levelNum, int finalScore, int best, bool newBest){
    float t = (endTimer - END_BOX_DELAY)/END_BOX_DROP;
    if(t <= 0.0f) return;
    if(t > 1.0f) t = 1.0f;

    DrawRectangle(0, 0, BASE_W, BASE_H, Fade(BLACK, 0.45f*t));

    Rectangle start = EndBoxRect(0.0f);
    float yOff = (1.0f - Box_animate(t))*-(start.y + start.height + 40.0f);
    Rectangle b = EndBoxRect(yOff);
    float cx = b.x + b.width/2.0f;

    DrawRectangleRounded((Rectangle){ b.x, b.y + 10, b.width, b.height }, 0.08f, 16, Fade(BLACK, 0.25f));
    DrawRectangleRounded((Rectangle){ b.x - 4, b.y - 4, b.width + 8, b.height + 8 }, 0.08f, 16, Fade(WHITE, 0.6f));
    DrawRectangleRounded(b, 0.08f, 16, PANEL_C);
    DrawTitlebar(won ? "LEVEL CLEARED!" : "GAME OVER", cx, b.y - 30, won ? WIN_GREEN : BUTTON1);

    DrawUICentered(TextFormat("LEVEL %d", levelNum), cx, b.y + 48, 20, INK_SOFT);

    float c = (endTimer - END_BOX_DELAY - END_BOX_DROP)/END_COUNT_UP;
    if(c < 0.0f) c = 0.0f;
    if(c > 1.0f) c = 1.0f;
    int shownScore = (int)(finalScore*c);
    if(c < 1.0f) shownScore -= shownScore % 10;
    DrawUICentered("SCORE", cx, b.y + 84, 20, INK_SOFT);
    DrawUICentered(TextFormat("%d", shownScore), cx, b.y + 110, 60, INK);

    DrawRectangle((int)(b.x + 60), (int)(b.y + 186), (int)(b.width - 120), 2, Fade(INK, 0.12f));

    int shownBest = best;
    if(newBest && shownScore > bestBeforeRun) shownBest = shownScore;
    else if(newBest) shownBest = bestBeforeRun;
    DrawUICentered("HIGH SCORE", cx, b.y + 200, 20, INK_SOFT);
    DrawUICentered(TextFormat("%d", shownBest), cx, b.y + 224, 30, INK);

    if(newBest && finalScore > 0 && c >= 1.0f){
        float pulse = 0.5f + 0.5f*sinf(endTimer*6.0f);
        const char *txt = "NEW HIGH SCORE!";
        v2 m = MeasureUI(txt, 20);
        Rectangle badge = { cx - (m.x + 32)/2.0f, b.y + 262, m.x + 32, 32 };
        DrawRectangleRounded((Rectangle){ badge.x - 4*pulse, badge.y - 3*pulse, badge.width + 8*pulse, badge.height + 6*pulse },
                             0.6f, 12, Fade(GOLD_C, 0.35f));                  // glow
        DrawRectangleRounded(badge, 0.6f, 12, GOLD_C);
        DrawUICentered(txt, cx, badge.y + 6, 20, BTN_EDGE);
    }

    DrawMenuButton(EndButtonRect(0, yOff), won ? "REPLAY" : "RETRY", endHover[0], true);
    DrawMenuButton(EndButtonRect(1, yOff), "HOME", endHover[1], false);
}

/*==============================================================================
||                                 MAIN FUNCTION                                ||
==============================================================================*/
int main(void){

/*==============================================================================
||                                 WINDOW SCREEN                                   ||
==============================================================================*/
    InitWindow(BASE_W, BASE_H, "Bounce Classic");
    InitAudioDevice();   
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);


    LoadMenuFont();
    InitMenuBackground();    


    ScoreRecord scoreHistory[MAX_SCORES];
    int scoreCount = 0;
    LoadScoreHistory(scoreHistory, &scoreCount);


    char playerName[MAX_LEN + 1] = "\0";
    int letterCount = 0;

/*==============================================================================
||                             SPRITES & TEXTURE INIT                          ||
==============================================================================*/
    Texture2D ballsprite = LoadTexture("assets/images/red-ball.png");
    Texture2D pumpedballsprite = LoadTexture("assets/images/ball_big@2x.png");
    Texture2D retrybuttonsprite = LoadTexture("assets/images/retry-button.png");
    Texture2D resumebuttonsprite = LoadTexture("assets/images/resume.png");
    Texture2D homebuttonsprite = LoadTexture("assets/images/home.png");
    Texture2D logo = LoadTexture("assets/images/title-logo.png");
    Texture2D pausesprite = LoadTexture("assets/images/PauseButton.png");
    Texture2D levelselectsprite = LoadTexture("assets/images/level-select-button.png");
    Texture2D spidersprite = LoadTexture("assets/images/spider.png");
    Texture2D poppedballsprite = LoadTexture("assets/images/pop-red-ball.png");   
    Sound coincollectaudio = LoadSound("assets/audios/coins.mp3");                 
    Sound ballpoppedaudio  = LoadSound("assets/audios/pop.mp3");
    Sound levelpassedaudio = LoadSound("assets/audios/universfield-next-level-114480.mp3");
    Texture2D soundonsprite = LoadTexture("assets/images/sound_on.png");
    Texture2D soundoffsprite = LoadTexture("assets/images/sound_off.png");
    Music menuemusic = LoadMusicStream("assets/audios/stage1.wav");
    

    Ball ball = {
        .radius = 14.0f,
        .rotation = 0.0f,
        .texture = ballsprite,
        .type = Ball_normal
    };

    Levelasset levelAssets = {
        .brick  = LoadTexture("assets/images/brick.png"),
        .spike  = BgRemover("assets/images/tile_spike.png"),
        .spring = LoadTexture("assets/images/spring.png"),
        .ring   = BgRemover("assets/images/tile_ring.png"),
        .goal   = BgRemover("assets/images/tile_goal.png"),
        .pumper = LoadTexture("assets/images/pumper@2x.png")
    };


    Gamestate state = MAIN_MENUE;
    // Rectangle src = {0.0f, 0.0f, (float)ball.texture.width, (float)ball.texture.height};
    // Rectangle src2 = {0.0f, 0.0f, (float)ball.texture.width, (float)ball.texture.height};
    Camera2D camera = { 0 };
    camera.offset = (v2){ BASE_W / 2.0f, BASE_H / 2.0f };
    camera.zoom = 1.6f;

/*==============================================================================
||                                 GAME LOOP                                   ||
==============================================================================*/
    while(!WindowShouldClose() && !quitRequested){

/*==============================================================================
||                                 GAME UPDATE                                 ||
==============================================================================*/
        float dt = GetFrameTime();
        Rectangle src = {0.0f, 0.0f, (float)ball.texture.width, (float)ball.texture.height};
        if(dt > 1.0f/30.0f) dt = 1.0f/30.0f;

        v2 mouse = GetMousePosition();
        cursorOverButton = false;
        if(fadeAlpha > 0.0f) fadeAlpha -= dt*4.0f;

        if(sound == 1 && !IsMusicStreamPlaying(menuemusic) && (state == MAIN_MENUE || state == NAME_INPUT || state == HOW_TO_PLAY || state == CREDITS || state == LEADERBOARD))
        {
            PlayMusicStream(menuemusic);
        }
        else 
        {
            StopMusicStream(menuemusic);
        }
        
        if(IsMusicStreamPlaying(menuemusic)) UpdateMusicStream(menuemusic);
        UpdateMenuBackground(dt);


        switch(state){


            case PLAYING:
                  
                if(Clicked(&PauseButton, mouse) || IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)){
                    state = PAUSED;
                    break;
                }
                UpdateBall(&ball, dt, &state);
                if(state == PLAYING) UpdateSpider(&ball, dt, &state);

            //Audio stuff
                if(coin_collected && sound){ PlaySound(coincollectaudio); coin_collected = false; }
                if(ball_popped && sound)   { PlaySound(ballpoppedaudio);  ball_popped = false; }
                if(level_passed && sound)  { PlaySound(levelpassedaudio); level_passed = false; }

                if((state == DEAD || state == WIN) && !scoresaved){

                    bestBeforeRun = (scoreCount > 0) ? scoreHistory[0].score : 0;
                    AddScoreRecord(scoreHistory, &scoreCount, playerName, score);
                    scoresaved = true;
                    StartEndBox();
                }
                break;

    
            case PAUSED:
                if(Clicked(&ResumeButton, mouse) || IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) state = PLAYING;
                else if(Clicked(&RetryButton, mouse) || IsKeyPressed(KEY_R)) Reset(&ball, &state);
                else if(Clicked(&HomeButton, mouse) || IsKeyPressed(KEY_H)) GoTo(&state, LEVEL_SELECT);
                else if(Clicked(&SoundOnOffButton,mouse)) sound ^= 1;
                break;

            case DEAD:
            case WIN:
            {
                int action = UpdateEndBox(mouse, dt);
                if(action == 0) Reset(&ball, &state);
                else if(action == 1) GoTo(&state, MAIN_MENUE);
                break;
            }


            case MAIN_MENUE:
            {

                int item = UpdateMainMenu(mouse, dt);
                if(item == menu_START){
                    letterCount = 0;
                    playerName[0] = '\0';
                    GoTo(&state, NAME_INPUT);
                }
                else if(item == menu_HOWTO)       { backHover = 0.0f; GoTo(&state, HOW_TO_PLAY); }
                else if(item == menu_LEADERBOARD) { backHover = 0.0f; GoTo(&state, LEADERBOARD); }
                else if(item == menu_CREDITS)     { backHover = 0.0f; GoTo(&state, CREDITS); }
                else if(item == menu_QUIT)        quitRequested = true;
                break;
            }

            case HOW_TO_PLAY:
            case LEADERBOARD:
            case CREDITS:
                if(UpdateBackButton(mouse, dt)) GoTo(&state, MAIN_MENUE);
                break;

 
            case NAME_INPUT:
                if(IsKeyPressed(KEY_ESCAPE)){ GoTo(&state, MAIN_MENUE); break; }
                UpdateNameInput(playerName, &letterCount, &state);
                if(state != NAME_INPUT) fadeAlpha = 1.0f;
                break;

            case LEVEL_SELECT:
            {
                int pick = UpdateLevelSelect(mouse, dt);
                if(pick >= 0 && pick <= 2){ current_level = pick + 1; Reset(&ball, &state); }
                else if(pick == 3) GoTo(&state, MAIN_MENUE);
                break;
            }
        }



        SetMouseCursor(cursorOverButton ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);


        UpdateGameCamera(&camera, ball.position);

/*==============================================================================
||                                 GAME DRAWING                                   ||
==============================================================================*/
        BeginDrawing();
        ClearBackground(MixColor(SKY1,SKY2,0.7));

        bool inGame = (state == PLAYING || state == PAUSED || state == DEAD || state == WIN);

        DrawMenuBackground(&levelAssets, ball.texture,inGame);

/*==============================================================================
||                                 MORE CAMERA STUFF                           ||
==============================================================================*/
        if(inGame){
            BeginMode2D(camera);
                DrawTile(&levelAssets);
                Rectangle des = {ball.position.x, ball.position.y, ball.radius*2.0f, ball.radius*2.0f};
                if(state == DEAD){
                    
                    DrawTexturePro(poppedballsprite, (Rectangle){0, 0, (float)poppedballsprite.width, (float)poppedballsprite.height},
                                   des, (v2){ball.radius, ball.radius}, 0.0f, WHITE);
                }else {
                    if(ball.type == Ball_normal) ball.texture = ballsprite;
                    else if(ball.type == Ball_pumped) ball.texture = pumpedballsprite;
                    DrawTexturePro(ball.texture, src, des, (v2){ball.radius, ball.radius}, ball.rotation, WHITE);
                }
                
                DrawSpider(spidersprite);
            EndMode2D();

            DrawText(TextFormat("Player: %s", playerName), 20, 20, 22, WHITE);
            DrawText(TextFormat("Score: %d", score), BASE_W - 220, 20, 22, WHITE);

            if(state == PLAYING || state == PAUSED) DrawButtonSprite(pausesprite, &PauseButton);
        }

        if(state == NAME_INPUT){
            DrawRectangle(0, 0, BASE_W, BASE_H, (Color){ 20, 20, 30, 170 });
            DrawText("ENTER YOUR NAME:", BASE_W / 2 - 160, 220, 30, RAYWHITE);

            Rectangle textBox = { BASE_W / 2 - 175, 280, 350, 50 };
            DrawRectangleRec(textBox, LIGHTGRAY);
            DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

            DrawText(playerName, (int)textBox.x + 15, (int)textBox.y + 12, 28, MAROON);
            DrawText("Press ENTER to continue", BASE_W / 2 - 150, 360, 20, GRAY);
        }

        if(state == PAUSED){
            DrawRectangleRec(MenueRect, RAYWHITE);
            DrawText(TextFormat("LEVEL %d", current_level), MenueRect.x + 150, MenueRect.y + 20, 60, BLACK);
            DrawButtonSprite(resumebuttonsprite, &ResumeButton);
            DrawButtonSprite(retrybuttonsprite, &RetryButton);
            DrawButtonSprite(homebuttonsprite, &HomeButton);
            if(sound) DrawButtonSprite(soundonsprite, &SoundOnOffButton);
            else DrawButtonSprite(soundoffsprite, &SoundOnOffButton);
        }


        if(state == MAIN_MENUE)  DrawMainMenu(logo, ball.texture, scoreHistory, scoreCount);
        if(state == HOW_TO_PLAY) DrawHowToPlay(&levelAssets, ball.texture, spidersprite);
        if(state == LEADERBOARD) DrawLeaderboard(scoreHistory, scoreCount, playerName);
        if(state == CREDITS)     DrawCredits();

        if(state == LEVEL_SELECT) DrawLevelSelect(ball.texture);

        if(state == DEAD || state == WIN){
            int best = (scoreCount > 0) ? scoreHistory[0].score : score;
            DrawEndBox(state == WIN, current_level, score, best, score > bestBeforeRun);
        }


        if(fadeAlpha > 0.0f) DrawRectangle(0, 0, BASE_W, BASE_H, Fade(BLACK, fadeAlpha));
        EndDrawing();
    }

/*==============================================================================
||                             SPRITES & TEXTURES DEINIT                      ||
==============================================================================*/
    if(!uiFontIsDefault) UnloadFont(uiFont);
    UnloadTexture(ball.texture);
    UnloadTexture(levelAssets.brick);
    UnloadTexture(levelAssets.spike);
    UnloadTexture(levelAssets.spring);
    UnloadTexture(levelAssets.ring);
    UnloadTexture(levelAssets.goal);
    UnloadTexture(retrybuttonsprite);
    UnloadTexture(resumebuttonsprite);
    UnloadTexture(homebuttonsprite);
    UnloadTexture(logo);
    UnloadTexture(pausesprite);
    UnloadTexture(levelselectsprite);
    UnloadTexture(spidersprite);
    UnloadTexture(poppedballsprite);   
    UnloadSound(coincollectaudio);
    UnloadSound(ballpoppedaudio);
    UnloadSound(levelpassedaudio);
    UnloadTexture(soundonsprite);
    UnloadTexture(soundoffsprite);
    UnloadTexture(pumpedballsprite);
    UnloadTexture(levelAssets.pumper);
    UnloadMusicStream(menuemusic);
    
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
