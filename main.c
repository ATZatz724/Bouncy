#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BASE_W 1280
#define BASE_H 720
#define GRAVITY 1500.0f
#define FLOOR_Y 650.0f
#define RESTITUTION 0.4f
#define PLATFORMS 5
#define MAX_ACC 900.0f
#define MAX_SPEED 260.0f
#define FRICTION 0.9f
#define JUMP -650.0f
#define SPIDER_SPEED 60
#define MAX_LEN 16
#define SCORES "highscore.txt"
#define MAX_SCORES 100

#define TILE_SIZE 32.0f
#define LEVEL_ONE_ROW 8
#define LEVEL_ONE_COL 105
#define LEVEL_TWO_ROW 22
#define LEVEL_TWO_COL 147
#define LEVEL_THREE_ROW 36
#define LEVEL_THREE_COL 134
#define MAP_WIDTH_PX  (LEVEL_ONE_COL * TILE_SIZE)  
#define MAP_HEIGHT_PX (LEVEL_ONE_ROW * TILE_SIZE)



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

typedef enum platformtype{
    PT_NORMAL,
    PT_SPRING,
    PT_SPIKE,
    PT_GOAL
}platformtype;

typedef enum Gamestate{
    PLAYING,
    DEAD,
    WIN, 
    PAUSED,
    MAIN_MENUE,
    LEVEL_SELECT,
    NAME_INPUT
}Gamestate;


typedef struct Levelasset{
    Texture2D brick;
    Texture2D spike;
    Texture2D spring;
    Texture2D ring;
    Texture2D goal;

}Levelasset;



int spider_num[] = {7,0};






typedef struct spider{
    Rectangle rect;
    int speed;
}spider;

