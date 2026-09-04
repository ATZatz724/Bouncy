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
    WIN
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
                }
                continue;
            }
            if(tile == 2) {
            Rectangle spikehit = {
                tilerect.x + (TILE_SIZE * 0.25f), 
                tilerect.y + (TILE_SIZE * 0.25f), 
                TILE_SIZE * 0.5f,                 
                TILE_SIZE * 0.5f                  
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

int main(void){
// INIT
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(BASE_W,BASE_H,"Bounce Classic");
    SetTargetFPS(60);
    Texture2D ballsprite = LoadTexture("assets/images/red-ball.png");

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
    Gamestate state = PLAYING;
    Reset(&ball);
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
        
        
         

//DRAW
        BeginDrawing();
        ClearBackground((Color){174, 206, 240, 255});
        
        BeginMode2D(camera);
            DrawTile(&levelAssets);
            Rectangle des = {ball.position.x,ball.position.y,ball.radius*2.0f,ball.radius*2.0f};
            v2 origin = {ball.radius,ball.radius};
            DrawTexturePro(ball.texture,src,des,origin,ball.rotation,WHITE);
        EndMode2D();


        
        

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

    CloseWindow();


    return 0;

}