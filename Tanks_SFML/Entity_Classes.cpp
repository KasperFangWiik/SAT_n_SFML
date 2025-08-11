#include"Entity_Classes.h"


int Entity::ID_sum = 0;
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

sf::Sprite* make_spritetest(std::string sprite_path, float size_factor, std::vector<sf::Sprite>& all_sprites, std::vector<sf::Texture>& all_textures) {

    // after push_back here the pointer to all_sprites textures becomes a dangling pointer probably because of all_textures resize..
    all_textures.emplace_back();

    if (!((all_textures.back()).loadFromFile(sprite_path))) {
        std::cout << "Path for Entity did not work" << "\n";
    }

    all_sprites.emplace_back(sf::Sprite(all_textures.back()));

    auto sizeVec_sp = all_sprites.size() - 1;

    sf::Vector2f scale = (all_sprites.back()).getScale();
    (all_sprites.back()).setScale(scale * size_factor);

    return &all_sprites.back();
}

bool find_entity_with_id(int search_id, std::vector<Entity>& entitys, Entity& return_entity) {

    for (Entity& e : entitys) { // const leads to anachronism used ???
        if (search_id == e.id) {
            return_entity = e; // this does not work if one varible in entity class is constant 
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

// behöver för någon anledning inte skriva : id(++ID_sum) detta görs av sig skälv, varför?
Player::Player(sf::Shape* c, sf::Sprite* s) { // const sf::Texture &t
    
    coli = c;
    // .loadFromFile
    //tex = t; // ("./Resource Files/FyshSkull.png", false);
    spr = s;
}

Player::Player(sf::Shape* c) { // const sf::Texture &t
    coli = c;
    // .loadFromFile
    //tex = t; // ("./Resource Files/FyshSkull.png", false);
    spr = nullptr;
}

void Player::moveEnt(sf::Vector2<float> v) {
    if(coli != nullptr)
        coli->move(v);

    if (spr != nullptr)
        spr->move(v);
}

void Player::set_direction() {
    dirV.y = int(w xor s);
    dirV.x = int(a xor d);

    // better as const float instead of macro
    const float ONE_DIV_SQRTWO = 0.70706781f;

    if ((w && d) || (w && a) || (s && d) || (s && a)) // moving diagonaly
        dirV = { ONE_DIV_SQRTWO , ONE_DIV_SQRTWO }; // normalized vector

    if (w == true)
        dirV.y = -dirV.y;

    if (a == true)
        dirV.x = -dirV.x;
}

void Player::dirMove(float dt) { // the function is overiding a virtual function, then the function is automatically considered virtual function in the derived class

    //set_direction();

    //std:: cout << "( " << n.dirV.x << ", " << n.dirV.y << " )" <<"\n";
    moveEnt(dirV * speed * dt); // scales the drection vector by the speed
}

void playerKeyEvent(sf::Keyboard::Scancode key_code, bool pressed ,Player& playerOne){
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

// can i do this without send a player to the function?
// VERKAR SOM ATT std::optional<sf::Event> event inte kan passeras till functionen och använda getIf på
// Då den inte verkar kunna tyda att eventet är av subtypen sf:Event längre
void keyPressControll(std::optional<sf::Event> event, Player* const playerOne) {

    sf::Keyboard::Scancode tmp_scancode;
    bool Pressed = false;
    bool Released = false;

    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        Pressed = true;
        tmp_scancode = keyPressed->scancode;
    }
    else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        Released = true;
        tmp_scancode = keyReleased->scancode;
    }

    sf::Keyboard::Key::Down;
    sf::Keyboard::Key::Down;
    
    sf::Keyboard::Scancode::W;
    sf::Keyboard::Scancode::Down;

    // testa:
    // if (keyPressed || keyReleased)
    if (Pressed || Released)
    {
        switch (tmp_scancode) {

        case sf::Keyboard::Scancode::W:
            playerOne->w = Pressed;
            break;
        case sf::Keyboard::Scancode::A:
            playerOne->a = Pressed;
            break;
        case sf::Keyboard::Scancode::S:
            playerOne->s = Pressed;
            break;
        case sf::Keyboard::Scancode::D:
            playerOne->d = Pressed;

        }

    }
}


/*
// &s reference is destroyd/ danglling ref???
Player::Player(const char* sprite_path, float size_factor) {
    sf::Texture t;

    if (!t.loadFromFile(sprite_path)) {
        std::cout << "Path for Entity did not work" << "\n";
    }

    sf::Sprite s(t);

    sf::Vector2f scale = s.getScale();
    s.setScale(scale.x * size_factor, scale.y * size_factor);

    // is s destroyed after player is made?
    spr = &s;
}
*/