#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include<filesystem>


#include"Entity_remake.h"
#include"essential_collision.h"



/*

Bara ha en vector som innehåller vector med id_paris som representerar alla object som kan kollidera. Sedan skapa interfaces åt dem alla

*/


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

void drawpoint(sf::RenderWindow& window, sf::Vector2f& pos) {
    sf::RectangleShape test({ 5,5 });
    test.setFillColor(sf::Color(255, 0, 0));
    test.setPosition(pos);
    window.draw(test);
}

class Chunk {
    // chunk id? Or should it'sda position in the array/vector be
public:

    std::vector<Id_Pair<sf::ConvexShape>>    polygon_coliders{};
    std::vector<Id_Pair<sf::RectangleShape>> rect_coliders{};
    std::vector<Id_Pair<sf::CircleShape>>    circle_coliders{};


    int numb_backrund_sprite{};
    sf::Sprite* background{};

    // when i have moved to composition version of player class then this std::vector<Entity*> changed to std::vector<Entity> 
    std::vector<Entity> chunks_entitys{};

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

        //numb_backrund_sprite = sprite_num;

        chunks_entitys.reserve(sprite_num + sprite_num);
        //tmp_backgound_chunks_entitys.reserve(sprite_num);

        bool background_first = true;

        for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(map_path)) {
            std::string file_path = dirEntry.path().string();

            if (background_first) {
                background_first = false;
                background = make_spritetest(file_path, size_factor, all_sprites, all_textures);
            }
            else {
                // when the std::vector resizes then the copy constructor is called to move the objects to a new location 
                chunks_entitys.emplace_back(Entity(make_spritetest(file_path, size_factor, all_sprites, all_textures)));
                numb_backrund_sprite++;
            }
        }
    }


    // this function needs to be revisit to controll when entitys are renderd to simulate depth
    void render_chunk(sf::RenderWindow& window) {

        window.draw(*background);
        size_t tot_numb_entitys = chunks_entitys.size();

        for (size_t i = numb_backrund_sprite; i < tot_numb_entitys; i++) {
            window.draw(*(chunks_entitys.at(i).spr));
        }

        for (size_t i = 0; i < numb_backrund_sprite; i++) {
            window.draw(*(chunks_entitys.at(i).spr));
        }
        /*
        for (const Entity& n : chunks_entitys) {
            window.draw(*(n.spr));
        }
        */
    }

    // could i minimize this with templates?
    void render_chunk_coliders(sf::RenderWindow& window) {


        for (Id_Pair<sf::ConvexShape>& n : polygon_coliders) {
            window.draw(n.value);
        }

        std::array<sf::Vector2f, 4> vertecis_rect2{};
        for (Id_Pair<sf::RectangleShape>& n : rect_coliders) {
            window.draw(n.value);

 
            sf::Vector2f offset_ori = n.value.getOrigin() + (n.value.getSize() / 2.f); // { ori.x + sizeer.x/2 ,ori.y + sizeer.y /2 };
            sf::Vector2f rect2_ceter = n.value.getTransform() * offset_ori; // n.value.getOrigin();//n.value.getSize() / 2.f;// rect2.getOrigin();
            drawpoint(window,rect2_ceter);
            
            vertecis_rect2 = get_vertecis_of_rectcol(n.value);
           
        }

        for (const Id_Pair<sf::CircleShape>& n : circle_coliders) {
            window.draw(n.value);
            /*
            float radius = n.value.getRadius();
            sf::Vector2f circle1_ceter = n.value.getTransform() * (n.value.getOrigin() + sf::Vector2f{ radius,radius });//n.value.getOrigin(); n.value.getPosition();
            drawpoint(window, circle1_ceter);

            // sf::Vector2f& point, std::array<sf::Vector2f, 4>& rect2_vertices
            sf::Vector2f test_tv = closest_polyVertex_to_point(circle1_ceter, vertecis_rect2);
            drawpoint(window, test_tv);
            //sf::Vector2f testcircle1_ceter = circle1.getTransform() * circle1.getGeometricCenter();
            */
        }


    }

    void move_coliders_to_entity_pos(Entity&& e) {

        int polygon_col{};
        if (e.find_index_of_id_pair(polygon_coliders, polygon_col)) {
            e.apply_rot_n_pos(polygon_coliders.at(polygon_col).getvalue());
        }

        int rect_col{};
        if (e.find_index_of_id_pair(rect_coliders, rect_col)) {
            e.apply_rot_n_pos(rect_coliders.at(rect_col).getvalue());
        }

        int circle_col{};
        if (e.find_index_of_id_pair(circle_coliders, circle_col)) {
            e.apply_rot_n_pos(circle_coliders.at(circle_col).getvalue());
        }

    }
    // write a move function first then include collisions later
    void move_all_transformables(float dt) {
        // WE SHOULD MAKE IT So that the dirMove, RotEnt and set_direction is called on all relevant parties aka all sprites and colliders in chunk

        // here we should have a function that just setts the player's direction that is called before we move anny thing.

        for (Entity& e : chunks_entitys) { // ska man använda const här? const entity n
            
            // behöver nog splittra rörliga och icke rörliga object då minskas antallet sådana checks 
            if (e.speed == 0 && e.rot_angle == 0)
                 continue;

            e.RotEnt(dt);
            e.dirMove(dt);

            move_coliders_to_entity_pos(std::move(e));

            // n.apply_rot_n_pos(/*collider with right id*/);
            // check collision and resolv collision(how do i whant to distinguage if i whant to resolv or not resolv normaly the collision?)
        }
    }

    // should be able to add entitys without colider and add coliders without entity!!!
    void colider_move_ent_to_chunk(Entity& ent, sf::RectangleShape&& rect_colider) {
        chunks_entitys.push_back(ent); // could use emplace_back? especially if changed to this std::vector<Entity>...
        ent.add_assosiate_vall_entity_id_to_vec(std::move(rect_colider), rect_coliders);

        int rect_col{};
        ent.find_index_of_id_pair(rect_coliders, rect_col);
        ent.apply_rot_n_pos(rect_coliders.at(rect_col).getvalue());
 
    }
    void colider_move_ent_to_chunk(Entity& ent, sf::CircleShape&& circle_colider) {
        chunks_entitys.push_back(ent); // could use emplace_back? especially if changed to this std::vector<Entity>...
        ent.add_assosiate_vall_entity_id_to_vec(std::move(circle_colider), circle_coliders);

        int circle_col{};
        ent.find_index_of_id_pair(circle_coliders, circle_col);
        ent.apply_rot_n_pos(circle_coliders.at(circle_col).getvalue());
    }


    void colider_move_ent_to_chunk(Entity& ent, sf::ConvexShape&& polygon_colider) {
        chunks_entitys.push_back(ent); // could use emplace_back? especially if changed to this std::vector<Entity>...
        ent.add_assosiate_vall_entity_id_to_vec(std::move(polygon_colider), polygon_coliders);

        int poly_col{};
        ent.find_index_of_id_pair(polygon_coliders, poly_col);
        ent.apply_rot_n_pos(polygon_coliders.at(poly_col).getvalue());
    }

    void add_ent_to_chunk(Entity& ent, Id_Pair<sf::ConvexShape> polygon_colider) {
        chunks_entitys.push_back(ent);
        polygon_coliders.push_back(polygon_colider);

        int ploy_col{};
        ent.find_index_of_id_pair(polygon_coliders, ploy_col);
        ent.apply_rot_n_pos(polygon_coliders.at(ploy_col).getvalue());
    }

    void add_ent_to_chunk(Entity& ent, Id_Pair<sf::RectangleShape> rect_colider) {
        chunks_entitys.push_back(ent);
        rect_coliders.push_back(rect_colider);

        int rect_col{};
        ent.find_index_of_id_pair(rect_coliders, rect_col);
        ent.apply_rot_n_pos(rect_coliders.at(rect_col).getvalue());
    }
    void add_ent_to_chunk(Entity& ent, Id_Pair<sf::CircleShape> circle_colider) {
        chunks_entitys.push_back(ent);
        circle_coliders.push_back(circle_colider);

        int circle_col{};
        ent.find_index_of_id_pair(circle_coliders, circle_col);
        ent.apply_rot_n_pos(circle_coliders.at(circle_col).getvalue());
    }

    void print_entity_ids() {
        // this function show that the constructor of entity is called extremly more times than it should be
        for (const Entity& en : chunks_entitys) {
            std::cout << "id:" << en.id << "\n";
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
    std::vector<Id_Pair<sf::RectangleShape>> rect_coliders{};
    std::vector<Id_Pair<sf::CircleShape>> circle_coliders{};
    */

  
  // behöver inte vara en member function kanske till och med virtual function?
  template<IsCollideble T, IsCollideble U>
  void check_collision_with_shape_vector(Entity& e, Id_Pair<U>& collider, std::vector<Id_Pair<T>>& colliders) {

      sf::Vector2f respons_vector{};
      for (Id_Pair<T>& c : colliders) {

          if (compair_diff_id_pair(collider, c)) // don't test collision with same entity_id
              continue;

          if (collision(collider.value, c.value, respons_vector)) {
              e.moveEnt(respons_vector); // should i apply
              move_coliders_to_entity_pos(std::move(e));
          }

          /*
          if (intersect(collider, r_c.value)) {
              std::cout << "intersected" << "\n";
          }
          */
      }
  }

  template<IsCollideble U>
  void check_collision_with_shapes(Entity& e, std::vector<Id_Pair<U>>& colliders) {

      sf::Vector2f respons_vector{};
      //int col_index{};
      //Id_Pair<U> entitys_colider;
      //if (e.find_index_of_id_pair(colliders, col_index)) {

     if (auto* entitys_colider = e.find_n_return_id_pair(colliders)) {
 

          //U& entitys_colider = colliders.at(col_index).value;

          this->check_collision_with_shape_vector(e, *entitys_colider, polygon_coliders);

          this->check_collision_with_shape_vector(e, *entitys_colider, rect_coliders);
          this->check_collision_with_shape_vector(e, *entitys_colider, circle_coliders); // might not want to move e just take referance?

      }
  }
  
  /*
    // hur ser jag till att de aldrig har samma collider id, jo jag ökar alltid båda med ett och samma värde...
    template<IsCollideble U>
    void check_collision_with_shapesTetst(Entity&& e, std::vector<U>&& colliders) {

        sf::Vector2f respons_vector{};
        int col_index{};

        if (e.find_index_of_id_pair(colliders, col_index)) {



            //sf::RectangleShape* es_colider = rect_coliders.at(rect_col).getvalue();
            U& entitys_colider = colliders.at(col_index);
            // e.apply_rot_n_pos(rect_coliders.at(rect_col).getvalue());

            for (const Id_Pair<std::vector<U>>& c : colliders) {
                if (compair_diff_id_pair(entitys_colider, c)) // don't test collision with same entity_id
                    continue;
                // collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector)

                if (collision(entitys_colider.value, c.value, respons_vector)) {
                    e.moveEnt(respons_vector); // should i apply 
                    move_coliders_to_entity_pos(std::move(e));
                }
            }

            /*

            //sf::RectangleShape* es_colider = rect_coliders.at(rect_col).getvalue();
            U& entitys_colider = colliders.at(col_index);
            // e.apply_rot_n_pos(rect_coliders.at(rect_col).getvalue());

            for (const Id_Pair<sf::RectangleShape>& r_c : rect_coliders) {
                if (compair_diff_id_pair(entitys_colider,r_c)) // don't test collision with same entity_id
                    continue;
                // collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector)

                if (collision(entitys_colider.value, r_c.value, respons_vector)) {
                    e.moveEnt(respons_vector); // should i apply 
                    move_coliders_to_entity_pos(std::move(e));
                }

                
                if (intersect(entitys_colider.value, r_c.value)) {
                    std::cout << "collided with rectangle" << "\n";
                }
                
                
            }

            for (const Id_Pair<sf::CircleShape>& c_c : circle_coliders) {
                if (compair_diff_id_pair(entitys_colider, c_c)) // don't test collision with same entity_id
                    continue;


                if (collision(entitys_colider.value, c_c.value, respons_vector)) {
                    e.moveEnt(respons_vector); // should i apply
                    move_coliders_to_entity_pos(std::move(e));
                }

                
                if (intersect(entitys_colider.value, c_c.value)) {
                    std::cout << "collision with circle" << "\n";
                }
                
                
                
            }
            */
  /*
        }
    }
    */

    void resolve_collisions() {
    
        for (Entity& e : chunks_entitys) {
            if (e.speed == 0) // && e.rot_angle == 0
                continue;

            this->check_collision_with_shapes<sf::ConvexShape>(e, polygon_coliders);
            this->check_collision_with_shapes<sf::RectangleShape>(e, rect_coliders);

            // i should use swept collision if the object is moving...
            this->check_collision_with_shapes<sf::CircleShape>(e, circle_coliders);
            
        }
    }

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


/*
            // sf::RectangleShape and CircleShape has no appropriate defult constrors HAHAHAHAH
            Id_Pair<sf::RectangleShape> rect_col{ {},sf::RectangleShape(sf::Vector2f{}) };
            if (n.bolean_find_id_pair(rect_coliders, rect_col)) {
                n.apply_rot_n_pos(&rect_col.value);
            }

            Id_Pair<sf::CircleShape> circle_col{ {},sf::CircleShape( 0, 30) };;
            if (n.bolean_find_id_pair(circle_coliders, circle_col)) {
                n.apply_rot_n_pos(&circle_col.value);
            }

            if (sf::RectangleShape* rect_col = n.find_id_pair<sf::RectangleShape>(rect_coliders)) {
                n.apply_rot_n_pos(rect_col);
            }

            if (sf::CircleShape* circle_col = n.find_circleid_pair(circle_coliders)) {
                n.apply_rot_n_pos(circle_col);
            }



            sf::RectangleShape* rect_col{};
            if (n.bolean_find_id_pair(rect_coliders, rect_col)) {
                n.apply_rot_n_pos(rect_col);
            }

            sf::CircleShape* circle_col{};
            if (n.bolean_find_id_pair(circle_coliders, circle_col)) {
                n.apply_rot_n_pos(circle_col);
            }





            // no errors but doesn't work
            auto collider_ptr1 = n.find_id_pair(rect_coliders);
            auto collider_ptr2 = n.find_id_pair(circle_coliders);
            if (collider_ptr1) { // collider_ptr != nullptr
                n.apply_rot_n_pos(collider_ptr1);
            }

            if (collider_ptr2) { // collider_ptr != nullptr
                n.apply_rot_n_pos(collider_ptr2);
            }





            // assuming only one collider per entity, then the code above is correct
            auto collider_ptr = n.find_id_pair(circle_coliders);
            if (collider_ptr) { // collider_ptr != nullptr
                n.apply_rot_n_pos(collider_ptr);
            }
            */