
#include<iostream>
#include<filesystem>
#include <string>
#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>

#include "Entity_Classes.h"
#include "map_classes.h"
//#include "Standard_Vectorfunc.h"
#include "collision_self.h"




//#define ONE_DIV_SQRTWO 0.70706781f //float value of ONE_DIV_SQRTWO = 1/sqrt(2)


// skriv inte för mycket kod, testa med att få grund saker att funka 
// exemple, function som ritar rectangel, kunna flytta på den 
// flytta den sedan med inputs från keyboardet
// kolla upp hur man använder deltaTime / hur spel uppdaterar frames utan att bero på frame rate?


// varför använder man get och sett functioner å varför private??
// there is a difference between moving and non-moving entitys
// maby this should be a identity factory 

// https://en.cppreference.com/w/cpp/container/vector
//Reallocations are usually costly operations in terms of performance. 
//The reserve() function can be used to eliminate reallocations 
//if the number of elements is known beforehand.
// 
// do we whant an std::vector<entity>* or std::vector<shape>*?

//std::vector<sf::Sprite>::iterator
void move_with_Coll_entitys(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt);
void move_intersect(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt);

void move_entitys(const std::vector<Entity*>& moveb_enlist, float dt) {

    for (Entity* n : moveb_enlist) // ska man använda const här? const entity n
        n->dirMove(dt);
}


