#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.hpp"
#include "Utilities.hpp"

using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

//Global Setup Parameters
SDL_Window* displayWindow;
SDL_Event event;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
bool done = false;
float lastFrameTicks = 0.0f;
float elapsed = 0.0;
GLuint barTexture=0;
GLuint spriteSheet=0;
GLuint fontTexture=0;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL };
int state = STATE_GAME_LEVEL;
//int state = STATE_MAIN_MENU;

Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

vector<Entity> entities;
vector<Entity> bullets;
Entity humanShip;
Entity leftBorder;
Entity rightBorder;



void Setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glClearColor(.0f,.0f,.3f,.0f);
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
    
    leftBorder = Entity(-5.4f,0.0f,.2f,3.0f);
    rightBorder = Entity(5.4f,0.0f,.2f,3.0f);
    humanShip = Entity(0.0f,-2.8f,.2f,.2f,5);
    for(int i=0;i<12;i++){
        for(int j=0;j<5;j++){
            Entity myEntity;
            myEntity = Entity(-5.0+i*.5,2.8-j*.5,.2,.2,1);
            entities.push_back(myEntity);
        }
    }
}

void ProcessEvents(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }else if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                Entity newBullet(humanShip.x,humanShip.y,.1,.1,0,5);
                bullets.push_back(newBullet);
            }
        }
    }
}

void UpdateMainMenu(){
    
}

void UpdateGameLevel(){
    //Update timer for smoother movements
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    //Get input for movement of ship
    if(keys[SDL_SCANCODE_RIGHT]) {
        humanShip.x += elapsed * humanShip.speed_x;
    }else if(keys[SDL_SCANCODE_LEFT]){
        humanShip.x += elapsed * -humanShip.speed_x;
    }
    
    //Check for humanShip collision
    if(rightBorder.isCollision(humanShip)){
        humanShip.x += elapsed * -humanShip.speed_x;
    }else if(leftBorder.isCollision(humanShip)){
        humanShip.x += elapsed * humanShip.speed_x;
    }
    
    //Bullet Movements
    for(int i=0;i<bullets.size();i++){
        bullets[i].y += elapsed * bullets[i].speed_y;
    }
    
    //UFO movements
    for(int i=0;i<entities.size();i++){
        if(rightBorder.isCollision(entities[i])){
            for(int i=0;i<entities.size();i++){
                entities[i].speed_x = -1;
                entities[i].y -= elapsed * 5;
            }
        }else if(leftBorder.isCollision(entities[i])){
            for(int i=0;i<entities.size();i++){
                entities[i].speed_x = 1;
                entities[i].y -= elapsed * 5;
            }
        }
        entities[i].x += elapsed * entities[i].speed_x;
    }
    
    //Cross Check UFOs and Bullets
    for(int i=0;i<entities.size();i++){
        for(int j=0;j<bullets.size();j++){
            if(bullets[j].isCollision(entities[i])){
                entities.erase(entities.begin()+i);
                bullets.erase(bullets.begin()+j);
            }
        }
    }
}

void RenderMainMenu(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    DrawText(&program, fontTexture, "SPACE INVADERS!", .5, -.1);
    
    SDL_GL_SwapWindow(displayWindow);
}

void RenderGameLevel(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoords00[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
 
    float u=434.0f/1024.0f;
    float v=234.0f/1024.0f;
    float width= 91.0f/1024.0f;
    float height= 91.0f/1024.0f;
    float ufoGreenUV[] = { u+width, v, u, v, u, v+height, u+width, v, u, v+height, u+width, v+height};
    
    u=346.0f/1024.0f;
    v=75.0f/1024.0f;
    width= 98.0f/1024.0f;
    height= 75.0f/1024.0f;
    float playerShip3Red[] = {u+width, v, u, v, u, v+height, u+width, v, u, v+height, u+width, v+height};
    
    humanShip.DrawSprite( program, spriteSheet, playerShip3Red);
    rightBorder.DrawSprite( program, barTexture, textureCoords00);
    leftBorder.DrawSprite( program, barTexture, textureCoords00);
    for(int i=0;i<entities.size();i++){
        entities[i].DrawSprite( program, spriteSheet, ufoGreenUV);
    }
    for(int i=0;i<bullets.size();i++){
        bullets[i].DrawSprite( program, spriteSheet, ufoGreenUV);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


int main(int argc, char *argv[])
{
    Setup();
    barTexture = LoadTexture("bar.png");
    spriteSheet = LoadTexture("sheet.png");
    fontTexture = LoadTexture("font1.png");
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    while (!done) {
        ProcessEvents();
        
        switch(state) {
            case STATE_MAIN_MENU:
                UpdateMainMenu();
                break;
            case STATE_GAME_LEVEL:
                UpdateGameLevel();
                break;
        }
        switch(state) {
            case STATE_MAIN_MENU:
                RenderMainMenu(program );
                break;
            case STATE_GAME_LEVEL:
                RenderGameLevel(program);
                break;
        }
    }
    SDL_Quit();
    return 0;
}