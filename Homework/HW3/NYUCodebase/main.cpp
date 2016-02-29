#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
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
GLuint humanShipTexture=0;
GLuint ufoTexture=0;
GLuint barTexture=0;
GLuint spriteSheet=0;

Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

class Entity {
public:
    float x;
    float y;
    float angle;
    int textureID;
    float width;
    float height;
    float direction_x = 0;
    float direction_y = 0;
    float speed_x = 0;
    float speed_y = 0;
    
    Entity() {};
    Entity(float x, float y, float width, float height, float speed_x = 0, float speed_y = 0, float angle = 0):x(x), y(y), width(width), height(height), speed_x(speed_x), speed_y(speed_y), angle(angle) {};
    
    void DrawSprite(ShaderProgram& program, GLuint& texture, float textureCoord[]) {
        
        float vertices[] = {x-width, y-height,x+width, y-height,x+width, y+height,
            x-width, y-height,x+width, y+height,x-width, y+height};
        Matrix modelMatrix;
        
        program.setModelMatrix(modelMatrix);
        glBindTexture(GL_TEXTURE_2D, texture);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    bool isCollision(Entity other){
        if(y-height > other.y+other.height || y+height < other.y-other.height ||
           x-width > other.x+other.width || x+width < other.x-other.width){
            return 0;
        }else{
            return 1;
        }
    }
};

vector<Entity> entities;
vector<Entity> bullets;
Entity humanShip;
Entity leftBorder;
Entity rightBorder;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}

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

void Update(){
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

void Render(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoords00[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    humanShip.DrawSprite( program, humanShipTexture, textureCoords00);
    rightBorder.DrawSprite( program, barTexture, textureCoords00);
    leftBorder.DrawSprite( program, barTexture, textureCoords00);
    for(int i=0;i<entities.size();i++){
        entities[i].DrawSprite( program, ufoTexture, textureCoords00);
    }
    for(int i=0;i<bullets.size();i++){
        bullets[i].DrawSprite( program, ufoTexture, textureCoords00);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


int main(int argc, char *argv[])
{
    Setup();
    humanShipTexture = LoadTexture("humanShip.png");
    ufoTexture = LoadTexture("ufo.png");
    barTexture = LoadTexture("bar.png");
    spriteSheet = LoadTexture("sheet.png");
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    while (!done) {
        ProcessEvents();
        Update();
        Render(program);
    }
    
    SDL_Quit();
    return 0;
}