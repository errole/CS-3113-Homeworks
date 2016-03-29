//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#include "Entity.h"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

void Entity::Render(ShaderProgram *program, GLuint& texture, float textureCoord[]) {
    
    
    
    float vertices[] = {x-width, y-height,x+width, y-height,x+width, y+height,
        x-width, y-height,x+width, y+height,x-width, y+height};
    Matrix modelMatrix;
    
    program->setModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoord);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
bool Entity::collidesWith(Entity *other){
    if(y-height > other->y+other->height || y+height < other->y-other->height ||
        x-width > other->x+other->width || x+width < other->x-other->width){
        return 0;
    }else{
        return 1;
    }
}

void Entity::UpdateX(float FIXED_TIMESTEP){
    velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
    velocity_x += acceleration_x * FIXED_TIMESTEP;
    x += velocity_x * FIXED_TIMESTEP;
}

void Entity::UpdateY(float FIXED_TIMESTEP){
    velocity_y = gravity_y * FIXED_TIMESTEP;
    velocity_y += lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
    velocity_y += acceleration_y * FIXED_TIMESTEP;
    y += velocity_y * FIXED_TIMESTEP;
}