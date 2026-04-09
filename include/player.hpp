#pragma once
#include<SFML/Graphics.hpp>
class Player{
    public:
    Player();
    ~Player();

    void draw();
    
    void setPos(sf::Vector2f newPos);

    void setSpeed(float newSpeed);
    void setDash(float newDash);

    void updateHp(float newHp);
    void updateStamina(float newStamina);

    sf::Vector2f getPos();

    float getSpeed();
    float getDash();

    float getHp(float newHp);
    float getStamina(float newStamina);

    private:
        float speed;
        float dash;

        float hp;
        float stamina;



};