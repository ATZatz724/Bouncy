#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BASE_W 1280
#define BASE_H 720
#define GRAVITY 1500.0f
#define FLOOR_Y 650.0f
#define RESTITUTION 0.6f
#define PLATFORMS 5
#define MAX_ACC 900.0f
#define MAX_SPEED 260.0f
#define FRICTION 0.9f
#define JUMP -650.0f

#define TILE_SIZE 32.0f

#define MAP_ROW 8
#define MAP_COL 105
// #define MAP_WIDTH_PX  (MAP_COL * TILE_SIZE)  
// #define MAP_HEIGHT_PX (MAP_ROW * TILE_SIZE)
#define MAX_LEN 16
#define SCORES "highscore.txt"

#define LEVEL_ONE_ROW 8
#define LEVEL_ONE_COL 105
#define LEVEL_TWO_ROW 22
#define LEVEL_TWO_COL 147
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



typedef enum Gamestate{
    PLAYING,
    DEAD,
    WIN, 
    PAUSED,
    MAIN_MENUE,
    NAME_INPUT,
    LEVEL_SELECT
}Gamestate;


typedef struct Levelasset{
    Texture2D brick;
    Texture2D spike;
    Texture2D spring;
    Texture2D ring;
    Texture2D goal;

}Levelasset;

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
    "11000000000000011111111100110011000000000111100000000000000000000110000000000000000000040000000000000001100000000040000000000110011000011000110011111",
    "11000000000000011000001100110011000000000111100000000000000000000110000000000000000000000000000000000001100000000000000000000110011001111000000000011",
    "11000000000000011000000000000011000000000111100000000000000000000400000000000000000000011000000000000001100000000011000000000110000000011000000000011",
    "11000000000002011000000000000011000000000111100000000000000000000000000000000000000000011000000000000001100000000011000000000110000000011000000002011",
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",
};


const char **levels[2] = {level_one_map, level_two_map};

int current_level = 1;
int current_row = LEVEL_ONE_ROW;
int current_col = LEVEL_ONE_COL;

char map1[LEVEL_ONE_ROW][LEVEL_ONE_COL];
char map2[LEVEL_TWO_ROW][LEVEL_TWO_COL];

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

char *maps[2] = {&map1[0][0], &map2[0][0]};




/* scoring */ int score = 0;

v2 starting_positions[2] = {
    (v2){2.5f*TILE_SIZE,1.5f*TILE_SIZE},
    (v2){ 2.5f * TILE_SIZE, 13.5f * TILE_SIZE + 100}
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
    score = 0;
    *state = PLAYING;
}


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
    if(tiletype == '5') {*state = WIN; return;}

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

    

   
    if (b->position.x - b->radius < 0) {
        b->position.x = b->radius;
        b->velocity.x = 0;
    }
    if (b->position.x + b->radius > current_col*TILE_SIZE - TILE_SIZE) {//was max width
        b->position.x = current_col*TILE_SIZE - TILE_SIZE - b->radius;//wasmax width
        b->velocity.x = 0;
    }

    if(b->position.y > current_row*TILE_SIZE+50.0f){//was maxheight
        *state = DEAD;
    }
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

