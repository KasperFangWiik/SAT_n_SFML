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


// Id_Pair& operator =(const Id_Pair&) {} //will i have to do this??

template <typename T>
struct Id_Pair{
    const int entity_id;
    T value;

    bool compair_id(Id_Pair<T>& other_pair) {
        return entity_id == other_pair.entity_id;
    }
};

// a entity can be contected to more than one collider but no colider whill be contected to more than one.
// a collider vill always have the same transform as the entity/sprite could this lead to double calculation?
// maby only when colliders are rendred to...

/*
två problem:

1. när std::vector<sf::Sprite> resizas så borde vi fortfarande ha problemet med att 
    Sprite(&texture) borde invalidata &texture referencen eller tar std::shared_ptr<T> 
    och fixar det?
2. om vi har std::vector<std::shared_ptr<sf::Texture>> då är pekarna satta continuerligt i minnet men 
   Varje sf::Texture object ligger sudo randomly omkring på heapen
*/
struct Texture_Id_Pair {
    const int Texture_id;
    std::shared_ptr<sf::Texture> tex; 
};

/*
// assosiating more than one might be usefull
template<typename T, size_t L>
class MyArray
{
    T arr[L];
public:
    MyArray() { ... }
};
*/

struct sprite_point {
    sf::Sprite* not_same_pointer;
};

sf::Sprite* make_spritetest(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);
sprite_point make_spritetesttv(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);

void make_sprite(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);
//void make_sprite(std::string sprite_path, float size_factor, std::vector<spri_textur>& all_sprites_textures);

void playerKeyEvent(sf::Keyboard::Scancode key_code, bool pressed, Player& playerOne);
void keyPressControll(std::optional<sf::Event> event, Player* const playerOne);

//sf::Sprite make_sprite(const char* sprite_path, float size_factor);

// borde nog va en virtual class...
class Entity {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:

    sf::Vector2f dirV{}; // directinal vector
    float speed{};
    float rot_angle{};
    //const int id;
    int id;
    // förklaring till inline's användning i detta forum: https://cplusplus.com/forum/beginner/282511/, only sinnse c++17
    //static inline int ID_sum = 0;
    

    // sf::Shape* texturebox{};  // shape for texture

    sf::Shape* coli{};        // colition- & hitbox.
    //sf::Texture* tex{};      // the texture
    sf::Sprite* spr{};

    Entity(): id(++ID_sum) {
        // id = ++ID_sum; // says  error E0137 expression most be a modefible lvalue 
    }// standard constructor

    ~Entity() {} // standard destructor

    // copy constructor (needed for use in std::vector, s there a standard?)
    // how does std::move() directly work..
    Entity(const Entity& other) : coli(other.coli), spr(other.spr), id(other.id) {} // ID_sum

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

    Entity(sf::Shape& c, sf::Sprite* s) : id(++ID_sum) {
        //passing one shape setts the colider to the texture box
        spr = s;
        coli = &c;

    }

    Entity(sf::Sprite* s) : id(++ID_sum) {
        //passing one shape setts the colider to the texture box
        spr = s;
    }

    Entity(sf::Shape* c) : id(++ID_sum) {
        //passing one shape setts the colider to the texture box
        coli = c;
    }

    /*
     // DENNA LÖSTE Problemet med std::vector som uppcom då const int id; las till, e denna korrect?
    Entity& operator =(const Entity& other) = delete;//{ --ID_sum; return ; }

    Entity(Entity&& other) noexcept : 
        coli(other.coli), spr(other.spr), id(ID_sum) {}

    Entity& operator =(Entity&& other) {
        coli = std::move(other.coli);
        spr = std::move(other.spr);

        dirV = std::move(other.dirV);
        speed = std::move(other.speed);
        rot_angle = std::move(other.rot_angle);
        return *this;
    }
    
    
    */



    template <typename T>
    Id_Pair<T> assosiate_vall_to_entity_id(const T&& value ) {
        return Id_Pair<T>{ id, value };
    }

    // should const T& value be const T&& value?
    template <typename T>
    void add_assosiate_vall_entity_id_to_vec(const T&& value, std::vector<Id_Pair<T>>& vector) {
        vector.emplace_back(Id_Pair<T>{ id, value });
    }

    template<typename T>
    bool compair_entity_id(Id_Pair<T> other_pair) {
        return id == other_pair.entity_id;
    }

    // BEHÖVER kopplasamman rotation och move för att se om det sker knas...
    void moveEnt(sf::Vector2<float> v) {
        // borde möjlligen alldrig flytta på en sf::shape, bara ändra säga att den har samma kordinater/ possition som entityn? 
        if (coli) // spr == NULL samma som !n->spr
            coli->move(v);

        if (spr)
            spr->move(v);
        
    }

    void RotEnt(float dt) { // might need to inforce that dt should be used
        if (coli) { // spr == NULL samma som !n->spr
            coli->rotate(sf::degrees(rot_angle * dt));
        }

        if (spr) {
            spr->rotate(sf::degrees(rot_angle * dt));
        }
    }

    virtual void set_direction() {}

    void dirMove(float dt) { // move entity in the direction of direction vector   
        moveEnt(dirV * speed * dt); // scales the drection vector by the speed
    }

    void give_dir_Move(sf::Vector2f other_dir, float dt) { // move entity in the direction of direction vector   
        moveEnt(other_dir * speed * dt); // scales the drection vector by the speed
    }

    protected:
        static int ID_sum;
    //private:
    //    static int ID_sum;
};

bool find_entity_with_id(int search_id, std::vector<Entity*>& entitys, Entity& return_entity);


// used only so that players are shecked for inputs
class Player : public Entity {

public:
    //sf::Vector2f dirV{}; // directinal vector
    //float speed{};

    //const int id; // varför kan jag inte ärva denna ??? GÖR detta och behöver inte änns öka den i constructorn..

    bool w{}, a{}, s{}, d{};

    Player(sf::Shape* c, sf::Sprite* s);

    Player(sf::Shape* c);
    void set_direction();

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