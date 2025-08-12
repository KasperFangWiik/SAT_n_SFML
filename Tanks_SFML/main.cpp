
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
#include "collision_self.h"
//#include "chunk_uppdate.h"

#include "chunk_composition.h"
//#define ONE_DIV_SQRTWO 0.70706781f //float value of ONE_DIV_SQRTWO = 1/sqrt(2)



/*
______________________________________________________________________________
                                TODO LIST:

        3. Creat a function that sifts throu the two lists and resolvs collisions...

    1. find how manny copy constructor calls that are made and se if you can eliminate them we should at this momment probably have one copy call per entity and player
    2. fix rect on circle collision...
    3. Creat a function that sifts throu the two lists and resolvs collisions...
    4. implement mecanic where you can shoot "bullets" in the direction of mose
    5. make them bounce once and disaper the next time...

    1. Implement simplistic broad faze collision?
    2. make a function that divides a map in to chunks, Semi correct formulerat...
    3. (State machine creator...)
    4. // this function needs to be revisit to controll when entitys are renderd to simulate depth
    void render_chunk(sf::RenderWindow& window)

    maintenece work:
    1. make .cpp files for .h files correctly
    2. make wrapper SFML specific functions for intersect and collision function so that there more generaliced
______________________________________________________________________________

Questions:
1. make so that every entity has a unik ID, use static varible? Or other ways?  BORDE VARA KLAR
    UNDERSÖK:!!!!!!!!!!!!!!!!!!!!!!!
    // DENNA LÖSTE Problemet med std::vector som uppcom då const int id; las till
    Entity& operator =(const Entity&) {}

    // behöver för någon anledning inte skriva : id(++ID_sum) detta görs av sig skälv, varför?
     Player::Player(sf::Shape* c, sf::Sprite* s)



Noah snackade något om hur man skulle  hantera texturer eller shapes...

std::vector nyttjar sig av RAII och deletar sig skälv efter gåt ut ur scope
och kallar på destructorn av objectet inom den men om denna destructor inte
kallar på delet kommer det skapa minnes luckor?

*/


// skriv inte för mycket kod, testa med att få grund saker att funka 
// exemple, function som ritar rectangel, kunna flytta på den 
// flytta den sedan med inputs från keyboardet
// kolla upp hur man använder deltaTime / hur spel uppdaterar frames utan att bero på frame rate?


// varför använder man get och sett functioner å varför private??
// there is a difference between moving and non-moving entitys
// maby this should be a identity factory 

//void move_intersect(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt);

