#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include<filesystem>
#include"Entity_Classes.h"
#include"essential_collision.h"
/*
______________________________________________________________________________
                    Map and chunk classes in progress:
USE this to iterate over a map containing all the sprites for the chunks in a map.
#include <filesystem>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
...
for (const auto& dirEntry : recursive_directory_iterator(myPath))
     std::cout << dirEntry << std::endl;
______________________________________________________________________________
*/

/*

what should a shunk be able to do?

1. ska kunna placera ut positionellt alla entitys och då sätta up alla NPC'er och NPC data, om använd som start chunk ska spelaren placeras
2. ska rendera alla object innom sig
3. ska kunna resolva collisions innom sig
4. Ha en aktvie och inaktiv state, when aktiv ska collisions inte checkas
5. ha en function som innefattar en transition över till nästa chunk. möjligen rendrad redan.
----
hur kan dessa uppfyllas?

1. vi checkar vilka entitys som intersectar vilken chunk för att beräkna om entityn/collision shapen behöver byta chunk rellaterad vector.
   kan då också cacha vissa intersect beräkningar beroende på distansen de rörtsig från sinn start punkt och varje gång den byter chunk upp
   daterar vi denna startpunkt, med detta så vet vi att x chunk inte behövs testas
   (antingen detta eller så är de bara inte aktiva om ingen spelare är där).

2. 

*/

class Chunk {
    // chunk id? Or should it'sda position in the array/vector be
public:


    std::vector<Id_Pair<sf::RectangleShape>> rect_coliders{};
    std::vector<Id_Pair<sf::CircleShape>> circle_coliders{};


    sf::Sprite* background{};

    // when i have moved to composition version of player class then this std::vector<Entity*> changed to std::vector<Entity> 
    std::vector<Entity*> chunks_entitys{};

    // when chunks_entitys changes to std::vector<Entity> then the two vectors can be sett together
    std::vector<Entity> tmp_backgound_chunks_entitys{};
    


    // std::vector<Entity> chunks_entitys{};
    // std::vector<Player> chunks_players{};

    // not needed, we only need the size of the sprite?
    int x{};
    int y{};

    //sf::Texture tex;      // the texture

    /*
    should have a function that reads a file, could be of JSON format, and constructs entitys and there colliders.. 

    */


