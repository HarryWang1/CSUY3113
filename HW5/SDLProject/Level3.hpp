//
//  Level3.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level3_hpp
#define Level3_hpp

#include "Scene.hpp"

class Level3 : public Scene
{
public:
    void Initialize() override;
    void Update (float deltaTime) override;
    void Render (ShaderProgram *program) override;
    void Reset () override;
};

#endif /* Level3_hpp */
