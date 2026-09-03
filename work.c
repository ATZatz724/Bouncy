#include "raylib.h"
#include "raymath.h"

#define BASE_W 1280
#define BASE_H 720
#define BULLET_MAX 200
#define ASTEROID_MAX 25
#define STAR_MAX 30


int score = 0;
Sound fxShoot;
Sound fxLaser;
Music bgMusic;


typedef Vector2 v2;

typedef enum {
    BULLETS,
    LASER
}weapontype;

typedef struct Player{
    v2 position ;
    v2 velocity;
    float rotation;
    float radius;
    weapontype curweapon;
}Player;

typedef struct Bullet{
    v2 position;
    v2 velocity;
    weapontype type;
    float rotation;
    bool active;

}Bullet;


typedef struct Asteroid{
    v2 position;
    v2 velocity;
    float radius;
    float rotation;
    float rotspeed;
    int index;
    bool active;
}Asteroid;

typedef struct Star{
    v2 position;
    float alpha;
    float speed;

}Star;

Bullet bullets[BULLET_MAX]={0};
Asteroid ast[ASTEROID_MAX]={0};
Star star[STAR_MAX];

void initStar(){
    for(int i=0; i < STAR_MAX;i++){
        star[i].position= (v2){GetRandomValue(0,BASE_W),GetRandomValue(0,BASE_H)};
        star[i].alpha = (float)GetRandomValue(50,255)/255.0f;
        star[i].speed = (float)GetRandomValue(1,3)*0.5f;
    }
}

void starUpdate(float dt,v2 vel){
    for(int i=0; i< STAR_MAX; i++){

        float id = (i%3==0)?0.08f:0.03f;
        star[i].position.x -= vel.x*id*dt;
        star[i].position.y -= vel.y*id*dt;

        star[i].position.x = Wrap(star[i].position.x, 0.0f, BASE_W);
        star[i].position.y = Wrap(star[i].position.y, 0.0f, BASE_H);
        


        star[i].alpha += 0.1f*star[i].speed*dt;
        if(star[i].alpha > 1.0f || star[i].alpha < 0.2f){
            star[i].speed *=-1.0f;
        }
        Color starC = ColorAlpha(WHITE,star[i].alpha);
        DrawCircleV(star[i].position,(i % 3==0)?2.0f:1.0f,starC);

    }

}


void UpdatePlayer(Player *p, float dt){
    const float turnspeed = 220.0f;
    const float acc = 400.0f;
    const float maxspeed = 500.0f;
    const float drag = 0.99f;
    


    if(IsKeyDown(KEY_LEFT)) p->rotation -= turnspeed*dt;
    if(IsKeyDown(KEY_RIGHT)) p->rotation += turnspeed*dt;



    if(IsKeyDown(KEY_UP)){
        v2 forward= {cosf(p->rotation*DEG2RAD),sinf(p->rotation*DEG2RAD)};
        p->velocity.x += forward.x * acc * dt;
        p->velocity.y += forward.y * acc * dt;
    }  

    float speed = Vector2Length(p->velocity);
    if(speed > maxspeed){
        p->velocity = Vector2Scale(Vector2Normalize(p->velocity),maxspeed);
    }

    p->velocity =Vector2Scale(p->velocity,drag);

    p->position.x += p->velocity.x*dt;
    p->position.y += p->velocity.y*dt;

    p->position.x =  Wrap(p->position.x,0.0f-30.0,BASE_W+30.0);
    p->position.y = Wrap(p->position.y, 0.0f-30.0, BASE_H+30.0);
    


}

void Bulletshot(v2 pos,v2 vel,Texture2D sprite,float rotation,float scale, weapontype type){

    v2 forward = {cosf(rotation*DEG2RAD),sinf(rotation*DEG2RAD)};

    float distance = ((float)(sprite.width)*scale)/2.0;

    v2 bulletpos = {pos.x+forward.x*distance,pos.y+forward.y*distance};


    for(int i=0; i < BULLET_MAX ; i++){
        if(!bullets[i].active){
            bullets[i].position=bulletpos;
            bullets[i].velocity= (v2) {vel.x+forward.x*600.0f, vel.y+forward.y*600.0f};
            bullets[i].type =type;
            bullets[i].rotation = rotation;
            bullets[i].active = 1;
            PlaySound(fxLaser);
            return;
        }
        
    }


}

