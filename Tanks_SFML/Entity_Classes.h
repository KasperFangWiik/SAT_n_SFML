#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include<filesystem>

/* 
// denna funkar inte ny då sf::Sprite inte har en standard constructor längre
struct spri_textur {
    sf::Sprite spr;
    sf::Texture tex;
};
*/

class Entity;
class Player;


sf::Sprite* make_spritetest(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);
void make_sprite(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);
//void make_sprite(std::string sprite_path, float size_factor, std::vector<spri_textur>& all_sprites_textures);
void playerKeyEvent(sf::Keyboard::Scancode key_code, bool pressed, Player& const playerOne);
void keyPressControll(std::optional<sf::Event> event, Player* const playerOne);

//sf::Sprite make_sprite(const char* sprite_path, float size_factor);

// borde nog va en virtual class...
class Entity {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:

    sf::Vector2f dirV{}; // directinal vector
    float speed{};

    // sf::Shape* texturebox{};  // shape for texture

    sf::Shape* coli{};        // colition- & hitbox.
    //sf::Texture* tex{};      // the texture
    sf::Sprite* spr{};

    Entity() {}// standard constructor
    ~Entity() {} // standard destructor

    // copy constructor (needed for use in std::vector, s there a standard?)
    Entity(const Entity& other) : coli(other.coli), spr(other.spr) {}

    /*
    Entity(const Entity& other) : coli(other.coli),
                                  speed(other.speed),
                                  posV(other.posV),
                                  spr(other.spr) {}


    Entity(const char* sprite_path, float size_factor) {
        sf::Texture t;
        if (!t.loadFromFile(sprite_path)) {
            std::cout << "Path for Entity did not work" << "\n";
        }

        sf::Sprite s(t);

        sf::Vector2f scale = s.getScale();
        s.setScale(scale.x * size_factor, scale.y * size_factor);
        spr = &s;
    }
    */

    Entity(sf::Shape& c, sf::Sprite* s) {
        //passing one shape setts the colider to the texture box
        spr = s;
        coli = &c;
    }

    Entity(sf::Sprite* s) {
        //passing one shape setts the colider to the texture box
        spr = s;
    }

    Entity(sf::Shape* c) {
        //passing one shape setts the colider to the texture box
        coli = c;
    }

    // BEHÖVER kopplasamman rotation och move för att se om det sker knas...
    void moveEnt(sf::Vector2<float> v) {
        /*
        * if (!spr) // spr == NULL samma som !n->spr
            coli->move(v);
        else
            spr->move(v);

        */

        // borde möjlligen alldrig flytta på en sf::shape, bara ändra säga att den har samma kordinater/ possition som entityn? 
        if (coli) // spr == NULL samma som !n->spr
            coli->move(v);

        if (spr)
            spr->move(v);

    }

    virtual void set_direction() {}

    virtual void dirMove(float dt) { // move entity in the direction of direction vector   
        moveEnt(dirV * speed * dt); // scales the drection vector by the speed
    }

    virtual void give_dir_Move(sf::Vector2f other_dir, float dt) { // move entity in the direction of direction vector   
        moveEnt(other_dir * speed * dt); // scales the drection vector by the speed
    }

    //private:
    //protected:
};


// used only so that players are shecked for inputs
class Player : public Entity {

public:
    //sf::Vector2f dirV{}; // directinal vector
    //float speed{};

    bool w{}, a{}, s{}, d{};

    Player(sf::Shape* c, sf::Sprite* s);

    void moveEnt(sf::Vector2<float> v);

    void set_direction();
    // move entity in the direction of direction vector
    void dirMove(float dt) override;
    //private:

    //protected:
};

/*
sf::Sprite make_sprite(const char* sprite_path, float size_factor) {
    sf::Texture t;

    if (!t.loadFromFile(sprite_path)) {
        std::cout << "Path for Entity did not work" << "\n";
    }

    sf::Sprite s(t);

    sf::Vector2f scale = s.getScale();
    s.setScale(scale.x * size_factor, scale.y * size_factor);

    // is s destroyed after player is made?
    return s;
}


const std::filesystem::path fi_path = "C:/Users/User/Desktop/sprites/FyshSkull.png";
    sf::Texture t;
    if (!t.loadFromFile(fi_path.string())) { // fi_path.string()
*/