const spider level_two_spiders[] = 
{
    {
        .rect = (Rectangle){2*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2, 2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){4*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2, 2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){51*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){57*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){63*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){97*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){91*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    }
};

spider level_two_spiders_buffer[] =
{
    {
        .rect = (Rectangle){2*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2, 2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){4*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2, 2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){51*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){57*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){63*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){97*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    },
    {
        .rect = (Rectangle){91*TILE_SIZE+2, 15*TILE_SIZE+2, 2*TILE_SIZE-2,2*TILE_SIZE-2},
        .speed = SPIDER_SPEED
    }
};

spider *all_spiders[] = 
{
    level_two_spiders_buffer
};






const char *level_one_map[LEVEL_ONE_ROW]={ //[MAP_COL]
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
    "11111000001111110011111111111111111111111111111111111111111111111111111111111111111111111111000111111111111111111111111111111111111111",
    "11000000000000000000000000000001100001100000000000004000004000011111104001111111111111111111000110000000000000110000000000000000000000",
    "11000000000000000000000000000001100001100000000000000000000000001111000000111111111111111111000110000000000000110000000000000000000000",
    "11000011000000000000000000000000000000000000000000000000000000000111000000111111111111111111000110000000000000110000000000000000000000",
    "11000040001100000000001111000000000000000000110000000000000000000011000000111111111111111111000110000000000000110000000000000000000000",
    "11000000001110000000011111000001100001100000111000000000000000000000000000111111111111111111000110000000000000110000000000000000000000",
    "11000000001111000000111111000001100001140004111100000004000000000000000000111111111111111111000110000000000000110000000000000000000000",
    "11111111111111111111111111111111100001111111111111111111111111111111000000111111111111111111000110000000000000110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111000111000000001111110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111000111000000001111110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111000110000000000111110000000000000000000000",
    "00000000000000000000001133333333333333330000110000000000000000000011100000111111111111111111000110000000000011110000000000000000000000",
    "00000000000000000000001120000000000000000000110000000000000000000011100000111111111111111111000111000000000001110000000000000000000000",
    "00000000000000000000001100000000000000000000110000000000000000000011100000111111111111111111000111100000000000110000000000000000000000",
    "00000000000000000000001111111111111111111111110000000000000000000011100000111111111111111111000111110000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111110000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111100000000000110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111000000000001110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000110000000000011110000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111111111111111111111111111111111111",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000110000000000000110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000110000000000000110000000000000000020011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000110000000000000110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000111111111111111111000111111111111111110000000000000000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000000000000000000000000000000000000000000000000000400000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011100000000000000000000000000000000000000000000000000000110000000011",
    "00000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111111",
};





const char **levels[3] = {level_one_map, level_two_map, level_three_map};

int current_level = 1;
int current_row = LEVEL_ONE_ROW;
int current_col = LEVEL_ONE_COL;

char map1[LEVEL_ONE_ROW][LEVEL_ONE_COL];
char map2[LEVEL_TWO_ROW][LEVEL_TWO_COL];
char map3[LEVEL_THREE_ROW][LEVEL_THREE_COL];

// struct level_maps{
//     char * zerozero;
//     int row;
//     int col;
// };

// struct level_maps maps[2] = {
//     {
//     .zerozero = &map[0][0],
//     .row = LEVEL_ONE_ROW,
//     .col = LEVEL_ONE_COL,
//     },
//     {
//         .zerozero = &map2[0][0],
//         .row = LEVEL_TWO_ROW,
//         .col = LEVEL_TWO_COL
//     }
// };

char *maps[3] = {&map1[0][0], &map2[0][0], &map3[0][0]};




/* scoring */ int score = 0;
bool scoresaved=false;

v2 starting_positions[3] = {
    (v2){2.5f*TILE_SIZE,1.5f*TILE_SIZE},
    (v2){ 2.5f * TILE_SIZE + 160, 13.5f * TILE_SIZE + 100},
    (v2){ 38.5f * TILE_SIZE, 1.5f * TILE_SIZE }
};

void Reset(Ball *b,Gamestate *state){

    b->position=starting_positions[current_level - 1];
    b->velocity=(v2){0.0f,0.0f};
    b->rotation = 0.0f;
    for(int r=0; r < current_row; r++){
        for(int c=0; c < current_col; c++){
            maps[current_level - 1][r * current_col + c]=levels[current_level-1][r][c];
        }
    }

    //reset spiders
    for(int i = 0;i<spider_num[current_level - 2];i++)
    {
        if(current_level == 2)
        {
            all_spiders[current_level - 2][i].rect.x = level_two_spiders[i].rect.x;
            all_spiders[current_level - 2][i].rect.y = level_two_spiders[i].rect.y;
            all_spiders[current_level - 2][i].speed = level_two_spiders[i].speed;
        }
        else if(current_level == 3)
        {

        }
    }


    score = 0;
    *state = PLAYING;
}

bool coin_collected = false;
bool ball_popped = false;
bool level_passed = false;
void BallPlatformCollision(Ball *b, Rectangle tilerect, char tiletype, Gamestate *state){

    float ClosestX = Clamp(b->position.x,tilerect.x,tilerect.x+tilerect.width);
    float ClosestY = Clamp(b->position.y,tilerect.y,tilerect.y+tilerect.height);

    float dx = b->position.x - ClosestX;
    float dy = b->position.y - ClosestY;

    float dis_sq = dx*dx +dy*dy;
    if(b->radius*b->radius <= dis_sq ){
        return;
    }
    float dis = sqrtf(dis_sq);
    v2 normal = (dis > 0.00001f)?(v2){dx/dis,dy/dis}:(v2){0,-1};
    float penetration = b->radius - dis;
    b->position.x += normal.x*penetration;
    b->position.y += normal.y*penetration;

    if(tiletype == '2') {*state = DEAD; return;}
    if(tiletype == '5') {*state = WIN; level_passed = true;return;}

    float Normalvel = b->velocity.x*normal.x + b->velocity.y*normal.y;

    if(Normalvel < 0){
        if(tiletype=='3' && normal.y < -0.5f){
            b->velocity.y = -900.0f;
        }else{
            b->velocity.x -= (1+RESTITUTION)*Normalvel*normal.x;
            b->velocity.y -= (1+RESTITUTION)*Normalvel*normal.y;
        }
        
    }

    if(normal.y < -0.5f){
        b->grounded=true;
    }

}
void UpdateBall(Ball *b, float dt,Gamestate *state)
{

    
    if(IsKeyDown(KEY_RIGHT)){
        b->velocity.x += MAX_ACC*dt;
    }
    if(IsKeyDown(KEY_LEFT)){
        b->velocity.x -= MAX_ACC*dt;
    }
    if(!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)){
        b->velocity.x *=FRICTION;
    }
    

    if(b->velocity.x > MAX_SPEED){
        b->velocity.x = MAX_SPEED;
    }
    if(b->velocity.x < -MAX_SPEED){
        b->velocity.x = -MAX_SPEED;
    }
    b->velocity.y += GRAVITY*dt;

    

    b->position.x += b->velocity.x*dt;
    b->position.y += b->velocity.y*dt;

    b->rotation += (b->velocity.x/b->radius)*RAD2DEG*dt;
    b->grounded = false;

    for(int r=0; r < current_row; r++){
        for(int c=0; c < current_col; c++){
            char tile= maps[current_level - 1][r * current_col + c];
            if(tile == '0') continue;
            Rectangle tilerect  = {c*TILE_SIZE,r*TILE_SIZE,TILE_SIZE,TILE_SIZE};

            if(tile=='4'){
                if(CheckCollisionCircleRec(b->position,b->radius,tilerect)){
                    maps[current_level - 1][r * current_col + c] = '0';
                    score+=500;
                    coin_collected = true;
                }
                continue;
            }
            if(tile == '2') {
            Rectangle spikehit = {
                tilerect.x + (TILE_SIZE * 0.25f), 
                tilerect.y + (TILE_SIZE * 0.25f), 
                TILE_SIZE * 0.45f,                 
                TILE_SIZE * 0.45f                  
            };

            if(CheckCollisionCircleRec(b->position, b->radius, spikehit)) {
                *state = DEAD;
                ball_popped = true;

            }
            continue;
        }
            BallPlatformCollision(b,tilerect,tile,state);




        }
    }
    
    if(IsKeyPressed(KEY_UP) && b->grounded){
        b->velocity.y = JUMP;
        b->grounded = false;
    }

    

   
    // if (b->position.x - b->radius < 0) {
    //     b->position.x = b->radius;
    //     b->velocity.x = 0;
    // }
    // if (b->position.x + b->radius > current_col*TILE_SIZE - TILE_SIZE) {//was max width
    //     b->position.x = current_col*TILE_SIZE - TILE_SIZE - b->radius;//wasmax width
    //     b->velocity.x = 0;
    // }

    // if(b->position.y > current_row*TILE_SIZE+50.0f){//was maxheight
    //     *state = DEAD;
    // }


    ///Extra
    // if(b->position.y > BASE_H){
    //     b->position.y =-10;
    //     b->position.x = 5;
    // }

}




void DrawTile(Levelasset *lvl){
    for(int r=0; r < current_row; r++){
        for(int c=0; c < current_col; c++){
            char tile = maps[current_level - 1][r * current_col + c];


            Texture2D *tex=NULL;

            switch(tile){
                case '1':tex=&lvl->brick;break;
                case '2':tex=&lvl->spike;break;
                case '3':tex=&lvl->spring;break;
                case '4':tex=&lvl->ring;break;
                case '5':tex=&lvl->goal;break;
                default:break;
            }
            if(tex != NULL){
                DrawTexturePro(*tex,(Rectangle){0.0f,0.0f,(float)tex->width,(float)tex->height},(Rectangle){c*TILE_SIZE,r*TILE_SIZE,TILE_SIZE,TILE_SIZE},(v2){0.0f,0.0f},0.0,WHITE);
            }
        }
    }
}

void DrawSpider(Texture2D spidersprite)
{
    if(current_level > 1)
    {
        for(int i = 0;i<spider_num[current_level - 2];i++)
        {
            DrawTexturePro(spidersprite, (Rectangle){0.0f, 0.0f, spidersprite.width, spidersprite.height}, all_spiders[current_level - 2][i].rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
}

void UpdateSpider(Gamestate *state,Ball b,float dt)
{
    if(current_level > 1 && *state == PLAYING)
    {
        for(int i = 0;i<spider_num[current_level - 2];i++)
        {
            if(CheckCollisionCircleRec(b.position, b.radius, all_spiders[current_level - 2][i].rect))
            {
                *state = DEAD;
                ball_popped = true;
            }
            all_spiders[current_level - 2][i].rect.y += all_spiders[current_level - 2][i].speed*dt;

            for(int r = 0;r<current_row;r++)
            {
                for(int c = 0;c<current_col;c++)
                {
                    Rectangle tilerect  = {c*TILE_SIZE,r*TILE_SIZE,TILE_SIZE,TILE_SIZE};
                    
                    if(CheckCollisionRecs(tilerect, all_spiders[current_level - 2][i].rect) && maps[current_level - 1][r * current_col + c] == '1')
                    {
                        all_spiders[current_level - 2][i].speed *= -1;
                        if(all_spiders[current_level - 2][i].rect.y - tilerect.y < -all_spiders[current_level - 2][i].rect.y + tilerect.y)
                            all_spiders[current_level - 2][i].rect.y = tilerect.y- 2*TILE_SIZE - 2;
                        else 
                            all_spiders[current_level - 2][i].rect.y = tilerect.y + TILE_SIZE + 2;
                    }
                }
            }
        }
    }
}

// Score file Handling
ScoreRecord history[MAX_SCORES];
int scorecount=0;
void LoadScoreHistory(ScoreRecord history[], int *count) {
    *count = 0;
    FILE *file = fopen(SCORES, "r");
    if (file != NULL) {
        while (*count < MAX_SCORES && fscanf(file, "%15s %d", history[*count].name, &history[*count].score) == 2) {
            (*count)++;
        }
        fclose(file);
    }
}

void SaveScoreHistory(ScoreRecord history[], int count) {
    FILE *file = fopen(SCORES, "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %d\n", history[i].name, history[i].score);
        }
        fclose(file);
    }
}

void AddScoreRecord(ScoreRecord history[], int *count, const char *name, int newScore) {
    if (*count < MAX_SCORES) {
        strncpy(history[*count].name, (strlen(name) > 0) ? name : "Player", MAX_LEN);
        history[*count].name[MAX_LEN] = '\0';
        history[*count].score = newScore;
        (*count)++;
    } else {
        // Replace lowest score if filled
        if (newScore > history[*count - 1].score) {
            strncpy(history[*count - 1].name, (strlen(name) > 0) ? name : "Player", MAX_LEN);
            history[*count - 1].name[MAX_LEN] = '\0';
            history[*count - 1].score = newScore;
        }
    }

    // Sort descending by score
    for (int i = 0; i < *count - 1; i++) {
        for (int j = i + 1; j < *count; j++) {
            if (history[j].score > history[i].score) {
                ScoreRecord temp = history[i];
                history[i] = history[j];
                history[j] = temp;
            }
        }
    }

    SaveScoreHistory(history, *count);
}

void DrawScoreHistoryUI(ScoreRecord history[], int count, int x, int y) {
    DrawText("LEADERBOARD", x, y, 22, GOLD);
    DrawLine(x, y + 25, x + 250, y + 25, GOLD);
    
    int displayLimit = (count < 5) ? count : 5; // Show top 5
    if (displayLimit == 0) {
        DrawText("No records yet", x, y + 35, 18, GRAY);
        return;
    }

    for (int i = 0; i < displayLimit; i++) {
        DrawText(TextFormat("%d. %-10s %d", i + 1, history[i].name, history[i].score), x, y + 35 + (i * 24), 18, RAYWHITE);
    }
}
void LoadHighscore(ScoreRecord *record){
    FILE *file = fopen(SCORES,"r");
    if (file != NULL) {
        if (fscanf(file, "%15s %d", record->name, &record->score) != 2) {
            strcpy(record->name, "None");
            record->score = 0;
        }
        fclose(file);
    } else {
        strcpy(record->name, "None");
        record->score = 0;
    }
}

void SaveHighScoreIfBest(const char *name, int currentScore, ScoreRecord *record) {
    if (currentScore > record->score) {
        record->score = currentScore;
        strncpy(record->name, (strlen(name) > 0) ? name : "Player", MAX_LEN);
        record->name[MAX_LEN] = '\0';

        FILE *file = fopen(SCORES, "w");
        if (file != NULL) {
            fprintf(file, "%s %d\n", record->name, record->score);
            fclose(file);
        }
    }
}

void UpdateNameInput(char *nameBuffer, int *letterCount, Gamestate *state) {
    int key = GetCharPressed();

    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (*letterCount < MAX_LEN)) {
            nameBuffer[*letterCount] = (char)key;
            nameBuffer[*letterCount + 1] = '\0';
            (*letterCount)++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        (*letterCount)--;
        if (*letterCount < 0) *letterCount = 0;
        nameBuffer[*letterCount] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && *letterCount > 0) {
        *state = LEVEL_SELECT;
    }
}

typedef enum ButtonState
{
    NORMAL,
    PRESSED,
}ButtonState;

typedef struct Button
{
    Rectangle rect;
    ButtonState state;
}Button;



Rectangle MenueRect = (Rectangle){0.27*BASE_W, 0.08*BASE_H, 0.46*BASE_W, 0.75*BASE_H};


Button PauseButton = {
    .rect = (Rectangle){0.94*BASE_W, 0.05*BASE_H,0.04*BASE_W, 0.04*BASE_W},
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
    .rect = (Rectangle){ 0.30*BASE_W, 0.54*BASE_H, 0.21*BASE_H, 0.21*BASE_H},
    .state = NORMAL
};

Button MenuePlayButton = {
    .rect = (Rectangle){ BASE_W*0.5f - 130.0f, 0.60f * BASE_H, 240.0f, 100.0f },
    .state = NORMAL
};

Button LevelSelectButton = {
    .rect = (Rectangle){ 0.46*BASE_W, 0.54*BASE_H, 0.21*BASE_H, 0.21*BASE_H},
    .state = NORMAL
};



Rectangle LevelSelectionRect = (Rectangle){ 0.3125f * BASE_W, 0.2222f * BASE_H, 0.375f * BASE_W - 30, 0.5556f * BASE_H - 100};


Button Level1Button = {
    .rect = (Rectangle){ 0.334f * BASE_W, 0.278f * BASE_H, 0.16f * BASE_H, 0.16f * BASE_H },
    .state = NORMAL
};

Button Level2Button = {
    .rect = (Rectangle){ 0.443f * BASE_W, 0.278f * BASE_H, 0.16f * BASE_H, 0.16f * BASE_H },
    .state = NORMAL
};

Button Level3Button = {
    .rect = (Rectangle){ 0.552f * BASE_W, 0.278f * BASE_H, 0.16f * BASE_H, 0.16f * BASE_H },
    .state = NORMAL
};


Button LevelBackButton = {
    .rect = (Rectangle){ 0.334f * BASE_W, 0.625f * BASE_H - 90, 0.096f * BASE_H, 0.096f * BASE_H },
    .state = NORMAL
};
int main(void){
// INIT
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(BASE_W,BASE_H,"Bounce Classic");
    InitAudioDevice();
    SetTargetFPS(60);

    ScoreRecord scoreHistory[MAX_SCORES];
    int scoreCount = 0;
    LoadScoreHistory(scoreHistory, &scoreCount);


    char playerName[MAX_LEN + 1] = "\0";
    int letterCount = 0;
    
    Texture2D ballsprite = LoadTexture("assets/images/red-ball.png");


    Texture2D retrybuttonsprite = LoadTexture("assets/images/retry-button.png");
    Texture2D resumebuttonsprite = LoadTexture("assets/images/resume.png");
    Texture2D homebuttonsprite = LoadTexture("assets/images/home.png");
    Texture2D logo = LoadTexture("assets/images/title-logo.png");
    Texture2D menueplaysprite = LoadTexture("assets/images/play-button.png");
    Texture2D pausesprite = LoadTexture("assets/images/PauseButton.png");
    Texture2D levelselectsprite = LoadTexture("assets/images/level-select-button.png");
    Texture2D spidersprite = LoadTexture("assets/images/spider.png");
    Texture2D poppedballsprite = LoadTexture("assets/images/pop-red-ball.png");
    Texture2D backbuttonsprite = LoadTexture("assets/images/back-button.png");
    Sound coincollectaudio = LoadSound("assets/audios/coins.mp3");
    Sound ballpoppedaudio = LoadSound("assets/audios/pop.mp3");
    Sound levelpassedaudio = LoadSound("assets/audios/universfield-next-level-114480.mp3");

    Font levelfont = LoadFont("assets/fonts/Zhetia.otf");

    Ball ball = {
                .radius=14.0f,
                .rotation = 0.0f,
                .texture = ballsprite
            
            };  
    Levelasset levelAssets = {
        .brick  = LoadTexture("assets/images/brick.png"),
        .spike  = LoadTexture("assets/images/tile_spike.png"),
        .spring = LoadTexture("assets/images/spring.png"),
        .ring   = LoadTexture("assets/images/tile_ring.png"),
        .goal   = LoadTexture("assets/images/tile_goal.png")
    };
    Gamestate state = MAIN_MENUE;
    // Reset(&ball);
    Rectangle src= {0.0f,0.0f,(float)ball.texture.width,(float)ball.texture.height};
    Camera2D camera = { 0 };
    camera.offset = (v2){ BASE_W / 2.0f, BASE_H / 2.0f };
    camera.zoom = 1.6f;
    
    



    while(!WindowShouldClose()){

        float dt = GetFrameTime();
<<<<<<< HEAD
        if(state ==PLAYING){
            UpdateBall(&ball,dt,&state); 
            if((state==DEAD ||  state==WIN ) && !scoresaved){
                AddScoreRecord(scoreHistory,&scoreCount,playerName,score);
                scoresaved=true;
            }
        }else if(IsKeyPressed(KEY_R)){
            Reset(&ball, &state);
        }
=======
        
>>>>>>> 1bc436932c239b92bfa302bf724fb615a0bd5588

//Camera Update
        camera.target = ball.position;
        float min_camera_x = BASE_W / (2.0f * camera.zoom);
        float max_camera_x = current_col*TILE_SIZE - BASE_W / (2.0f * camera.zoom);// max width

        
        // if (max_camera_x < min_camera_x) {
        //     camera.target.x = MAP_WIDTH_PX / 2.0f;
        // } else 
        
        camera.target.x = Clamp(ball.position.x, min_camera_x, max_camera_x);

        float min_camera_y = BASE_H / (2.0f * camera.zoom);
        float max_camera_y = current_col*TILE_SIZE - BASE_H / (2.0f * camera.zoom);// max width
        camera.target.y = Clamp(ball.position.y, min_camera_y, max_camera_y);
        



        
        // camera.target.y = ball.position.y;
        // float map_center_y = (current_row * TILE_SIZE) / 2.0f;
        // //max height
        // camera.target.y = map_center_y;

        
    
//Button Update
        v2 mouse = GetMousePosition();

        if(CheckCollisionPointRec(mouse, PauseButton.rect))
        {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // PauseButton.state = PRESSED;
                state = PAUSED;
            }
            // else PauseButton.state = NORMAL;
        }
        // else PauseButton.state = NORMAL;


        if(state == PAUSED)
        {
            if(CheckCollisionPointRec(mouse, ResumeButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    // ResumeButton.state = PRESSED;
                    state = PLAYING;
                    // ResumeButton.state = NORMAL;
                }
                // else ResumeButton.state = NORMAL;
            }
            // else ResumeButton.state = NORMAL;

            if(CheckCollisionPointRec(mouse, RetryButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    // RetryButton.state = PRESSED;
                    Reset(&ball,&state);
                }
                // else RetryButton.state = NORMAL;
            }
            // else RetryButton.state = NORMAL;

            if(CheckCollisionPointRec(mouse, HomeButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    state = LEVEL_SELECT;
                    // HomeButton.state = PRESSED;
                }
                // else HomeButton.state = NORMAL;
            }
            // else HomeButton.state = NORMAL;

            // if(CheckCollisionPointRec(mouse, LevelSelectButton.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            // {
            //     state = LEVEL_SELECT;
            // }


        }
        else if(state == MAIN_MENUE)
        {
            if(CheckCollisionPointRec(mouse, MenuePlayButton.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                state = NAME_INPUT;
                letterCount = 0;
                playerName[0] = '\0';
            }
        }
        else if (state == NAME_INPUT) {
            UpdateNameInput(playerName, &letterCount, &state);
            if (state == PLAYING) {
                Reset(&ball,&state);

                state = LEVEL_SELECT;

            }
        }
        else if(state == LEVEL_SELECT)
        {
            if(CheckCollisionPointRec(mouse, Level1Button.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                current_level = 1;
                current_row = LEVEL_ONE_ROW;
                current_col = LEVEL_ONE_COL;
                Reset(&ball,&state);

            }
            else if(CheckCollisionPointRec(mouse, Level2Button.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                current_level = 2;
                current_row = LEVEL_TWO_ROW;
                current_col = LEVEL_TWO_COL;
                    Reset(&ball, &state);
            }
            else if(CheckCollisionPointRec(mouse, Level3Button.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                current_level = 3;
                current_row = LEVEL_THREE_ROW;
                current_col = LEVEL_THREE_COL;
                    Reset(&ball, &state);
            }
            else if(CheckCollisionPointRec(mouse, LevelBackButton.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                state = MAIN_MENUE;
            }
        }
<<<<<<< HEAD
        // else if (state == PLAYING) {
        //     UpdateBall(&ball, dt, &state);
=======
        else if (state == PLAYING) {
            UpdateBall(&ball, dt, &state);
            UpdateSpider(&state, ball, dt);
            if(coin_collected == true)
            {
                PlaySound(coincollectaudio);
                coin_collected = false;
            }
            if(ball_popped == true)
            {
                PlaySound(ballpoppedaudio);
                ball_popped = false;
            }
            if(level_passed == true)
            {
                PlaySound(levelpassedaudio);
                level_passed = false;

            }
>>>>>>> 1bc436932c239b92bfa302bf724fb615a0bd5588
            
        //     // Check for game end & save score
        //     if (state == DEAD || state == WIN) {
        //         SaveHighScoreIfBest(playerName, score, &highScore);
        //     }
        // }
        // else if (IsKeyPressed(KEY_R)) {
        //     Reset(&ball,&state);
        //     state = PLAYING;
        // }

        
        
         

//DRAW

        BeginDrawing();
        ClearBackground((Color){174, 206, 240, 255});
        if (state == NAME_INPUT) {
            DrawRectangle(0, 0, BASE_W, BASE_H, (Color){ 20, 20, 30, 230 });
            DrawText("ENTER YOUR NAME:", BASE_W / 2 - 160, 220, 30, RAYWHITE);

            Rectangle textBox = { BASE_W / 2 - 175, 280, 350, 50 };
            DrawRectangleRec(textBox, LIGHTGRAY);
            DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

            DrawText(playerName, (int)textBox.x + 15, (int)textBox.y + 12, 28, MAROON);
            DrawText("Press ENTER to Start Playing", BASE_W / 2 - 180, 360, 20, GRAY);
        }
    
        if(state != MAIN_MENUE && state != LEVEL_SELECT && state != NAME_INPUT)

        {
            BeginMode2D(camera);
                DrawTile(&levelAssets);
                Rectangle des = {ball.position.x,ball.position.y,ball.radius*2.0f,ball.radius*2.0f};
                if(state == PLAYING || state == WIN)
                    DrawTexturePro(ball.texture,src,des,(v2){ball.radius, ball.radius},ball.rotation,WHITE);
                else if(state == DEAD)
                {
                    DrawTexturePro(poppedballsprite,src,des,(v2){ball.radius, ball.radius},0,WHITE);
                }

                DrawSpider(spidersprite);
                

            EndMode2D();


            DrawText(TextFormat("Player: %s", playerName), 20, 20, 22, WHITE);
            DrawText(TextFormat("Score: %d", score), BASE_W - 220, 20, 22, WHITE);
            
        }

        if(state != MAIN_MENUE) DrawTexturePro(pausesprite, (Rectangle){0.0f, 0.0f, pausesprite.width, pausesprite.height}, PauseButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
        
        if(state == PAUSED)
        {
            DrawRectangleRec(MenueRect, RAYWHITE);
            DrawText(TextFormat("LEVEL %d",current_level), MenueRect.x + 150, MenueRect.y + 20, 60, BLACK);
            DrawTexturePro(resumebuttonsprite, (Rectangle){0.0f, 0.0f, resumebuttonsprite.width, resumebuttonsprite.height}, ResumeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(retrybuttonsprite, (Rectangle){0.0f, 0.0f, retrybuttonsprite.width, retrybuttonsprite.height}, RetryButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(homebuttonsprite, (Rectangle){0.0f, 0.0f, homebuttonsprite.width, homebuttonsprite.height}, HomeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
<<<<<<< HEAD
            // DrawTexturePro(levelselectsprite, (Rectangle){0.0f, 0.0f, levelselectsprite.width, levelselectsprite.height}, LevelSelectButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawScoreHistoryUI(scoreHistory,scorecount,50,50);
=======


>>>>>>> 1bc436932c239b92bfa302bf724fb615a0bd5588
        }

        if(state == MAIN_MENUE)
        {

            float x = 0;
            while(x + 100 < BASE_W)
            {
                DrawTexturePro(
                    levelAssets.brick,
                    (Rectangle){0.0f,0.0f,levelAssets.brick.width, levelAssets.brick.height},
                    (Rectangle){x, BASE_H - 100, 100, 100},
                    (v2){0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
                x += 100;
            }
            DrawTexturePro(
                    levelAssets.brick,
                    (Rectangle){0.0f,0.0f,levelAssets.brick.width, levelAssets.brick.height},
                    (Rectangle){x, BASE_H - 100, 100, 100},
                    (v2){0.0f, 0.0f},
                    0.0f,
                    WHITE
                );

            DrawTexturePro(logo, 
                (Rectangle){0.0f, 0.0f, logo.width, logo.height - 50}, 
                (Rectangle){(BASE_W - 400.0f)/2, (BASE_H - 200.0f)/2 - 50.0f, 400.0f, 200.0f}, 
                (v2){0.0f, 0.0f}, 
                0.0f, 
                WHITE
            );

           

            DrawTexturePro(menueplaysprite, 
            (Rectangle){0.0f, 0.0f, menueplaysprite.width, menueplaysprite.height},
            MenuePlayButton.rect,
            (v2){0.0f, 0.0f},
            0.0f,
            WHITE
            );

            DrawScoreHistoryUI(scoreHistory, scoreCount, 50, 50);
            // if(IsKeyDown(KEY_ENTER)) 
            // {
            // state = PLAYING;
            // Reset(&ball);
            // }
        }

        if(state == LEVEL_SELECT)
        {
                float x = 0;
                while(x + 100 < BASE_W)
                {
                    DrawTexturePro(
                        levelAssets.brick,
                        (Rectangle){0.0f,0.0f,levelAssets.brick.width, levelAssets.brick.height},
                        (Rectangle){x, BASE_H - 100, 100, 100},
                        (v2){0.0f, 0.0f},
                        0.0f,
                        WHITE
                    );
                    x += 100;
                }
                DrawTexturePro(
                        levelAssets.brick,
                        (Rectangle){0.0f,0.0f,levelAssets.brick.width, levelAssets.brick.height},
                        (Rectangle){x, BASE_H - 100, 100, 100},
                        (v2){0.0f, 0.0f},
                        0.0f,
                        WHITE
                    );


                DrawRectangleRec(LevelSelectionRect, WHITE);
                DrawRectangleLinesEx(LevelSelectionRect, 3, BLACK);

                DrawTexturePro(levelAssets.brick,(Rectangle){0.0f,0.0f,levelAssets.brick.width,levelAssets.brick.height},Level1Button.rect,(v2){0.0f,0.0f},0.0f,WHITE);
                DrawTexturePro(levelAssets.brick,(Rectangle){0.0f,0.0f,levelAssets.brick.width,levelAssets.brick.height},Level2Button.rect,(v2){0.0f,0.0f},0.0f,WHITE);
                DrawTexturePro(levelAssets.brick,(Rectangle){0.0f,0.0f,levelAssets.brick.width,levelAssets.brick.height},Level3Button.rect,(v2){0.0f,0.0f},0.0f,WHITE);
                DrawTexturePro(backbuttonsprite,(Rectangle){0.0f,0.0f,backbuttonsprite.width,backbuttonsprite.height},LevelBackButton.rect,(v2){0.0f,0.0f},0.0f,WHITE);

                DrawTextEx(GetFontDefault(), "1", (v2){Level1Button.rect.x + 47,Level1Button.rect.y + 10},100,1.0f,WHITE);
                DrawTextEx(GetFontDefault(), "2", (v2){Level2Button.rect.x + 37,Level2Button.rect.y + 10},100,1.0f,WHITE);
                DrawTextEx(GetFontDefault(), "3", (v2){Level3Button.rect.x + 37,Level3Button.rect.y + 10},100,1.0f,WHITE);

                


        }

        if(state == DEAD){
            DrawText("GAME OVER",480,20,24,RED);
            DrawScoreHistoryUI(scoreHistory, scoreCount, 50, 100);
        } 
        if(state == WIN) {
            DrawText("LEVEL CLEARED",480,20,24,GOLD);
            DrawScoreHistoryUI(scoreHistory, scoreCount, 50, 100);
        }    
        EndDrawing();

    }

//DEINIT
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
    UnloadTexture(menueplaysprite);
    UnloadTexture(pausesprite);
    UnloadTexture(levelselectsprite);
    UnloadTexture(spidersprite);
    UnloadSound(coincollectaudio);
    UnloadTexture(poppedballsprite);
    UnloadTexture(backbuttonsprite);
    UnloadFont(levelfont);
    UnloadSound(ballpoppedaudio);
    UnloadSound(levelpassedaudio);
    CloseAudioDevice();
    CloseWindow();


    return 0;

}
