

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
