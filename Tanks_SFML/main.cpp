
#include<iostream>
#include<filesystem>
#include <string>
//#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>

//#include "Entity_Classes.h"
#include "Entity_remake.h"
//#include "map_classes.h"
//#include "Standard_Vectorfunc.h"
//#include "chunk_uppdate.h"
#include"essential_collision.h"

#include "chunk_composition.h"
//#define ONE_DIV_SQRTWO 0.70706781f //float value of ONE_DIV_SQRTWO = 1/sqrt(2)

/*
______________________________________________________________________________
                                Collisondetection:
   testing SAT algorithm, this is usfull links/sotces:
   https://dyn4j.org/2010/01/sat/
   https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
______________________________________________________________________________

*/

/*
______________________________________________________________________________
                                TODO LIST:

    Har hittat ett papper som beskriver hur SAT kan generaliseras för object som rör på sig så först gör detta sedan swept collision
    1. (start with a sweept circle to circle collision)
    2. (i need to know what's a good way of giving attributes to entitys/NPC's?)
        - implement mecanic where you can shoot "bullets" in the direction of mose
        - make them bounce once and disaper the next time...


    1. Implement simplistic broad faze collision?
    2. make a function that divides a map in to chunks, Semi correct (formulerat...)
    3. (State machine creator...)
    4. this function needs to be revisit to controll when entitys are renderd to simulate depth (Painter's algorithm?)
     void render_chunk(sf::RenderWindow& window)
    5. expand so that different actions can happen when collision happen between different ?Entitys? / object types

    maintenece work:
    1. make .cpp files for .h files correctly
    2. make wrapper SFML specific functions for intersect and collision function so that there more generaliced
    3. clean up chunk_composition
    4. find how manny copy constructor calls that are made and se if you can eliminate them we should at this momment probably have one copy call per entity and player
    5. is it posible to simplify circle vs OBB and vise versa...
______________________________________________________________________________

std::vector nyttjar sig av RAII och deletar sig skälv efter gåt ut ur scope
och kallar på destructorn av objectet inom den men om denna destructor inte
kallar på delet kommer det skapa memory leaks?

*/



// We whill probably creat a game class that controlls game settings and one of it's memberfunction should be this?
void uppdate_state(sf::RenderWindow& window, Player& players, Chunk& chunk, bool new_chunk, sf::Clock& clock) {

    sf::Time dt = clock.getElapsedTime(); 

    window.clear();

    // jag vill bara resiza window om vi ska rendera en ny chunk men set size behåller storleken hela tiden...
    if (new_chunk) { // resize the window
        sf::FloatRect view = chunk.background->getGlobalBounds();

        window.setView(sf::View(view));
        window.setSize(sf::Vector2u(view.size));
        new_chunk = false;
    }

    players.set_direction();

    chunk.move_all_transformables(dt.asSeconds());
    chunk.resolve_collisions();

    chunk.render_chunk(window); // how do i wan't to decied the order of rendering to decide depth
    chunk.render_chunk_coliders(window);

    clock.restart(); //clock.restart().asSeconds(); // clock->restart().asMilliseconds();

    window.display();
}

/*
// do we whant an std::vector<entity>* or std::vector<shape>*?
void render(sf::RenderWindow* window, std::vector<Movable_Entity*> *enlist, sf::Clock* clock) {

    sf::Time dt = clock->getElapsedTime(); // .asMilliseconds() direkt här ?

    // varför är "frameraten" alltid lite större
    std::cout << 1.0f/dt.asSeconds() << "\n";

    window->clear();

    // render the shapes of the entitys in the entity list
    for (Movable_Entity* n : *enlist) { // ska man använda const här? const entity n

        //n->dirMove(dt.asMilliseconds());
        n->dirMove(dt.asSeconds());
        window->draw(*(n->texturebox) );// kalla på egen funk som formaterar utseendt eller användereget
    }

    //clock->restart().asMilliseconds();
    clock->restart().asSeconds();

    window->display();
}
*/

