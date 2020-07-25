//
//  Level4.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level4_hpp
#define Level4_hpp

#include "Scene.hpp"

class Level4 : public Scene
{
public:
    void Initialize() override;
    void Update (float deltaTime) override;
    void Render (ShaderProgram *program) override;
    void Reset () override;
};


#endif /* Level4_hpp */
