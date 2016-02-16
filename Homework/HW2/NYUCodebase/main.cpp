#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Entity {
public:
    float x;
    float y;
    float angle;
    int textureID;
    float width;
    float height;
    float speed;
    float direction_x = 0;
    float direction_y = 0;
    
    Entity(float x, float y, float width, float height, float speed = 0):x(x),y(y),width(width),height(height) ,speed(speed) {};
    
    void DrawSprite(ShaderProgram& program, GLuint& texture, float textureCoord[]) {
        
        float vertices[] = {x-width, y-height,x+width, y-height,x+width, y+height,
                            x-width, y-height,x+width, y+height,x-width, y+height};
        Matrix modelMatrix;
        //modelMatrix.Translate(0.0, 0.0, 0.0);
        //modelMatrix.Rotate();
        
        program.setModelMatrix(modelMatrix);
        glBindTexture(GL_TEXTURE_2D, texture);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    bool isCollsion(Entity other){
        if(y-height > other.y+other.height || y+height < other.y-other.height ||
           x-width > other.x+other.width || x+width < other.x-other.width){
            return 0;
        }else{
            return 1;
        }
    }
};

SDL_Window* displayWindow;
void Setup(){
}

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

int main(int argc, char *argv[])
{
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
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    SDL_Event event;
    
    GLuint BarTexture = LoadTexture("bar.png");
    GLuint PLeftWinsTexture = LoadTexture("LeftPlayerWin.png");
    GLuint PRightWinsTexture = LoadTexture("RightPlayerWin.png");

    float lastFrameTicks = 0.0f;
    
    Entity paddle1(-5.4f,0.0f,.1f,.7f,10);
    Entity paddle2(5.4f,0.0f,.1f,.7f,10);
    Entity ball(0.0f,0.0f,.1f,.1f,1);
    Entity botmBoarder(0.0f,-2.9f,5.55f,.1f);
    Entity topBoarder(0.0f,2.9f,5.55f,.1f);
    Entity PLeftWins(0.0f,0.0f,2.0f,1.0f);
    ball.angle = 0;
    
    bool done = false;
    while (!done) {
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            } else if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode == SDL_SCANCODE_W) {
                    paddle1.y += elapsed * paddle1.speed;
                }else if(event.key.keysym.scancode == SDL_SCANCODE_S){
                    paddle1.y += elapsed * -paddle1.speed;
                }
                if(event.key.keysym.scancode == SDL_SCANCODE_UP){
                    paddle2.y += elapsed * paddle2.speed;
                }else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN){
                    paddle2.y += elapsed * -paddle2.speed;
                }
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);

        float textureCoords00[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        ball.DrawSprite( program, BarTexture, textureCoords00);
        
        botmBoarder.DrawSprite( program, BarTexture, textureCoords00);
        
        topBoarder.DrawSprite( program, BarTexture, textureCoords00);
        
        if(paddle1.isCollsion(topBoarder)){
            paddle1.y += elapsed * -paddle1.speed;
        }else if(paddle1.isCollsion(botmBoarder)){
            paddle1.y += elapsed * paddle1.speed;
        }
        paddle1.DrawSprite( program, BarTexture, textureCoords00);
        
        
        if(paddle2.isCollsion(topBoarder)){
            paddle2.y += elapsed * -paddle2.speed;
        }else if(paddle2.isCollsion(botmBoarder)){
            paddle2.y += elapsed * paddle2.speed;
        }
        paddle2.DrawSprite( program, BarTexture, textureCoords00);
        
        
        if(ball.isCollsion(topBoarder)){
            ball.angle = 180-ball.angle;
        }else if(ball.isCollsion(botmBoarder)){
            ball.angle = 180-ball.angle;
        }else if(ball.isCollsion(paddle1)){
            ball.angle = 180-ball.angle;
        }else if(ball.isCollsion(paddle2)){
            ball.angle = 180-ball.angle;
        }
        ball.x += cos(ball.angle*3.14/180) * elapsed * ball.speed;
        ball.y += sin(ball.angle*3.14/180) * elapsed * ball.speed;
        ball.DrawSprite( program, BarTexture, textureCoords00);
        
        if(!(ball.x-ball.width > -5.55)){
            PLeftWins.DrawSprite( program, PRightWinsTexture, textureCoords00);
        }else if(!(ball.x+ball.width < 5.55)){
            PLeftWins.DrawSprite( program, PLeftWinsTexture, textureCoords00);
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