// Ska nog ändast behålla render_chunk
void render_Entitys(sf::RenderWindow& window, const std::vector<Entity*>& enlist) {

    for (Entity* n : enlist) {
        if (!n->spr) // n->spr == NULL samma som !n->spr
            window.draw(*(n->coli));
        else
            window.draw(*(n->spr));
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
    // i cant call reserve here because sf::Shape is not copyable; KOLLA detta fel/error igen för du förstod inte denna !!!!!!!!
    std::vector<sf::Shape*>  all_coliders;
    all_coliders.reserve(20);


    // skulle detta vara en lösning till resize problemet, Svar Nej
    // i teori är det lättar e att hålla koll på när resize sker och att då ändra pekaren då kanske
    //std::vector<spri_textur> all_sprites_textures;
    const float sprite_size_factor = 4.0;

    const char tex_piller_file_path[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/piller_head.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures); // magic number 4.0 that is the size factor should be clearer strong typing?
    sf::Sprite* piller_sprite = &all_sprites.back();
    sf::FloatRect colid_pill = piller_sprite->getGlobalBounds();
    Entity piller(piller_sprite);

    // one piller head is (15 * 4) in "pixel" size
    const float piller_pixel_size = 15 * sprite_size_factor;
    piller.moveEnt({ piller_pixel_size * 6 , piller_pixel_size * 4 }); // 8*2*4 = 16 => 4 pixels, 16 pixels, 64 = 17 pix,  8*8-4 = 8*2*4-4 = 16*4-4 = 15*4
    
    piller.spr->setRotation(sf::degrees(45));
    //piller.spr->rotate(sf::degrees(45));
    // calls copy constructor?
    std::vector<Entity*> fixed_enlist;//  = { &piller };
    //fixed_enlist.emplace_back(&piller);

    sf::CircleShape shape2(50.f);
    //shape2.setFillColor(sf::Color::Red);
    //shape2.move(200.f, 200.f);

    // this might make one extra copy...
    const char tex1[] = "C:/Users/HP/OneDrive/Skrivbord/SFML_prodject/sprites/player.png";
    make_sprite(tex_piller_file_path, sprite_size_factor, all_sprites, all_textures);
    sf::Sprite* s = &all_sprites.back();// &all_sprites.back();
    Player playerOne(&shape2, s);


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

class Entitytest1 {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:
    sf::Vector2<float> posV{}; // position in window
    sf::Vector2<float> dirV{}; // directinal vector
    float speed{};
    sf::Shape* coli{};        // colition- & hitbox.
    sf::Shape* texturebox{};  // shape for texture
    //sf::Texture tex;      // the texture

    bool w{}, a{}, s{}, d{};

    void moveEnt(sf::Vector2<float> v) {
        coli->move(v);
        texturebox->move(v);
    }

    virtual void dirMove() { // move entity in the direction of direction vector   
        moveEnt(dirV * speed); // scales the drection vector by the speed
    }

    //resulting acceleration vector for next time it uppdates? (how do i make quater frames?)

    Entitytest1() {}


    Entitytest1(sf::Shape* c) {
        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

    //should we pass "pointers/refferences" instead of just float?
    Entitytest1(float x, float y, sf::Shape* c) {
        posV.x = x, posV.y = y;

        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

    //private:

    //protected:

};

class Entitytest2 {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:
    sf::Vector2<float> posV{}; // position in window
    sf::Vector2<float> dirV{}; // directinal vector
    float speed{};
    sf::Shape* coli{};        // colition- & hitbox.
    sf::Shape* texturebox{};  // shape for texture
    //sf::Texture tex;      // the texture

    bool w{}, a{}, s{}, d{};

    void moveEnt(sf::Vector2<float> v) {
        coli->move(v);
        texturebox->move(v);
    }

    virtual void dirMove() { // move entity in the direction of direction vector   
        moveEnt(dirV * speed); // scales the drection vector by the speed
    }

    //resulting acceleration vector for next time it uppdates? (how do i make quater frames?)

    sf::Shape* form(sf::Shape* universalShape) {

        // hur tar man och skapar en variabel som alla object i klassen har till gång till
        // skapa ny textur så att vi kan använda en univerrsal shape istället för flera
        sf::Texture testTex;
        testTex.resize({ 100, 100 });

        const sf::Texture* nytextur = &testTex;

        universalShape->setTexture(nytextur, true);

        /*

        bool 	loadFromFile (const std::string &filename, const IntRect &area=IntRect())
            Load the texture from a file on disk

        bool create (unsigned int width, unsigned int height)
            Create the texture


        sf::Shape::setTexture	(	const Texture *	texture, bool	resetRect = false )
        texture	New texture
        resetRect	Should the texture rect be reset to the size of the new texture?

        */
        return universalShape;
    }

    Entitytest2() {}// standard constructor


    Entitytest2(sf::Shape* c) {
        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

    //should we pass "pointers/refferences" instead of just float?
    Entitytest2(float x, float y, sf::Shape* c) {
        posV.x = x, posV.y = y;

        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

private:
    static sf::Shape* universalShape; // const? och 

    //protected:

};

/*
______________________________________________________________________________
                                TODO LIST:
    1. Make a system for applying textures to shapes
    2. make a function that divides a map in to chunks
    3. make a system for checking collisons
    4. (State machine creator...)
______________________________________________________________________________

Questions:
Noah snackade något om hur man skulle  hantera texturer eller shapes...

std::vector nyttjar sig av RAII och deletar sig skälv efter gåt ut ur scope
och kallar på destructorn av objectet inom den men om denna destructor inte
kallar på delet kommer det skapa minnes luckor?

*/



/*
______________________________________________________________________________
                                Collisondetection:
   testing SAT algorithm, this is usfull links/sotces:
   https://dyn4j.org/2010/01/sat/
   https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics4collisiondetection/2017%20Tutorial%204%20-%20Collision%20Detection.pdf
______________________________________________________________________________

*/

// struct sortof taken from  pdf, contains all data that is neaded to resolv a collison..
struct CollisuinData {
    float penetration;
    sf::Vector2f normal;
    sf::Vector2f pointOnPlane;
};
/*
We note that while this information is suitable to resolve relatively simple collisions, more complex
collision resolution requires more information. This will be explored in more detail in the tutorial on
Manifolds.
*/



//-------------------------------------------------------------- Det över är mer gennerella func


/*

//The function should probably return a std::span aka a pointer and the length of the arr,  (C++20)
// (sf::Rect<float>* rect, sf::Transform transform_matrix)
sf::Vector2f* calculate_normals_of_rect(sf::Sprite* colid_sprite) {

    sf::FloatRect localRect = colid_sprite->getLocalBounds();
    sf::Transform transformMatrix = colid_sprite->getTransform();

    // behöver jag initializera på detta sätt?
    sf::Vector2f normals[2] = { sf::Vector2f{}, sf::Vector2f{} };

    // we transform the local boumds...
    sf::Vector2f starting_point = localRect.position; //transform_matrix * rect->getPosition();
    sf::Vector2f xy2_axis_side_x = { starting_point.x + localRect.size.x, starting_point.y };
    sf::Vector2f xy2_axis_side_y = { starting_point.x , starting_point.y + localRect.size.y };

    starting_point = transformMatrix * starting_point;
    xy2_axis_side_x = transformMatrix * xy2_axis_side_x;
    xy2_axis_side_y = transformMatrix * xy2_axis_side_y;

    normals[0] = calc_normal_of_lineSegment(starting_point, xy2_axis_side_x);
    normals[1] = calc_normal_of_lineSegment(starting_point, xy2_axis_side_y);

    return normals;
    // if we define dx=x2-x1 and dy=y2-y1, then the normals are (-dy, dx) and (dy, -dx)
}




//-------------------------
// SIMPLIFIED rectangle kollision with out rotation/transformation exemp size and directiom:

sf::Vector2f* simplefide_normals_of_rect(const sf::Rect<float>& rect) {

    // behöver jag initializera på detta sätt?
    sf::Vector2f normals[2] = { sf::Vector2f{}, sf::Vector2f{} };

    sf::Vector2f starting_point = rect.position;


    sf::Vector2f xy2_axis_side_x = { starting_point.x + rect.size.x, starting_point.y };
    normals[0] = calc_normal_of_lineSegment(starting_point, xy2_axis_side_x);


    sf::Vector2f xy2_axis_side_y = { starting_point.x, starting_point.y + rect.size.y };
    normals[1] = calc_normal_of_lineSegment(starting_point, xy2_axis_side_y);

    
    return normals;
    // if we define dx=x2-x1 and dy=y2-y1, then the normals are (-dy, dx) and (dy, -dx)
}

*/

bool colid_nonRotated_rectangles(sf::Sprite* rect1, sf::Sprite* rect2) {

    bool collison = true;
    sf::Vector2f* normals_rect1 = normals_of_rect_withFunk(rect1);
    sf::Vector2f* vertecis_rect1 = get_vertecis_of_rectcol(rect1).vertecis;

    sf::Vector2f* normals_rect2 = normals_of_rect_withFunk(rect2);
    sf::Vector2f* vertecis_rect2 = get_vertecis_of_rectcol(rect2).vertecis;

    //int numb_vertecis = sizeof(vertecis_rect1);

    // HÄR BORDE JAG BARA SE OM Värdena över är korrect!!

    //  distance = a * b_normal, a is the vertex vector

    std::vector<float> axis_distances_1 = {};
    std::vector<float> axis_distances_2 = {};
    //return check_SAT_axis_overlap() || check_SAT_axis_overlap();
    float Xmin_distance1 = dot_product(vertecis_rect1[Vertex::top_left], normals_rect1[Projection::x_axis]);
    float Xmax_distance1 = dot_product(vertecis_rect1[Vertex::top_right], normals_rect1[Projection::x_axis]);

    float Xmin_distance2 = dot_product(vertecis_rect2[Vertex::top_left], normals_rect2[Projection::x_axis]);
    float Xmax_distance2 = dot_product(vertecis_rect2[Vertex::top_right], normals_rect2[Projection::x_axis]);

    // if we find one not overlapping then return false aka we cant be intersecting


    dot_product(vertecis_rect1[top_left], normals_rect1[x_axis]);
    dot_product(vertecis_rect2[top_left], normals_rect2[top_left]);


    // get the all the points/vertexes of the rectangles and 
    return collison;
}

//-------------------------
// jag borde skillja på functionen som checkar kollisions och  movar?
void move_with_Coll_entitys(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt) {

    int i = 0;
    for (Entity* n : moveb_enlist) { // ska man använda const här? const entity n

        sf::Vector2f starting_point = n->spr->getPosition(); // should i just use get pos or possition values in entitiy?
        sf::Vector2f end_point = (starting_point)+(n->dirV) * (n->speed) * dt;
        sf::Vector2f vals = (n->dirV) * (n->speed) * dt;

        float fraction = 16.0; // maby the fraction value should depend on the length, then we would allways test the same number of times... no...
        sf::Vector2f end_point_fraction = { end_point.x / fraction, end_point.y / fraction };

        std::cout << "index:" << i++ << "\n";
        std::cout << "starting_point:" << " { " << starting_point.x << " , " << starting_point.y << " } " << "\n";
        std::cout << "end_point:" << " { " << end_point.x << " , " << end_point.y << " } " << "\n";
        std::cout << "end_point_fraction:" << " { " << end_point_fraction.x << " , " << end_point_fraction.y << " } " << "\n";
        std::cout << "vals:" << " { " << vals.x << " , " << vals.y << " } " << "\n";
        std::cout << "speed:" << " { " << n->speed << " } " << "\n"; // varför är alltid speed noll? 
        bool collision = false;

        // (i.x < end_point.x || i.y < end_point.y) funkar inte 
        for (sf::Vector2f i = starting_point; vectf2_leng(i) < vectf2_leng(end_point) || (i.x < end_point.x || i.y < end_point.y); i += end_point_fraction) { // this i += end_point_fraction floating point arithmetics might make it impossible to reatche one specific pont..

            sf::Rect<float> current_colider_rect = n->spr->getGlobalBounds(); //n->coli->getGlobalBounds();
            n->moveEnt(i);

            for (Entity* m : moveb_enlist) {
                // checking against others are made
                std::cout << "checking against others" << "\n";
                // i have to check so that we are not testing aoure selves...
                if (m == n) {
                    //std::cout << "colison was made" << "\n";
                    continue;
                }

                sf::Rect<float> tmp_colider_rect = m->spr->getGlobalBounds(); // m->coli->getGlobalBounds();

                // constexpr std::optional<Rect<T>> Rect<T>::findIntersection
                if (current_colider_rect.findIntersection(tmp_colider_rect) ) {
                    // normalt vill vi typ göra detta men vad om vill utföra en action som också kan bero på vilka två object som colidar?
                    // och vad händer om något movar på ett annat sätt typ teleportar skulle de då vara en action istället? eller ska move och action va kopplat?
                    n->moveEnt(i - end_point_fraction);
                    collision = true;
                    std::cout << "colison was made" << "\n";
                }
            }
        }
        /*
        if (!collision) {
            n->dirMove(dt);
        }
        */

    }
}

// semi funkar, ett problm är att jag inte vet hur mycket jag ska röra mig bakåt..
void move_intersect(const std::vector<Entity*>& moveb_enlist, std::vector<sf::Shape*> all_coliders, float pixel_size_factor, float dt) {


    for (Entity* n : moveb_enlist) { // ska man använda const här? const entity n
        bool collision = false;
        sf::Rect<float> current_colider_rect = n->spr->getGlobalBounds(); //n->coli->getGlobalBounds();

        n->set_direction();
        //n->dirMove(dt);
        float test = -1.0f;
        sf::Vector2f other_dir{ n->dirV.x * -1.3f, n->dirV.y * -1.3f };;
        for (const Entity* m : moveb_enlist) {

            // checkar bara att de har samma pekare/minnes adress.. behövs starkare eqvivalence?
            if (m == n) {
                //std::cout << "colison was made" << "\n";
                continue;
            }

            sf::Rect<float> tmp_colider_rect = m->spr->getGlobalBounds(); // m->coli->getGlobalBounds();
            //if (current_colider_rect.findIntersection(tmp_colider_rect))
                if (intersect_nonRot_rect(n->spr, m->spr)) {
                    std::cout << "self_made collide" << "\n";
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
            n->dirMove(dt);
        }
    }
}



/*
// BORDE Utgå från punkten och checka frammåt eller?
        for (sf::Vector2f i = end_point; vectf2_leng(i) <= vectf2_leng(starting_point); i -= ((n->dirV) * pixel_size_factor) ){ //  i teorin är 4 en pixel

            bool collision = false;
            sf::Rect<float> current_colider_rect = n->coli->getGlobalBounds();

            // i have to check so that we are not testing aoure selves...
            for (Entity* m : moveb_enlist) {

                sf::Rect<float> tmp_colider_rect = m->coli->getGlobalBounds();

                if (current_colider_rect.intersects(tmp_colider_rect)) {
                    // save the closest point from this object (maby give i that value - 1pixel)
                }

            }
        }

*/

/*
Cmake forwarding..

x är först i kön och testar sinn colison så om y som går efter rör sig sammtidigt emot x så kommer väll x putta y? NEJ
*/



// ------------------------------
// Movement functions/functionelity should be sepperate from entity, we should only be calling on move functions if the entity is expected to move..

/*
void moveEnt(sf::Vector2<float> v) {
    coli->move(v);
    texturebox->move(v);
}

virtual void dirMove(float dt) { // move entity in the direction of direction vector
    moveEnt(dirV * speed * dt); // scales the drection vector by the speed
}

*/
// ------------------------------


/*

class Movable_Entity: public Entity {

    //varför sätter folk pos i private och mutator functions i public?
    //should i use smart pointers hear?
public:

    sf::Vector2<float> dirV{}; // directinal vector
    float speed{};
    //sf::Texture tex;      // the texture

    Movable_Entity() {} // standard constructor
    ~Movable_Entity() {} // standard destructor


    Movable_Entity(sf::Shape* c) {
        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

    //should we pass "pointers/refferences" instead of just float?
    Movable_Entity(float x, float y, sf::Shape* c) {
        posV.x = x, posV.y = y;

        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }

    Movable_Entity(float x, float y, sf::Shape* c, float speed) {
        posV.x = x, posV.y = y;
        this->speed = speed;
        //passing one shape setts the colider to the texture box
        coli = texturebox = c;
    }


    void moveEnt(sf::Vector2<float> v) {
        coli->move(v);
        texturebox->move(v);
    }

    virtual void dirMove(float dt) { // move entity in the direction of direction vector
        moveEnt(dirV * speed * dt); // scales the drection vector by the speed
    }

    //private:

    //protected:

};



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
