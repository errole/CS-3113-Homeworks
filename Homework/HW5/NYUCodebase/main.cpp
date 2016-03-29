#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utilities.h"
#include "Map.h"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

//Global Setup Parameters
SDL_Window* displayWindow;
SDL_Event event;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
bool done = false;
float lastFrameTicks = 0.0f;
float elapsed = 0.0;
GLuint mapTexture=0;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_WIN, STATE_LOSE};
int state = STATE_MAIN_MENU;

//Matrices
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

//Entity Data
Map levelData;
vector<Entity> mapEntities;
vector<Entity> enemies;
Entity player(100,-350,10,10);


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
    string levelFile = "/Users/errolelbasan/Documents/Codes/CS3113/CS3113-Homework/Homework/HW5/PlatformerMap.txt";
    ifstream infile(levelFile);
    string line;
    
    while (getline(infile, line)) {
        if(line == "[header]") {
            levelData.readHeader(infile);
        } else if(line == "[layer]") {
            levelData.readLayerData(infile);
        } else if(line == "[ObjectsLayer]") {
            levelData.readEntityData(infile, player);
        }
    }
    viewMatrix.Scale(.01,.01,0);
    viewMatrix.Translate(-300,200,0);
}

void ProcessEvents(ShaderProgram program){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }else if(event.type == SDL_KEYDOWN) {
        }
    }
}

void UpdateGameLevel(ShaderProgram program){
    player.collidedBottom = false;
    if(keys[SDL_SCANCODE_SPACE]){
        player.acceleration_y = 1.5;
        player.velocity_y = 2.2;
        player.velocity_y += player.lerp(player.velocity_y, 0.0f, FIXED_TIMESTEP * player.friction_y);
        player.velocity_y += player.acceleration_y * FIXED_TIMESTEP;
        player.y += player.velocity_y * FIXED_TIMESTEP;
        player.collidedBottom = false;
    }
    player.collidedRight = false;
    if(keys[SDL_SCANCODE_RIGHT]) {
        player.acceleration_x = 0.9;
        player.velocity_x = player.lerp(player.velocity_x, 0.0f, FIXED_TIMESTEP * player.friction_x);
        player.velocity_x += player.acceleration_x * FIXED_TIMESTEP;
        player.x += player.velocity_x * FIXED_TIMESTEP;
    }
    player.collidedLeft = false;
    if(keys[SDL_SCANCODE_LEFT]){
        player.acceleration_x = -0.9;
        player.velocity_x = player.lerp(player.velocity_x, 0.0f, FIXED_TIMESTEP * player.friction_x);
        player.velocity_x += player.acceleration_x * FIXED_TIMESTEP;
        player.x += player.velocity_x * FIXED_TIMESTEP;
    }
}

void RenderGameLevel(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    viewMatrix.identity();
    viewMatrix.Translate(-player.x, -player.y, 0);
    //program.setViewMatrix(viewMatrix);
    levelData.renderLevel(program, mapTexture, modelMatrix);
    player.Render();
    SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[])
{
    Setup();
    mapTexture = LoadTexture("mapTexture.png");
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    SheetSprite mySprite(&program, mapTexture,30,30,20, 10);
    player.sprite= &mySprite;
    
    while (!done) {
        ProcessEvents(program);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
        }
        
        UpdateGameLevel(program);
        player.Render();
        RenderGameLevel(program);
    }
    
    SDL_Quit();
    return 0;
}



