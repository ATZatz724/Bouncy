#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

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
#define MAP_WIDTH_PX  (MAP_COL * TILE_SIZE)  
#define MAP_HEIGHT_PX (MAP_ROW * TILE_SIZE)

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
    MAIN_MENUE
}Gamestate;


typedef struct Levelasset{
    Texture2D brick;
    Texture2D spike;
    Texture2D spring;
    Texture2D ring;
    Texture2D goal;

}Levelasset;

const int initial_map[MAP_ROW][MAP_COL]={
   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1},
    {1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,1,1,4,1,1,4,1,1,0,0,0,0,0,1,1},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,4,0,0,0,1,1,0,0,4,0,0,0,1,1,0,0,0,0,1,1,2,1,1,0,0,0,0,1,1,2,1,1,0,0,5,5},
    {1,1,0,0,0,0,0,0,0,4,0,0,0,0,1,1,0,2,0,0,0,0,0,0,0,0,0,1,1,0,0,2,0,0,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,5,5},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};
int map[MAP_ROW][MAP_COL];

/* scoring */ int score = 0;


// typedef struct Platform{
//     Rectangle rect;
//     platformtype type;

// }Platform;


// Platform level[PLATFORMS] = {
//     {{   0, 650, 400, 70 }, PT_NORMAL },
//     {{ 500, 600, 200, 30 }, PT_SPRING },
//     {{ 800, 420, 150, 30 }, PT_NORMAL },
//     {{ 950, 350, 150, 30 }, PT_SPIKE  },
//     {{980,  320, 150, 30 }, PT_GOAL   },
// };
void Reset(Ball *b){

    b->position=(v2){2.5f*TILE_SIZE,1.5f*TILE_SIZE};
    b->velocity=(v2){0.0f,0.0f};
    b->rotation = 0.0f;
    for(int r=0; r < MAP_ROW; r++){
        for(int c=0; c < MAP_COL; c++){
            map[r][c]=initial_map[r][c];
        }
    }
    score = 0;
}