int main()
{

    /*
     ___________________ Använda unordered_map kanske, kan se över att spara sakerna då i ett set då jag inte vill åter uprepa några texturer
     DET JAG VILL GÖRA ÄR ATT SPARA Hash key'n till den texturen som är assosierad med entityn...
    */

    // i don't think i will have a problem with resizing vectors with shapes, no dependency on references like sprites for texture ref

    // std::vector<Sprite_Texture_Id_Pair> sprites_n_textures_in_chunk; // maby when one is not found assume resize and re sett all texture references?

    std::vector<sf::Sprite>  all_sprites;
    all_sprites.reserve(20);
    std::vector<sf::Texture> all_textures;
    all_textures.reserve(20);

    float sprite_size_factor = 4.0;
    std::string map_path = "./sprites/chuncks";// Laptop file path
    int map_size = 9;
    Chunk ch = Chunk(map_path, map_size, sprite_size_factor, all_sprites, all_textures);
    bool new_chunk = true;

    const char tex_piller_file_path[] = "./sprites/piller_head.png";// Laptop file path
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* piller_sprite = &all_sprites.back();
    
    // change rotation origin /center to the center of the Sprite, i proobaby need to change the colid shapes center to...
    //piller_sprite->setOrigin((sf::Vector2f)piller_sprite->getTexture().getSize() / 2.f);
    
    sf::FloatRect colid_pill = piller_sprite->getGlobalBounds();
    Entity piller(piller_sprite);
    
    
    // one piller head is (15 * 4) in "pixel" size
    float piller_pixel_size = 15 * sprite_size_factor;
    piller.moveEnt({ piller_pixel_size * 6 , piller_pixel_size * 4 }); // 8*2*4 = 16 => 4 pixels, 16 pixels, 64 = 17 pix,  8*8-4 = 8*2*4-4 = 16*4-4 = 15*4
    piller.rot_angle = 50;
   // shape1.setOrigin((sf::Vector2f)piller_sprite->getTexture().getSize()/ 2.f);
    //shape1.setOrigin(piller_sprite->getOrigin());

    sf::Vector2f size2 = (sf::Vector2f)(piller_sprite->getTexture()).getSize() * sprite_size_factor;
    sf::RectangleShape rectcollshapeFirst = sf::RectangleShape(size2);

    //rectcollshapeFirst.setOrigin((sf::Vector2f)piller_sprite->getTexture().getSize() / 2.f);

    
    sf::ConvexShape convex2;

    convex2.setPointCount(3);
    convex2.setPoint(0, { 0.f, 0.f });
    convex2.setPoint(1, { 150.f, 10.f });
    convex2.setPoint(2, { 120.f, 90.f });
    ch.colider_move_ent_to_chunk(piller, std::move(convex2));
    

    //sf::CircleShape shape1(25.f);
    //ch.colider_move_ent_to_chunk(piller,std::move(shape1));

    //ch.colider_move_ent_to_chunk(piller, std::move(rectcollshapeFirst));


    sf::CircleShape shape2(25.f);

    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* s = &all_sprites.back();// &all_sprites.back();


    Entity pl_entity(s);
    sf::Vector2f size1 = (sf::Vector2f)(s->getTexture()).getSize() * sprite_size_factor;
    sf::RectangleShape rectcollshape = sf::RectangleShape(size1);


    // create an empty shape
    sf::ConvexShape convex;

    // resize it to 5 points
    //convex.setPointCount(5);
    convex.setPointCount(3);
    // define the points
    /*
    convex.setPoint(0, { 0.f, 0.f });
    convex.setPoint(1, { 150.f, 10.f });
    convex.setPoint(2, { 120.f, 90.f });
    convex.setPoint(3, { 30.f, 100.f });
    convex.setPoint(4, { 0.f, 50.f });
    */
    convex.setPoint(0, { 0.f, 0.f });
    convex.setPoint(1, { 150.f, 10.f });
    convex.setPoint(2, { 120.f, 90.f });

    //ch.add_ent_to_chunk(pl_entity, pl_entity.assosiate_vall_to_entity_id(std::move(rectcollshape)));

    ch.add_ent_to_chunk(pl_entity, pl_entity.assosiate_vall_to_entity_id(std::move(convex)));

    Player playerOne(&ch.chunks_entitys.back());

    // all_textures.resize(2); // tydligt visande att resize på all_textures är vad som stör all_sprites 

    playerOne.player_entity->speed = 200.0f;
    playerOne.player_entity->rot_angle = 0.0f;

    sf::RenderWindow window(sf::VideoMode({20, 20}), "SFML works!");

    // kan inte ha för hög frame rate det leder till att man har alldeles för liten korregering till hasstigheten som rundas ned till noll
    window.setFramerateLimit(144);

    sf::Clock clock;

    while (window.isOpen()) {
        
        std::optional<sf::Event> event;

        while (event = window.pollEvent()) {
            if (event-> is<sf::Event::Closed>())
                window.close();

            if (const auto* new_view_size = event->getIf<sf::Event::Resized>()) {
                // funkar men varför 
                const sf::Vector2f zero_pos = { 0.0f, 0.0f};

                sf::FloatRect view(zero_pos, sf::Vector2f(new_view_size->size));
                window.setView(sf::View(view));
            }

            bool Pressed = false;
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                Pressed = true;
                playerKeyEvent(keyPressed->scancode, Pressed, playerOne); // skickar en pekare av player till funktionen varje gång det är ett event, inte optimalt?
            }
            else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                playerKeyEvent(keyReleased->scancode, Pressed, playerOne);
            }
        }

        uppdate_state(window, playerOne, ch, new_chunk, clock);
    }

    return 0;

}
