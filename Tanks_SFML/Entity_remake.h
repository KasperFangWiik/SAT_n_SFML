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
struct Id_Pair {
    const int entity_id;
    T value;

    // how can i make so that the other id could have a nother type?
    bool compair_id(const Id_Pair& other_pair) {
        return entity_id == other_pair.entity_id;
    }

    /*
    // would this work? Brobably whon't to this because confusing
    bool operator==(Id_Pair<T>& other_pair) {
        return entity_id == other_pair.entity_id;
    }
    */
    T* getvalue() {
        return &value;
    }

    /*
    // shoudl i do this?
    T& getvalue() {
        return value;
    }
    */
};

template<typename T,typename U>
bool compair_diff_id_pair(const Id_Pair<T>& first_pair,const Id_Pair<U>& other_pair) {
    return first_pair.entity_id == other_pair.entity_id;
}

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


sf::Sprite* make_spritetest(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);

void make_sprite(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures);
//void make_sprite(std::string sprite_path, float size_factor, std::vector<spri_textur>& all_sprites_textures);

void playerKeyEvent(sf::Keyboard::Scancode key_code, bool pressed, Player& playerOne);

//sf::Sprite make_sprite(const char* sprite_path, float size_factor);

// borde nog va en virtual class...
class Entity {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:

    sf::Vector2f dirV{}; // directinal vector
    float speed{};
    float rot_angle{};
    int id; //const int id;
    // förklaring till inline's användning i detta forum: https://cplusplus.com/forum/beginner/282511/, only sinnse c++17
    //static inline int ID_sum = 0;


    sf::Sprite* spr{};

    Entity() : id(++ID_sum) {
        // id = ++ID_sum; // says  error E0137 expression most be a modefible lvalue 
    }// standard constructor

    ~Entity() {} // standard destructor

    // copy constructor (needed for use in std::vector, s there a standard?)
    // how does std::move() directly work..
    Entity(const Entity& other) : spr(other.spr), id(other.id) {
        dirV = other.dirV; // directinal vector
        speed = other.speed;
        rot_angle = other.rot_angle;
    }


    Entity(sf::Sprite* s) : id(++ID_sum) {
        //passing one shape setts the colider to the texture box
        spr = s;
    }


    template <typename T>
    Id_Pair<T> assosiate_vall_to_entity_id(const T& value) {
        return Id_Pair<T>{ id, value };
    }

    // should const T& value be const T&& value?
    template <typename T>
    void add_assosiate_vall_entity_id_to_vec(const T& value, std::vector<Id_Pair<T>>& vector) {
        vector.emplace_back(Id_Pair<T>{ id, value });
    }

    template<typename T>
    bool compair_entity_id(Id_Pair<T> other_pair) {
        return id == other_pair.entity_id;
    }

    template<typename T>
    T* find_id_pair(std::vector<Id_Pair<T>> other_pairs ) {
        for (Id_Pair<T>& other_pair : other_pairs)
            if (compair_entity_id(other_pair)) {
                T* test = &(other_pair.value);
                return test;
            }
        // what happens when nothing, return a nullptr?
        return nullptr;
    }

    sf::CircleShape* find_circleid_pair(std::vector<Id_Pair<sf::CircleShape>> other_pairs) {
        for (Id_Pair<sf::CircleShape>& other_pair : other_pairs)
            if (compair_entity_id(other_pair)) {
                //sf::CircleShape ttt = other_pair.value;
                sf::CircleShape* test = other_pair.getvalue();
                return test;
            }
        // what happens when nothing, return a nullptr?
        return nullptr;
    }


    // Might need to clearify that int i = 0; is a index of a specific vector
    template<typename T>
    bool find_index_of_id_pair(std::vector<Id_Pair<T>> other_pairs, int& index) {
        int i = 0;
        for (Id_Pair<T>& other_pair : other_pairs) {

            if (compair_entity_id(other_pair)) {
                index = i;
                return true;
            }
            i++;
        }
        // what happens when nothing, return a nullptr?
        return false;
    }

    template<typename T>
    bool bolean_find_id_pair(std::vector<Id_Pair<T>> other_pairs, T* value_pointer) {

        for (Id_Pair<T>& other_pair : other_pairs)
            if (compair_entity_id(other_pair)) {
                value_pointer = &(other_pair.value);
                return true;
            }

        return false;
    }

    template<typename T>
    bool bolean_find_id_pair(std::vector<Id_Pair<T>> other_pairs, Id_Pair<T>& value_pointer) {

        for (Id_Pair<T>& other_pair : other_pairs)
            if (compair_entity_id(other_pair)) {
                value_pointer = &other_pair;
                return true;
            }

        return false;
    }

    // BEHÖVER kopplasamman rotation och move för att se om det sker knas...
    void moveEnt(sf::Vector2<float> v) {
        if (spr)
            spr->move(v);

    }

    void RotEnt(float dt) { // might need to inforce that dt should be used
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

    void apply_rot_n_pos(sf::Transformable* obj) {
        obj->setPosition(spr->getPosition());
        obj->setRotation(spr->getRotation());
    }
// protected usefull when inherreting static varibles might not be usefull now that i'm using composition?
protected:
    static int ID_sum;
    //private:
    //    static int ID_sum;
};

bool find_entity_with_id(int search_id, std::vector<Entity>& entitys, Entity& return_entity);


/*
// get id and seatch for the ones with same id in sprite and collision std::vectors?

void moveEnt(sf::Vector2<float> v,sf::Vector2<Id_Pair<T>>) {
        if (spr)
            spr->move(v);

    }

void RotEnt(float dt) { // might need to inforce that dt should be used
        if (spr) {
            spr->rotate(sf::degrees(rot_angle * dt));
        }
    }

*/


/*
------------------------------------------------------------------------------------------------------------------
Player:
------------------------------------------------------------------------------------------------------------------
*/

// used only so that players are shecked for inputs
class Player{

public:
    // maby a player id is all that we should need... Maby we should make this private?
    Entity* player_entity{};

    bool w{}, a{}, s{}, d{};

    Player(Entity* const entity) { player_entity = entity; };

    void set_direction();

    //private:

    //protected:
};