void Updatebullets(float dt){
    


    for(int i=0; i < BULLET_MAX; i++){
        
        
        if(bullets[i].active){
            bullets[i].position.x += bullets[i].velocity.x*dt;
            bullets[i].position.y  += bullets[i].velocity.y*dt;

            if(bullets[i].position.x > BASE_W || bullets[i].position.x  < 0.0 || bullets[i].position.y  > BASE_H || bullets[i].position.y  < 0.0){
                bullets[i].active = 0;
            }
        }
    }

} 

void Asteroidspawn(v2 pos){
    for(int i=0; i < ASTEROID_MAX; i++){
        if(!ast[i].active){
            int side = GetRandomValue(0,3);
            v2 ast_pos ={0,0};
            if(side==0) ast_pos = (v2){GetRandomValue(0,BASE_W),-30.0f};
            else if(side == 1) ast_pos = (v2){GetRandomValue(0,BASE_W),BASE_H+30.0f};
            else if(side == 2) ast_pos = (v2){-30.0f,GetRandomValue(0,BASE_H)};
            else  ast_pos = (v2){BASE_W+30.0f,GetRandomValue(0,BASE_H)};

            v2 target = {pos.x+GetRandomValue(-100,100),pos.y+GetRandomValue(-100,100)};
            v2 dir = Vector2Normalize(Vector2Subtract(target,ast_pos));
            float speed = GetRandomValue(100,220);

            ast[i].position = ast_pos;
            ast[i].velocity= Vector2Scale(dir,speed);
            ast[i].radius=GetRandomValue(20,50);
            ast[i].rotation = GetRandomValue(0,360);
            ast[i].rotspeed = GetRandomValue(-120,120);
            ast[i].index = GetRandomValue(0,2);
            ast[i].active = 1;
            return;

        }

        
    }

}


void Updateasteroid(float dt){
    for(int i=0; i < ASTEROID_MAX;i++){
        if(ast[i].active){
            ast[i].position.x += ast[i].velocity.x*dt;
            ast[i].position.y += ast[i].velocity.y*dt;

            ast[i].rotation += ast[i].rotspeed*dt;
            ast[i].position.x =  Wrap(ast[i].position.x,-30.0f,BASE_W+30.0);
            ast[i].position.y =  Wrap(ast[i].position.y,-30.0f,BASE_H+30.0);
        }
        
    }
}

void CheckCollision(Player *p){
    for(int i=0; i < ASTEROID_MAX;i++){
        if(!ast[i].active) continue;
        for(int j=0; j < BULLET_MAX; j++){
            if(!bullets[j].active) continue;

            if(CheckCollisionCircles(bullets[j].position,3.0f,ast[i].position,ast[i].radius)){
                bullets[j].active=0;
                ast[i].active=0;
                score += GetRandomValue(50,100);
                PlaySound(fxShoot);
                break;
            }

        

        }
        if(ast[i].active && CheckCollisionCircles(p->position,p->radius,ast[i].position,ast[i].radius)){
            p->position = (v2){BASE_W/2.0,BASE_H/2.0};
            p->velocity = (v2){0,0};
            ast[i].active=0;
            score = 0;


        }
    }

}

void drawPlayer(Player *p, Texture2D sprite, float scale){

    Rectangle src = {0.0f,0.0f,(float)sprite.width,(float)sprite.height};

    
    Rectangle des = {p->position.x,p->position.y,
                    (float)sprite.width*scale,(float)sprite.height*scale};
        v2 origin = {des.width/2.0,des.height/2.0};
        DrawTexturePro(sprite,src,des,origin,p->rotation,WHITE);

}

void drawBullets(Texture2D laser, float laserscale){
    Rectangle src ={0.0f,0.0f,(float)laser.width,(float)laser.height};
    for(int i=0; i < BULLET_MAX; i++){
        if(bullets[i].active){
            if(bullets[i].type==BULLETS){
                DrawCircleV(bullets[i].position, 4, MAGENTA);
            }
            else if(bullets[i].type==LASER){

                Rectangle des = {bullets[i].position.x,bullets[i].position.y,
                    (float)laser.width*laserscale,(float)laser.height*laserscale};
                v2 origin = {des.width/2.0,des.height/2.0};
                DrawTexturePro(laser,src,des,origin,bullets[i].rotation,WHITE);

            }


            }
            
        }
    }