    // i will start with haveing the Sprite's and textures in main and include all...
    Chunk(std::string map_path, int sprite_num, float size_factor,
        std::vector<sf::Sprite>& all_sprites,
        std::vector<sf::Texture>& all_textures) {

        chunks_entitys.reserve(sprite_num);

        bool background_first = true;

        for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(map_path)) {
            std::string file_path = dirEntry.path().string();

            if (background_first) {
                background_first = false;
                background = make_spritetest(file_path, size_factor, all_sprites, all_textures);
            }
            else {
                Entity test = Entity(make_spritetest(file_path, size_factor, all_sprites, all_textures));
                tmp_backgound_chunks_entitys.emplace_back(test);
                //chunks_entitys.emplace_back(Entity(make_spritetest(file_path, size_factor, all_sprites, all_textures)));
            }
        }
    }

    void render_chunk(sf::RenderWindow& window) {

        window.draw(*background);

        for (const Entity* n : chunks_entitys) {

            if(n->spr)
                window.draw(*(n->spr));

            if (n->coli)
                window.draw(*(n->coli));
        }

        for (const Entity& n : tmp_backgound_chunks_entitys) {

            if (n.spr)
                window.draw(*(n.spr));

            if (n.coli)
                window.draw(*(n.coli));
        }
    }

    // write a move function first then include collisions later
    void move_transformables(float dt) { 
        // WE SHOULD MAKE IT So that the dirMove, RotEnt and set_direction is called on all relevant parties aka all sprites and colliders in chunk

        // here we should have a function that just setts the player's direction that is called before we move anny thing.

        for (Entity* n : chunks_entitys) { // ska man använda const här? const entity n

           /* if (n->dirV == sf::Vector2f{0,0})
                continue;
                */
            n->set_direction(); // should probably not be heare
            n->RotEnt(dt);
            n->dirMove(dt);
        }
    }

    void colider_move_ent_to_chunk(Entity* ent, const sf::RectangleShape&& rect_colider) {
        chunks_entitys.push_back(ent); // could use emplace_back? especially if changed to this std::vector<Entity>...
        ent->add_assosiate_vall_entity_id_to_vec(std::move(rect_colider), rect_coliders);
    }
    void colider_move_ent_to_chunk(Entity* ent, const sf::CircleShape&& circle_colider) {
        chunks_entitys.push_back(ent); // could use emplace_back? especially if changed to this std::vector<Entity>...
        ent->add_assosiate_vall_entity_id_to_vec(std::move(circle_colider), circle_coliders);
    }


    void add_ent_to_chunk(Entity* ent, Id_Pair<sf::RectangleShape> rect_colider) {
        chunks_entitys.push_back(ent);
        rect_coliders.push_back(rect_colider);
    }
    void add_ent_to_chunk(Entity* ent, Id_Pair<sf::CircleShape> circle_colider) {
        chunks_entitys.push_back(ent);
        circle_coliders.push_back(circle_colider);
    }

    void print_entity_ids() {
        // this function show that the constructor of entity is called extremly more times than it should be
        for (const Entity& n : tmp_backgound_chunks_entitys) {
            std::cout << "id:" << n.id << "\n" ;
        }

        for (const Entity* c_n : chunks_entitys) {
            std::cout << "id:" << c_n->id << "\n";
        }
    }
    /*
    BEHÖVS:

    1. dela upp forloops delar i egna functioner...
    2. gör om collisions till mer generiska 
    3. skapa en sweept collision detection för circlar
    4. testa så att collision  med två saker som rör på sig ser bra ut och inte eller behöver ha egna functioner eller annat?
    
    */
    
    /* 
    void resolve_collisions() {

        int numb_rect = rect_coliders.size();
        int numb_circles = circle_coliders.size();

       
        * if both are moving then we need to call an nother collision function????
        when should we not check collisio?

        i need to make a nother collision aka sweept circle collision or/and intersect.

        1. when the entity is not moving then we should not check
        2. when we find same colider we should not check (maby same entity_id?).
        3. 

      

        for (int i = 0; i < numb_rect; i++) {

            Id_Pair r1 = rect_coliders.at(i);
            int r1_id = r1.entity_id;
         
            Entity r_entity{}; // is speed set to zero in standard constructor? Can i be serten that logical gate is evaluated from left to right? answer YES
            if(find_entity_with_id(r1_id, chunks_entitys, r_entity))
                if(r_entity.speed == 0){ // same as if(r_entity.speed)
                    continue;
                }

            sf::RectangleShape r1_shape = r1.value;

            for (int j = 0; j < numb_rect; j++) {

                Id_Pair r2 = rect_coliders.at(j);
                int r2_id = r2.entity_id;

                // assumes that every entity has one collidior and can't collide with a nother with same entity_id
                if (r1_id == r2_id) {
                    continue;
                }

                sf::RectangleShape r2_shape = r2.value;
            }

            for (int j = 0; j < numb_rect; j++) {

                Id_Pair c2 = circle_coliders.at(j);
                int c2_id = c2.entity_id;

                // assumes that every entity has one collidior and can't collide with a nother with same entity_id
                // a circle id can be assumed to not be the same as a rectangles
                /*
                if (r1_id == c2_id) {
                    continue;
                }
           
                sf::CircleShape c2_shape = c2.value;
            }
        }


        for (int i = 0; i < numb_circles; i++) {
            circle_coliders.at(i);

        }
    }


    void resolve_Rect_coll(Id_Pair<sf::RectangleShape>&& r1){
        int numb_rect = rect_coliders.size();
        int numb_circles = circle_coliders.size();

        for (int i = 0; i < numb_rect; i++) {

            Id_Pair r1 = rect_coliders.at(i);
            int r1_id = r1.entity_id;

            Entity r_entity{}; // is speed set to zero in standard constructor? Can i be serten that logical gate is evaluated from left to right? answer YES
            if (find_entity_with_id(r1_id, chunks_entitys, r_entity))
                if (r_entity.speed == 0) { // same as if(r_entity.speed)
                    continue;
                }

            sf::RectangleShape r1_shape = r1.value; 


            for (int j = 0; j < numb_rect; j++) {

                Id_Pair r2 = rect_coliders.at(j);
                int r2_id = r2.entity_id;

                // assumes that every entity has one collidior and can't collide with a nother with same entity_id
                if (r1_id == r2_id) {
                    continue;
                }

                sf::RectangleShape r2_shape = r2.value;

                sf::Vector2f respons_vector{};
                if (collision(r1_shape, r2_shape, respons_vector))
                    r_entity.moveEnt(respons_vector);
            }

            for (int j = 0; j < numb_rect; j++) {

                Id_Pair c2 = circle_coliders.at(j);
                int c2_id = c2.entity_id;

                // assumes that every entity has one collidior and can't collide with a nother with same entity_id
                // a circle id can be assumed to not be the same as a rectangles
                //if (r1_id == c2_id) {
                //    continue;
                //}
                
                sf::CircleShape c2_shape = c2.value;

                sf::Vector2f respons_vector{};
                if (collision(c2_shape, r1_shape, respons_vector))
                    r_entity.moveEnt(respons_vector);
            }
        }
    }
    */

    //private:

    //protected:

};


/*
sf::Image Class Reference
https://www.sfml-dev.org/documentation/3.0.0/classsf_1_1Image.html


Use this sf::Image::copy function to divide a large image in to chunks:
https://www.sfml-dev.org/documentation/3.0.0/classsf_1_1Image.html#a5399551f13bd86c9f1b2d96ad52812ca
bool sf::Image::copy	(	const Image &	source,
Vector2u	dest,
const IntRect &	sourceRect = {},
bool	applyAlpha = false )


deside where the rectangels are located with:
constexpr 	Rect (Vector2< T > position, Vector2< T > size)
    Construct the rectangle from position and size.
*/