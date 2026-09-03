#include "raylib.h"
#include "raymath.h"

#define BASE_W 1280
#define BASE_H 720
#define WORLD_WIDTH 4*BASE_H
#define WORLD_HEIGHT BASE_H

#define SQUARE_SIDE_LENGHT 0.07*BASE_W

#define SPIKE_WIDTH 0.02*BASE_W
#define SPIKE_HEIGHT 0.14*BASE_H

#define GRAVITY 1500.0f
#define FLOOR_Y 650.0f
#define RESTITUTION 0.6f
#define PLATFORMS 50
#define MAX_ACC 900.0f
#define MAX_SPEED 800.0f
#define FRICTION 0.9f
#define JUMP -1000.0f

typedef Vector2 v2;

typedef struct Ball
{
    v2 position;
    v2 velocity;
    float radius;
    bool grounded;
    float rotation;
} Ball;

typedef enum Gamestate{
    PLAYING,
    DEAD,
    WIN
}Gamestate;

typedef enum platformtype{
    PT_NORMAL,
    PT_SPRING,
    PT_SPIKE,
    PT_GOAL
}platformtype;


typedef struct Walls{
    Rectangle rect;
    platformtype type;

}Walls;

Walls walls[] = 
{
    {{0, 0, 0.13*BASE_W, BASE_H},PT_NORMAL},
    {{0, BASE_H*(1 - 0.1), WORLD_WIDTH, 0.1*BASE_H},PT_NORMAL},//floor
    {{0, 0, WORLD_WIDTH, 0.1*BASE_H},PT_NORMAL},//ceiling
    {{0.33*BASE_W, 0.1*BASE_H, 6*SQUARE_SIDE_LENGHT, 4*SQUARE_SIDE_LENGHT},PT_NORMAL},//ceiling wall
    {{0.92*BASE_W, 0.41*BASE_H, 2*SQUARE_SIDE_LENGHT, 4*SQUARE_SIDE_LENGHT},PT_NORMAL},
    {{ 100, BASE_H - 100, SPIKE_WIDTH, SPIKE_HEIGHT}, PT_SPIKE}

    
};

void CheckCollision(Ball *b, Walls wall,Gamestate *state)
{
    bool Colliding = CheckCollisionCircleRec(b->position, b->radius, wall.rect);

    if(Colliding && wall.type == PT_SPIKE) *state = DEAD;
    if(Colliding && wall.type == PT_GOAL)
    {
        //next level update
    }

    if(Colliding)
    {
        float ClosestX = Clamp(b->position.x,wall.rect.x,wall.rect.x+wall.rect.width);
        float ClosestY = Clamp(b->position.y,wall.rect.y,wall.rect.y+wall.rect.height);

        float dx = b->position.x - ClosestX;
        float dy = b->position.y - ClosestY;

        float distance = sqrtf(dx*dx + dy*dy);
        v2 normal = (distance > 0.00001f) ? (v2){dx/distance, dy/distance} : (v2){0, -1};
        float penetration = b->radius - distance;
        b->position = Vector2Add(b->position, Vector2Scale(normal, penetration));


        float Normalvel = b->velocity.x*normal.x + b->velocity.y*normal.y;

        if(wall.type == PT_SPRING && normal.y < -0.5f)
        {
            b->velocity.y = -900.0f;
            b->grounded = true;
        }
        else
        {
            b->velocity.x -= (1+RESTITUTION)*Normalvel*normal.x;
            b->velocity.y -= (1+RESTITUTION)*Normalvel*normal.y;
        }

        if(normal.y < -0.5)
        {
            b->grounded = true;
        }

    }



}






void updateBALL(Ball *b, float dt,Gamestate *state)
{
    b->grounded = false;
    if(IsKeyDown(KEY_RIGHT)){
        b->velocity.x += MAX_SPEED;
    }
    if(IsKeyDown(KEY_LEFT)){
        b->velocity.x -= MAX_SPEED;
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

    b->rotation += (b->velocity.x / (float)b->radius);


    if(b->rotation > 360) b->rotation -= 360;
    if(b->rotation < 0) b->rotation += 360;


    //collision
    for(int i = 0;i<sizeof(walls)/sizeof(Walls);i++)
    {
        CheckCollision(b, walls[i], state);
    }




    if(IsKeyPressed(KEY_UP) && b->grounded){
        b->velocity.y = JUMP;
        b->grounded = false;
    }

    // if(b->position.x-b->radius < 0){
    //     b->position.x = b->radius;
    //     b->velocity.x = -b->velocity.x*RESTITUTION;
    // }
    // if(b->position.x+b->radius > BASE_W){
    //     b->position.x = BASE_W-b->radius;
    //     b->velocity.x = -b->velocity.x*RESTITUTION;
    // }



}





int main()
{
    //radius = 0.06*BASE_W

    
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(BASE_W,BASE_H,"Bounce Classic");
    SetTargetFPS(60);

    Ball normal_ball;

    normal_ball.radius = 0.04*BASE_W;
    normal_ball.velocity = (v2){0 , 0};
    normal_ball.position = (v2){0.13*BASE_W + normal_ball.radius, 0.1*BASE_H + normal_ball.radius};
    normal_ball.grounded = false;
    normal_ball.rotation = 0.0f;


    Gamestate state = PLAYING;
    


    Camera2D camera;
    camera.target = normal_ball.position;
    camera.offset = (v2){BASE_W / 2.0f, BASE_H / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
 
    Texture2D NormalBallSprite = LoadTexture("assets/images/red-ball.png");

    Rectangle src = {0.0f, 0.0f, (float)NormalBallSprite.width, (float)NormalBallSprite.height};

    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();

        camera.target.x = normal_ball.position.x;
        camera.target.y = BASE_H / 2.0f;
        
        if(camera.target.x < BASE_W / 2.0f) camera.target.x = BASE_W / 2.0f;


        if(state == PLAYING)
            updateBALL(&normal_ball, dt,&state);



        BeginDrawing();
            ClearBackground(WHITE);

            BeginMode2D(camera);

                // DrawCircleV(normal_ball.position, normal_ball.radius, RED);


                Rectangle des = {normal_ball.position.x, normal_ball.position.y, normal_ball.radius*2.0f, normal_ball.radius*2.0f};

                DrawTexturePro(NormalBallSprite, src, des, (v2){normal_ball.radius, normal_ball.radius},normal_ball.rotation, WHITE);



                for(int i = 0;i < sizeof(walls)/sizeof(Walls);i++)
                    DrawRectangleRec(walls[i].rect, ORANGE);

            EndMode2D();

        EndDrawing();
    }
    UnloadTexture(NormalBallSprite);
    CloseWindow();

}