void drawAsteroid(Texture2D asteroid[]){
    
    for(int i=0; i < ASTEROID_MAX;i++){
        if(ast[i].active){
            
            Texture2D tex = asteroid[ast[i].index];
            Rectangle src = {0.0f,0.0f,(float)tex.width,(float)tex.height};
            Rectangle des ={ast[i].position.x,ast[i].position.y,
                            ast[i].radius*2.0,ast[i].radius*2.0};

            v2 origin = {ast[i].radius, ast[i].radius};
        

            DrawTexturePro(tex,src,des,origin,0.0f,WHITE);

            
        }
    }
}





int main(){

    //Init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(BASE_W, BASE_H,"Galatic Breach");
    InitAudioDevice();
    initStar();
    SetTargetFPS(60);

    float cooldown = 0.0f;
    float scale = 2.0f;
    float laserscale = 0.4f;
    float ast_timer=0.0f;


    Texture2D background = LoadTexture("assets/images/galaxy.png");
    Texture2D spaceship = LoadTexture("assets/images/spp.png");
    Texture2D laser = LoadTexture("assets/images/15.png");
    Texture2D asteroid[] = {LoadTexture("assets/images/ast.png"),
                            LoadTexture("assets/images/ast2.png"),
                            LoadTexture("assets/images/ast3.png")};


    //Audio
    fxShoot = LoadSound("assets/audio/shoot.wav");
    fxLaser = LoadSound("assets/audio/laser.wav");
    bgMusic = LoadMusicStream("assets/audio/ambience.mp3");
    PlayMusicStream(bgMusic);
    SetMusicVolume(bgMusic,0.6f);

    Player player ={.position={640,360},.velocity={0,0},.rotation=0.0f
                    ,.radius=(float)(spaceship.width*scale)/2.0,.curweapon=BULLETS};

    

    

    

    while(!WindowShouldClose()){
        //Update
        float dt=GetFrameTime();
        UpdateMusicStream(bgMusic);

        cooldown -= dt;
        if(IsKeyPressed(KEY_TAB)){
            if(player.curweapon == BULLETS){
                player.curweapon = LASER;
            }else{
                player.curweapon = BULLETS;
            }
        }
        if(IsKeyDown(KEY_SPACE) && cooldown <= 0.0f){
            Bulletshot(player.position,player.velocity,spaceship,player.rotation,scale,player.curweapon);
            cooldown = 0.15f;
        }
        // if(IsKeyPressed(KEY_ENTER) && cooldown <=0.0f){
        //     Bulletshot(player.position,player.velocity,spaceship,player.rotation,scale,LASER);
        //     cooldown = 0.15f;
        // }
        ast_timer += dt;
        if(ast_timer >= 1.5) {
            Asteroidspawn(player.position);
            ast_timer=0.0f;
        }
        
        UpdatePlayer(&player,dt);
        Updatebullets(dt);
        Updateasteroid(dt);
        CheckCollision(&player);


        //Draw

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(background,0,0,WHITE);
            starUpdate(dt,player.velocity);

            drawPlayer(&player,spaceship,scale);
            drawBullets(laser,laserscale);
            drawAsteroid(asteroid);
            DrawText(TextFormat("Score: %06d", score),20,20,24,GOLD);
            
            if(player.curweapon==BULLETS){
                DrawText("Bullets Activated",1050,15,20,GOLD);
            }else{
                DrawText("Laser Activated",1050,15,20,GOLD);
            }
            
            

        EndDrawing();

        


    }

    //Deinit

    UnloadTexture(background);
    UnloadTexture(spaceship);
    UnloadTexture(laser);
    for(int i=0; i < 3 ; i++){
        UnloadTexture(asteroid[i]);
    }
    UnloadSound(fxShoot);
    UnloadSound(fxLaser);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();
    
    CloseWindow();



    return 0;
}