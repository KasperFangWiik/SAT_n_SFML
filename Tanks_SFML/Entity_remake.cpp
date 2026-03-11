#include"Entity_remake.h"


int Entity::ID_sum = 0;

// the problem is probably that the id
sf::Sprite* make_spritetest(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures) {

    // after push_back here the pointer to all_sprites textures becomes a dangling pointer probably because of all_textures resize..
    all_textures.emplace_back();

    if (!((all_textures.back()).loadFromFile(sprite_path))) {
        std::cout << "Path for the Entity's texture did not work" << "\n";
    }

    all_sprites.emplace_back(sf::Sprite(all_textures.back()));

    auto sizeVec_sp = all_sprites.size() - 1;

    sf::Vector2f scale = (all_sprites.back()).getScale();
    (all_sprites.back()).setScale(scale * size_factor);

    return &all_sprites.back();
}


// const char* sprite_path
void make_sprite(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures) {

    // after push_back here the pointer to all_sprites textures becomes a dangling pointer probably because of all_textures resize..
    all_textures.emplace_back();

    if (!((all_textures.back()).loadFromFile(sprite_path))) {
        std::cout << "Path for Entity did not work" << "\n";
    }

    all_sprites.emplace_back(sf::Sprite(all_textures.back()));


    sf::Vector2f scale = (all_sprites.back()).getScale();
    (all_sprites.back()).setScale(scale * size_factor);

}
// behöver se till att den faktiskt hämtar rätt object och inte copierar
bool find_entity_with_id(int search_id, std::vector<Entity>& entitys, Entity& return_entity){

    for (Entity& e : entitys) {
        if (search_id == e.id) {
            return_entity = e; // this does not work if one varible in entity class is constant , Might work know when we are just copying memmory address?
            return true;
        }
    }

    return false;
}
/*
-------------------------------------------------------------------------------
Player related code:
-------------------------------------------------------------------------------
*/

void Player::set_direction() {

        sf::Vector2f& dirV = player_entity->dirV;

        dirV.y = float(w xor s);
        dirV.x = float(a xor d);

        // better as const float instead of macro
        const float ONE_DIV_SQRTWO = 0.70706781f;

        if ((w && d) || (w && a) || (s && d) || (s && a)) // moving diagonaly
            dirV = { ONE_DIV_SQRTWO , ONE_DIV_SQRTWO }; // normalized vector

        if (w == true)
            dirV.y = -dirV.y;

        if (a == true)
            dirV.x = -dirV.x;

}

void playerKeyEvent(sf::Keyboard::Scancode key_code, bool pressed, Player& playerOne) {
    typedef sf::Keyboard::Scancode S_Code;

    switch (key_code) {
    case S_Code::W: case S_Code::Up:
        playerOne.w = pressed;
        break;
    case S_Code::A: case S_Code::Left:
        playerOne.a = pressed;
        break;
    case S_Code::S: case S_Code::Down:
        playerOne.s = pressed;
        break;
    case S_Code::D: case S_Code::Right:
        playerOne.d = pressed;
    }
}