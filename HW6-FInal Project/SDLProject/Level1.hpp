

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