// We whill probably creat a game class that controlls game settings and one of it's memberfunction should be this?
void uppdate_state(sf::RenderWindow& window, Player& players, Chunk& chunk, bool new_chunk, sf::Clock& clock) {

    sf::Time dt = clock.getElapsedTime(); // .asMilliseconds() direkt här ?

    // varför är "frameraten" alltid lite större
    std::cout << 1.0f / dt.asSeconds() << "\n";


    // we also whant to check if the moveble objects are
    // in the chunk
    window.clear();

    // jag vill bara resiza window om vi ska rendera en ny chunk men set size behåller storleken hela tiden...
    if (new_chunk) { // resize the window
        sf::FloatRect view = chunk.background->getGlobalBounds();

        window.setView(sf::View(view));
        // var för har de implementerat så att setSize tar en referens till vector?
        // leder visserligen till mindre minne som flyttar men inte mycket och svårare att bara ändra storlek 

        window.setSize(sf::Vector2u(view.size));
        new_chunk = false;
    }
    /*
        // could uppdate the player direction mm... Here instead of in the chunk?
    for (Player* player : players) {
        player->set_direction();
    }
    */

    players.set_direction();
    chunk.move_transformables(dt.asSeconds());
    chunk.render_chunk(window); // how do i wan't to decied the order of rendering to decide depth
    chunk.render_chunk_coliders(window);

    /*
    window.draw(*chunk.background);

    // if(moved) then uppdate move...
    //move_entitys(moveb_enlist, dt.asSeconds());
    float pixel_size_factor = 1.0;

    float test = moveb_enlist[0]->speed;

    //move_with_Coll_entitys(moveb_enlist, all_coliders, pixel_size_factor, dt.asSeconds());
    
    move_intersect(moveb_enlist, all_coliders, pixel_size_factor, dt.asSeconds());

    render_Entitys(window, moveb_enlist);

    render_chunk(window, chunk);
    render_Entitys(window, fixed_enlist);
    */

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

    //std::vector<sf::Sprite>  all_sprites; // becaues the entity class contains only pointers to Sprites i nead a place where they live

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

    const float sprite_size_factor = 4.0;

    std::string map_path = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/chuncks";
    int map_size = 9;
    Chunk ch = Chunk(map_path, map_size, sprite_size_factor, all_sprites, all_textures);
    bool new_chunk = true;

    // skulle deta vara en lösning till resize problemet, Svar Nej
    // i teori är det lättar e att hålla koll på när resize sker och att då ändra pekaren då kanske
    //std::vector<spri_textur> all_sprites_textures;

    sf::CircleShape shape1(25.f);

    const char tex_piller_file_path[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/piller_head.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures); // magic number 4.0 that is the size factor should be clearer strong typing?
    sf::Sprite* piller_sprite = &all_sprites.back();
    
    // change rotation origin /center to the center of the Sprite, i proobaby need to change the colid shapes center to...
    //piller_sprite->setOrigin((sf::Vector2f)piller_sprite->getTexture().getSize() / 2.f);
    
    sf::FloatRect colid_pill = piller_sprite->getGlobalBounds();
    //Entity piller(&shape1); // piller_sprite
    Entity piller(piller_sprite);

    // one piller head is (15 * 4) in "pixel" size
    const float piller_pixel_size = 15 * sprite_size_factor;
    piller.moveEnt({ piller_pixel_size * 6 , piller_pixel_size * 4 }); // 8*2*4 = 16 => 4 pixels, 16 pixels, 64 = 17 pix,  8*8-4 = 8*2*4-4 = 16*4-4 = 15*4
    piller.rot_angle = 50;
    shape1.setOrigin(piller_sprite->getOrigin());

    ch.colider_move_ent_to_chunk(piller,std::move(shape1));

    //std::cout << piller.id << "\n";
    //piller.spr->setRotation(sf::degrees(45));

    sf::CircleShape shape2(25.f);

    // this might make one extra copy...
    const char tex1[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/player.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* s = &all_sprites.back();// &all_sprites.back();

    // changes the origin that we rotate around Could temporarly change the origin and rotate right?
    s->setOrigin((sf::Vector2f)s->getTexture().getSize() / 2.f);
    shape2.setOrigin((sf::Vector2f)s->getTexture().getSize() / 2.f);
    Entity pl_entity(s);
    //Player playerOne(&pl_entity); //  Player playerOne(&shape2, s);

    ch.add_ent_to_chunk(pl_entity, pl_entity.assosiate_vall_to_entity_id(std::move(shape2)));

    Player playerOne(&ch.chunks_entitys.back());
    //ch.print_entity_ids();
    //playerOne.add_assosiate_vall_entity_id_to_vec(std::move(shape2), all_circle_colliders);


    // all_textures.resize(2); // tydligt visande att resize på all_textures är vad som stör all_sprites 

    playerOne.player_entity->speed = 200.0f;
    playerOne.player_entity->rot_angle = 0.0f;

    /*
    // why does this make an error does moveb_enlist[0] not index the playerOne and
    std::vector<Entity> moveb_enlist = { playerOne };
    Player& plone = &(moveb_enlist[0]);

    */

    // Inte bra med magic numbers...
    sf::RenderWindow window(sf::VideoMode({20, 20}), "SFML works!");

    // kan inte ha för hög frame rate det leder till att man har alldeles för liten korregering till hasstigheten som rundas ned till noll
    window.setFramerateLimit(144);

    // behövs denna stå i början?
    sf::Clock clock;

    while (window.isOpen()) {
        
        /*const*/ std::optional<sf::Event> event;

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

        //sf::Time dt = clock.getElapsedTime(); // .asMilliseconds() direkt här ?

        //std::cout << dt.asMilliseconds() << "\n";
        //std::vector<Player*>pls{ &playerOne };
        uppdate_state(window, playerOne, ch, new_chunk, clock);
        
        //std::cout << 1.0f / dt.asSeconds() << "\n";
        //std::cout << dt.asMilliseconds() << "\n";

        // resets the clock so we don't check tot acumalitive time
        //clock.restart().asMilliseconds();
    }

    return 0;

}

/*
______________________________________________________________________________
                                Collisondetection:
   testing SAT algorithm, this is usfull links/sotces:
   https://dyn4j.org/2010/01/sat/
   https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
______________________________________________________________________________

*/

// struct sortof taken from  pdf, contains all data that is neaded to resolv a collison..
/*
We note that while this information is suitable to resolve relatively simple collisions, more complex
collision resolution requires more information. This will be explored in more detail in the tutorial on
Manifolds.
*/



/*
// semi funkar, ett problm är att jag inte vet hur mycket jag ska röra mig bakåt..
void move_intersect(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt) {

    for (Entity* n : moveb_enlist) { // ska man använda const här? const entity n
        bool collision = false;

        n->set_direction();
        n->RotEnt(dt);
        n->dirMove(dt);
        for (const Entity* m : moveb_enlist) {

            // checkar bara att de har samma pekare/minnes adress.. behövs starkare eqvivalence?
            if (m == n) {
                //std::cout << "colison was made" << "\n";
                continue;
            }

            sf::Vector2f test_vec{};
            //if (current_colider_rect.findIntersection(tmp_colider_rect))
            //if(m->coli != nullptr|| n->spr != nullptr)
            if (m->coli != nullptr || n->spr != nullptr)
                if (n->speed > 0 && simple_rect_collision(n->spr,m->spr, test_vec)) { //simple_rect_collision(n->spr, m->spr, test_vec)
                    if (test_vec == sf::Vector2f{ 0,0 })
                        continue;

                    n->moveEnt(test_vec); 
                    collision = true;
                }
                //sf::RectangleShape test;
                //test.getGlobalBounds(); 
                
            // semi race condition som sker i collison om man byter direction och är inne i functionen aka innan other_dir sätts rätt så  kan fel vector nyttjas
            // verkar främst ske när jag trycker ned två knappar sammtidigt i olika ricktningar
        }

        if (!collision) {
           //n->dirMove(dt); // när jag använder denna så blir de knas
        }
    }
}

*/



/*
Cmake forwarding..

x är först i kön och testar sinn colison så om y som går efter rör sig sammtidigt emot x så kommer väll x putta y? NEJ
*/


/*
*
*
* smart pointers kolla upp?
* bara använda ett shape som ändras form på? Det ultimata hade varit att vissa entitys har egna shapes och andra bara använder sig av en universell shape de ändrar formen på
* composition för player iställät för att ärva entity klassen?
*
    *   what are we making?:
    *   the gole is to make retro mario 2D "level"
    *   subgoals:
    *    - paint ground
    *    - paint "hitbox of mario" on the ground
    *    - move the box back and forth
    *    - hit box shecks, ray infront and under, make a nother box that stops you
    *    - make the box able to jump.
    *    - add an gomba box
    *    - add the sprites to mario for jumping and moving, maby running allso.

        whishfull thinking:

        main file has the lteral game loop

        what objects do we nead:
        gameobject with start and end object function?

        boddy object that inclodes position on screan, movement vector?

        character object.

        hitbox object?

        entity object????

        --------------------------------
        what in main:

        loop for opening screen that ether constucts a game from standard constuctor or the object is modifyed by meny
        after settings / klicked on start, the game object is created and used to sett windowstandards

        calculationsfunc contains all changes to positions and hitboxes mm...
        renderfunc that renders everyting that should reander.

    */

    /*
    * nya idèer
    * Använd en static class varible för ett shape som då förändras på utifrån en textur som "skapas"
    * varje gång vi ska draw en build av objectet.
    * Ska man kolla om objectet ändrats på innan man utför drwa en liksom fixa en flagga i objectet som sätts
    till false om objectet har ändrat på sig.

    skillja på ositionella värden av entity och texturen, men hur ser man då till att rätt textur går till rätt entity utan extra värden i classen??





                    sf::Vector2u sc = window.getSize();
                    sf::Vector2f t = ((sf::Vector2f) sc )
                    shape1.setScale( t.x/100.f, t.y/100.f);
                    shape2.setScale(50.f/ shape2.getLocalBounds().width, 50.f/ shape2.getLocalBounds().height);
                    //shape2.getRadius();


     */
