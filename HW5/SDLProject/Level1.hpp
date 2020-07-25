//
//  Level1.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level1_hpp
#define Level1_hpp

#include "Scene.hpp"

class Level1 : public Scene
{
public:
    void Initialize() override;
    void Update (float deltaTime) override;
    void Render (ShaderProgram *program) override;
    void Reset () override;
};

#endif /* Level1_hpp */