// Score file Handling
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
    SetTargetFPS(60);

    ScoreRecord highScore = { "None", 0 };
    LoadHighscore(&highScore);

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
    Ball ball = {
                .radius=14.0f,
                .rotation = 0.0f,
                .texture = ballsprite
            
            };  
    Levelasset levelAssets = {
        .brick  = LoadTexture("assets/images/tile_brick.png"),
        .spike  = LoadTexture("assets/images/tile_spike.png"),
        .spring = LoadTexture("assets/images/tile_spring.png"),
        .ring   = LoadTexture("assets/images/tile_ring.png"),
        .goal   = LoadTexture("assets/images/tile_goal.png")
    };
    Gamestate state = MAIN_MENUE;
    // Reset(&ball);
    Rectangle src= {0.0f,0.0f,(float)ball.texture.width,(float)ball.texture.height};
    Camera2D camera = { 0 };
    camera.offset = (v2){ BASE_W / 2.0f, BASE_H / 2.0f };
    camera.zoom = 1.0f;
    
    



    while(!WindowShouldClose()){
//UPDATE
        float dt = GetFrameTime();
        if(state ==PLAYING){
            UpdateBall(&ball,dt,&state); 
        }else if(IsKeyPressed(KEY_R)){
            Reset(&ball, &state);
        }

//Camera Update
        camera.target = ball.position;
        float min_camera_x = BASE_W / (2.0f * camera.zoom);
        float max_camera_x = current_col*TILE_SIZE - BASE_W / (2.0f * camera.zoom);// max width

        
        // if (max_camera_x < min_camera_x) {
        //     camera.target.x = MAP_WIDTH_PX / 2.0f;
        // } else 
        
        camera.target.x = Clamp(ball.position.x, min_camera_x, max_camera_x);
        

        
        camera.target.y = ball.position.y;
        float map_center_y = (current_row * TILE_SIZE) / 2.0f;
        //max height
        camera.target.y = map_center_y;
    
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
                // Reset(&ball,&state);
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
            else if(CheckCollisionPointRec(mouse, LevelBackButton.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                state = MAIN_MENUE;

            }
        }
        else if (state == PLAYING) {
            UpdateBall(&ball, dt, &state);
            
            // Check for game end & save score
            if (state == DEAD || state == WIN) {
                SaveHighScoreIfBest(playerName, score, &highScore);
            }
        }
        else if (IsKeyPressed(KEY_R)) {
            Reset(&ball,&state);
            state = PLAYING;
        }

        
        
         

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
        if(state != MAIN_MENUE && state != NAME_INPUT)

        if(state != MAIN_MENUE && state != LEVEL_SELECT)

        {
            BeginMode2D(camera);
                DrawTile(&levelAssets);
                Rectangle des = {ball.position.x,ball.position.y,ball.radius*2.0f,ball.radius*2.0f};
                v2 origin = {ball.radius,ball.radius};
                DrawTexturePro(ball.texture,src,des,origin,ball.rotation,WHITE);
            EndMode2D();

            DrawText(TextFormat("Player: %s", playerName), 20, 20, 22, WHITE);
            DrawText(TextFormat("Score: %d", score), BASE_W - 220, 20, 22, WHITE);
            DrawText(TextFormat("High Score: %s (%d)", highScore.name, highScore.score), BASE_W - 420, 50, 20, GOLD);
        }

        if(state != MAIN_MENUE) DrawTexturePro(pausesprite, (Rectangle){0.0f, 0.0f, pausesprite.width, pausesprite.height}, PauseButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
        
        if(state == PAUSED)
        {
            DrawRectangleRec(MenueRect, RAYWHITE);
            DrawText("LEVEL 1", MenueRect.x + 150, MenueRect.y + 20, 60, BLACK);
            DrawTexturePro(resumebuttonsprite, (Rectangle){0.0f, 0.0f, resumebuttonsprite.width, resumebuttonsprite.height}, ResumeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(retrybuttonsprite, (Rectangle){0.0f, 0.0f, retrybuttonsprite.width, retrybuttonsprite.height}, RetryButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(homebuttonsprite, (Rectangle){0.0f, 0.0f, homebuttonsprite.width, homebuttonsprite.height}, HomeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            // DrawTexturePro(levelselectsprite, (Rectangle){0.0f, 0.0f, levelselectsprite.width, levelselectsprite.height}, LevelSelectButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);

        }

        if(state == MAIN_MENUE)
        {
            
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
            // if(IsKeyDown(KEY_ENTER)) 
            // {
            // state = PLAYING;
            // Reset(&ball);
            // }
        }

        if(state == LEVEL_SELECT)
        {
                DrawRectangleRec(LevelSelectionRect, BLACK);
                DrawRectangleRec(Level1Button.rect, WHITE);
                DrawRectangleRec(Level2Button.rect, WHITE);
                DrawRectangleRec(Level3Button.rect, WHITE);
                DrawRectangleRec(LevelBackButton.rect, YELLOW);


        }

        if(state == DEAD) DrawText("GAME OVER",480,20,24,RED);
        if(state == WIN) DrawText("LEVEL CLEARED",480,20,24,GOLD);    
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
    CloseWindow();


    return 0;

}
