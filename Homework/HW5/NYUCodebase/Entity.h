//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//
#pragma once

#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include "ShaderProgram.h"
#include <math.h>
#include "SheetSprite.h"

#endif /* Entity_h*/

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_COIN};

class Entity {
public:
    void UpdateX(float FIXED_TIMESTEP);
    void UpdateY(float FIXED_TIMESTEP);
    void Render(ShaderProgram *program, GLuint& texture, float textureCoord[]);
    bool collidesWith(Entity *entity);
    
    float x;
    float y;
    
    float width;
    float height;
    
    float velocity_x = 0;
    float velocity_y = 0;
    
    float acceleration_x = 0;
    float acceleration_y = 0;
    
    float friction_x=0;
    float friction_y=0;
    
    float gravity_x=0;
    float gravity_y=0;
    
    bool isStatic;
    EntityType entityType;
    SheetSprite sprite;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
};