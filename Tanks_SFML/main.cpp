
#include<iostream>
#include<filesystem>
#include <string>
//#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>

#include "Entity_Classes.h"
#include "map_classes.h"
//#include "Standard_Vectorfunc.h"
#include "collision_self.h"


//#define ONE_DIV_SQRTWO 0.70706781f //float value of ONE_DIV_SQRTWO = 1/sqrt(2)



/*
______________________________________________________________________________
                                TODO LIST:

    2. Creat two list of shapes, one for rectangles and one for circles... Eatch shape needs to have a ID connected to a entity...
        the class or struct with shape should be in collision .h/.cpp files..
    3. Creat a function that sifts throu the two lists and resolvs collisions...
    4. implement mecanic where you can shoot "bullets" in the direction of mose
    5. make them bounce once and disaper the next time...

    Implement simplistic broad faze collision?
    2. make a function that divides a map in to chunks, Semi correct formuleratt...
    4. (State machine creator...)
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

void move_intersect(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt);

void move_entitys(const std::vector<Entity*>& moveb_enlist, float dt) {

    for (Entity* const n : moveb_enlist) // ska man använda const här? const entity n
        n->dirMove(dt);
}

sf::RectangleShape rect_at_point( sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
    sf::RectangleShape rect(size);
    rect.setPosition(pos);
    rect.setFillColor(color);
    return rect;

}

// Ska nog ändast behålla render_chunk
void render_Entitys(sf::RenderWindow& window, const std::vector<Entity*>& enlist) {
     
    for (const Entity* n : enlist) {
        if (!n->spr) { // n->spr == NULL samma som !n->spr
            window.draw(*(n->coli));
        }
        else {
            window.draw(*(n->spr));
        }
    }
}

// i need to use something like a z value to determen in what order to draw.
void render_chunk(sf::RenderWindow& window, const ChunkTest& chunk) {

    // varför har man get funktioner...
    //window.draw(*chunk.background);

    const std::vector<Entity>& enlist = chunk.stationary_enti;
    for (const Entity& n : enlist) {
        if (!n.spr)
            window.draw(*(n.coli));
        else
            window.draw(*(n.spr));
    }
}


// above uppdate state function and maby uppdate state function should probably be in map_classes.h/.cpp right?
void uppdate_state(sf::RenderWindow& window, const ChunkTest& chunk, bool new_chunk, const std::vector<Entity*>& fixed_enlist, std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*>   all_coliders, sf::Clock& clock) {

    sf::Time dt = clock.getElapsedTime(); // .asMilliseconds() direkt här ?

    // varför är "frameraten" alltid lite större
    //std::cout << 1.0f / dt.asSeconds() << "\n";


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

    clock.restart().asSeconds(); //clock->restart().asMilliseconds();

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

    std::vector<sf::Sprite>  all_sprites;
    all_sprites.reserve(20);
    std::vector<sf::Texture> all_textures;
    all_textures.reserve(20);
    // Borde ha två olika listor, en med circle shape och en för rectangle shapes... och resolva kollision genom att gå igenom dem alla...
    std::vector<sf::Shape*>  all_coliders;
    all_coliders.reserve(20);


    // skulle deta vara en lösning till resize problemet, Svar Nej
    // i teori är det lättar e att hålla koll på när resize sker och att då ändra pekaren då kanske
    //std::vector<spri_textur> all_sprites_textures;
    const float sprite_size_factor = 4.0;

    const char tex_piller_file_path[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/piller_head.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures); // magic number 4.0 that is the size factor should be clearer strong typing?
    sf::Sprite* piller_sprite = &all_sprites.back();
    
    // change rotation origin /center to the center of the Sprite, i proobaby need to change the colid shapes center to...
    piller_sprite->setOrigin((sf::Vector2f)piller_sprite->getTexture().getSize() / 2.f);
    
    sf::FloatRect colid_pill = piller_sprite->getGlobalBounds();
    Entity piller(piller_sprite);

    // one piller head is (15 * 4) in "pixel" size
    const float piller_pixel_size = 15 * sprite_size_factor;
    piller.moveEnt({ piller_pixel_size * 6 , piller_pixel_size * 4 }); // 8*2*4 = 16 => 4 pixels, 16 pixels, 64 = 17 pix,  8*8-4 = 8*2*4-4 = 16*4-4 = 15*4
    piller.rot_angle = 100;
    std::cout << piller.id << "\n";
    //piller.spr->setRotation(sf::degrees(45));
    
   
    std::vector<Entity*> fixed_enlist;//  = { &piller };
    //fixed_enlist.emplace_back(&piller);

    sf::CircleShape shape2(25.f);

    // this might make one extra copy...
    const char tex1[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/player.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* s = &all_sprites.back();// &all_sprites.back();

    // changes the origin that we rotate around Could temporarly change the origin and rotate right?
    s->setOrigin((sf::Vector2f)s->getTexture().getSize() / 2.f);
    Player playerOne(&shape2); //  Player playerOne(&shape2, s);
    std::cout << playerOne.id << "\n";

    std::vector<Entity*> moveb_enlist = { &playerOne }; // we are invoking the copy constructor here on &playerOne? and it's 
    moveb_enlist.push_back(&piller);
    //moveb_enlist.emplace_back(&playerOne);


    // all_textures.resize(2); // tydligt visande att resize på all_textures är vad som stör all_sprites 

    /*
    * const char tex2[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/Map_test1.png";
    make_sprite(tex2, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* chunk_sprite = &all_sprites.back(); //*all_sprites.end();
    
    */

    std::string map_path = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/chuncks";
    int map_size = 9;
    ChunkTest ch = ChunkTest(map_path, map_size, sprite_size_factor, all_sprites, all_textures);

    playerOne.speed = 200.0f;
    playerOne.rot_angle = 0.0f;

    /*
    // why does this make an error does moveb_enlist[0] not index the playerOne and
    std::vector<Entity> moveb_enlist = { playerOne };
    Player& plone = &(moveb_enlist[0]);

    */
    //Player* plone = dynamic_cast<Player*>(moveb_enlist.front());

    // Inte bra med magic numbers...
    bool new_chunk = true;

    sf::FloatRect bounds = ch.background->getGlobalBounds();

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
        uppdate_state(window, ch, new_chunk, fixed_enlist, moveb_enlist, all_coliders, clock);
        
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
            if(n->coli != nullptr)
                if (n->speed > 0 && circle_rect_collision(n->coli, m->spr, test_vec)) { //simple_rect_collision(n->spr, m->spr, test_vec)
                    if (test_vec == sf::Vector2f{ 0,0 })
                        continue;
                    /*
                    std::cout << "self_made collide" << "\n";
                    std::cout << "testvec "; print_SF2Dvec(test_vec);
                    std::cout << "testvec normalized"; print_SF2Dvec(test_vec.normalized());
                    std::cout << "pos:ncolider: "; print_SF2Dvec(n->spr->getPosition());
                    std::cout << "pos:mcolide: "; print_SF2Dvec(m->spr->getPosition());
                    
                    */
                   
                    Rect_Vertecies vet = get_vertecis_of_rectcol(m->spr);
       
                    // n->moveEnt(test.componentWiseMul(test2) );
                    //sf::Vector2f newvec = { clamp(test_vec.x,vet.vertecis[down_left].x,vet.vertecis[down_right].x),clamp(test_vec.x,vet.vertecis[down_left].y,vet.vertecis[top_left].y) };
                    
                    // theres is a bugg where  test_vec / 2.0f gets stuck left rotated side...
                    n->moveEnt(test_vec); 
                    //m->spr->move(-test_vec / 2.0f);


                    //n->dirMove(dt); // * n->speed   * dt,, * test_vec.normalized()
                    //m->spr->move((n->speed * dt) * n->dirV);
                    //m->spr->move( ( n->speed * dt  ) * -test_vec.normalized());

                    //std::cout << "new_pos:ncolider: "; print_SF2Dvec(n->spr->getPosition());
                    collision = true;
                }
                //sf::RectangleShape test;
                //test.getGlobalBounds(); 
                
            // semi race condition som sker i collison om man byter direction och är inne i functionen aka innan other_dir sätts rätt så  kan fel vector nyttjas
            // verkar främst ske när jag trycker ned två knappar sammtidigt i olika ricktningar
            /* 
            if (current_colider_rect.findIntersection(tmp_colider_rect)) {
                collision = true;
                std::cout << "colison was made" << "\n";
                std::cout << "n->dirV_before:" << " { " << n->dirV.x << " , " << n->dirV.y << " } " << "\n";
                sf::Vector2f other_dirtw = { other_dir.x * test, other_dir.y * test };
                std::cout << "n->dirV_after:" << " { " << n->dirV.x << " , " << n->dirV.y << " } " << "\n";
                std::cout << "other_dir:" << " { " << other_dir.x << " , " << other_dir.y << " } " << "\n";
                n->give_dir_Move(other_dir, dt);
                test -= 0.01f;
            }
            */
        }

        if (!collision) {
           //n->dirMove(dt); // när jag använder denna så blir de knas
        }
    }
}





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
