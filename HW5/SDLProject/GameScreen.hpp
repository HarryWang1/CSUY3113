//
//  GameScreen.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef GameScreen_hpp
#define GameScreen_hpp

#include "Scene.hpp"

class GameScreen : public Scene
{
public:
    void Initialize() override;
    void Update (float deltaTime) override;
    void Render (ShaderProgram *program) override;
};

#endif /* GameScreen_hpp */
