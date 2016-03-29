//
//  Map.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/29/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#pragma once

#ifndef Map_h
#define Map_h

#include <stdio.h>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
using namespace std;
#endif /* Map_h */

class Map{
public:
    //Map Data
    unsigned char** levelData;
    float TILE_SIZE = 70;
    int mapWidth;
    int mapHeight;
    int SPRITE_COUNT_X = 14;
    int SPRITE_COUNT_Y = 7;

    bool readHeader(std::ifstream &stream);
    bool readLayerData(std::ifstream &stream);
    bool readEntityData(std::ifstream &stream, Entity &player);
    void renderLevel(ShaderProgram &program, GLuint mapTexture, Matrix &modelMatrix);
    void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
private:
    void placeEntity(string type,float placeX,float &placeY, Entity &player);
};