void BallPlatformCollision(Ball *b, Rectangle tilerect, int tiletype, Gamestate *state){

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

    if(tiletype == 2) {*state = DEAD; return;}
    if(tiletype == 5) {*state = WIN; return;}

    float Normalvel = b->velocity.x*normal.x + b->velocity.y*normal.y;

    if(Normalvel < 0){
        if(tiletype==3 && normal.y < -0.5f){
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

    for(int r=0; r < MAP_ROW; r++){
        for(int c=0; c < MAP_COL; c++){
            int tile= map[r][c];
            if(tile == 0) continue;

            
            Rectangle tilerect  = {c*TILE_SIZE,r*TILE_SIZE,TILE_SIZE,TILE_SIZE};
            if(tile==4){
                if(CheckCollisionCircleRec(b->position,b->radius,tilerect)){
                    map[r][c] = 0;
                    score+=500;
                }
                continue;
            }
            if(tile == 2) {
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

    

    // if(b->position.x-b->radius < 0){
    //     b->position.x = b->radius;
    //     b->velocity.x = -b->velocity.x*RESTITUTION;
    // }
    if (b->position.x - b->radius < TILE_SIZE) {
        b->position.x = TILE_SIZE + b->radius;
        b->velocity.x = 0;
    }
    if (b->position.x + b->radius > MAP_WIDTH_PX - TILE_SIZE) {
        b->position.x = MAP_WIDTH_PX - TILE_SIZE - b->radius;
        b->velocity.x = 0;
    }

    if(b->position.y > MAP_HEIGHT_PX+50.0f){
        *state = DEAD;
    }
    ///Extra
    // if(b->position.y > BASE_H){
    //     b->position.y =-10;
    //     b->position.x = 5;
    // }

}

void DrawTile(Levelasset *lvl){
    for(int r=0; r < MAP_ROW; r++){
        for(int c=0; c < MAP_COL; c++){
            int tile = map[r][c];
            v2 pos = {c*TILE_SIZE,r*TILE_SIZE}; 
            Rectangle des = {c*TILE_SIZE,r*TILE_SIZE,TILE_SIZE,TILE_SIZE};
            v2 origin = {0.0f,0.0f};
            Texture2D *tex=NULL;

            switch(tile){
                case 1:tex=&lvl->brick;break;
                case 2:tex=&lvl->spike;break;
                case 3:tex=&lvl->spring;break;
                case 4:tex=&lvl->ring;break;
                case 5:tex=&lvl->goal;break;
                default:break;

            }
            if(tex != NULL){
                Rectangle src={0.0f,0.0f,(float)tex->width,(float)tex->height};
                DrawTexturePro(*tex,src,des,origin,0.0,WHITE);
            }
        }
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


    


int main(void){
// INIT
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(BASE_W,BASE_H,"Bounce Classic");
    SetTargetFPS(60);
    Texture2D ballsprite = LoadTexture("assets/images/red-ball.png");


    Texture2D retrybuttonsprite = LoadTexture("assets/images/retry-button.png");
    Texture2D resumebuttonsprite = LoadTexture("assets/images/resume.png");
    Texture2D homebuttonsprite = LoadTexture("assets/images/home.png");
    Texture2D logo = LoadTexture("assets/images/title-logo.png");
    Texture2D menueplaybutton = LoadTexture("assets/images/play-button.png");
    Texture2D pausebutton = LoadTexture("assets/images/PauseButton.png");

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
    camera.zoom = 1.8f;
    
    



    while(!WindowShouldClose()){
//UPDATE
        float dt = GetFrameTime();
        if(state ==PLAYING){
            UpdateBall(&ball,dt,&state); 
        }else if(IsKeyPressed(KEY_R)){
            Reset(&ball);
            state = PLAYING;

        }
        camera.target = ball.position;
        float min_camera_x = BASE_W / (2.0f * camera.zoom);
        float max_camera_x = MAP_WIDTH_PX - BASE_W / (2.0f * camera.zoom);

        
        if (max_camera_x < min_camera_x) {
            camera.target.x = MAP_WIDTH_PX / 2.0f;
        } else {
            camera.target.x = Clamp(ball.position.x, min_camera_x, max_camera_x);
        }

        
        camera.target.y = ball.position.y;
        float map_center_y = MAP_HEIGHT_PX / 2.0f;
        camera.target.y = map_center_y;
    
//Button Update
        v2 mouse = GetMousePosition();

        if(CheckCollisionPointRec(mouse, PauseButton.rect))
        {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PauseButton.state = PRESSED;
                state = PAUSED;
            }
            else PauseButton.state = NORMAL;
        }
        else PauseButton.state = NORMAL;


        if(state == PAUSED)
        {
            if(CheckCollisionPointRec(mouse, ResumeButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    ResumeButton.state = PRESSED;
                    state = PLAYING;
                    ResumeButton.state = NORMAL;
                }
                else ResumeButton.state = NORMAL;
            }
            else ResumeButton.state = NORMAL;

            if(CheckCollisionPointRec(mouse, RetryButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    RetryButton.state = PRESSED;
                    Reset(&ball);
                    state = PLAYING;
                }
                else RetryButton.state = NORMAL;
            }
            else RetryButton.state = NORMAL;

            if(CheckCollisionPointRec(mouse, HomeButton.rect))
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    state = MAIN_MENUE;
                    HomeButton.state = PRESSED;
                }
                else HomeButton.state = NORMAL;
            }
            else HomeButton.state = NORMAL;
        }

        if(state == MAIN_MENUE)
        {
            if(CheckCollisionPointRec(mouse, MenuePlayButton.rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                state = PLAYING;
                Reset(&ball);
            }
        }

        
        
         

//DRAW

        BeginDrawing();
        ClearBackground((Color){174, 206, 240, 255});
        if(state != MAIN_MENUE)
        {
            BeginMode2D(camera);
                DrawTile(&levelAssets);
                Rectangle des = {ball.position.x,ball.position.y,ball.radius*2.0f,ball.radius*2.0f};
                v2 origin = {ball.radius,ball.radius};
                DrawTexturePro(ball.texture,src,des,origin,ball.rotation,WHITE);
            EndMode2D();

            DrawText(TextFormat("Score: %d",score), BASE_W - 300,BASE_H - 680, 30, WHITE);
        }

        if(state != MAIN_MENUE) DrawTexturePro(pausebutton, (Rectangle){0.0f, 0.0f, pausebutton.width, pausebutton.height}, PauseButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
        
        if(state == PAUSED)
        {
            DrawRectangleRec(MenueRect, RAYWHITE);
            DrawText("LEVEL 1", MenueRect.x + 150, MenueRect.y + 20, 60, BLACK);
            DrawTexturePro(resumebuttonsprite, (Rectangle){0.0f, 0.0f, resumebuttonsprite.width, resumebuttonsprite.height}, ResumeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(retrybuttonsprite, (Rectangle){0.0f, 0.0f, retrybuttonsprite.width, retrybuttonsprite.height}, RetryButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(homebuttonsprite, (Rectangle){0.0f, 0.0f, homebuttonsprite.width, homebuttonsprite.height}, HomeButton.rect, (v2){0.0f, 0.0f}, 0.0f, WHITE);

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

            DrawTexturePro(menueplaybutton, 
            (Rectangle){0.0f, 0.0f, menueplaybutton.width, menueplaybutton.height},
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
    UnloadTexture(menueplaybutton);
    UnloadTexture(pausebutton);
    CloseWindow();


    return 0;

}
