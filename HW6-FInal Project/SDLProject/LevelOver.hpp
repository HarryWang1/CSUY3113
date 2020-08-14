//
//  LevelOver.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef LevelOver_hpp
#define LevelOver_hpp

#include "Scene.hpp"

class LevelOver : public Scene
{
public:
    void Initialize() override;
    void Update (float deltaTime) override;
    void Render (ShaderProgram *program) override;
    void Reset () override;
};

#endif /* LevelOver_hpp */
