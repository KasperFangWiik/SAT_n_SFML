#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include<filesystem>
#include"Entity_Classes.h"
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

class ChunkTest {
    // chunk id? Or should it'sda position in the array/vector be
public:
    sf::Sprite* background{};

    std::vector<Entity> stationary_enti{};
    std::vector<sf::Shape*> coliders{};        // colition- & hitbox

    // not needed, we only need the size of the sprite?
    int x{};
    int y{};

    //sf::Texture tex;      // the texture

    // C:\Users\User\Desktop\sprites\map1_pngs
    ChunkTest(std::string map_path, int sprite_num, float size_factor,
        std::vector<sf::Sprite>& all_sprites,
        std::vector<sf::Texture>& all_textures) {

        stationary_enti.reserve(sprite_num);

        bool back_first = true;

        for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(map_path)) {
            std::string test = dirEntry.path().string();

            if (back_first) {
                back_first = false;
                background = make_spritetest(test, size_factor, all_sprites, all_textures);
            }
            else {
                stationary_enti.emplace_back(Entity(make_spritetest(test, size_factor, all_sprites, all_textures)));
            }

        }

    }


    ChunkTest(sf::Sprite* b, std::vector<Entity> s_e, std::vector<sf::Shape*> c) {

        background = b;
        stationary_enti = s_e;
        coliders = c;
    }

    ChunkTest(sf::Sprite* b, std::vector<Entity> s_e) {

        background = b;
        stationary_enti = s_e;
    }


    ChunkTest(sf::Sprite* s) {
        background = s;
    }

    //private:

    //protected:

};

class Game_Map {

    //std::vector<ChunkTest> layout;// std::vector<std::vector<Chunk>>

    // if we don't aoutomaticly divide an image then we don't need this?
    int x_off{}; // map offset in x coordinates
    int y_off{}; // map offset in y coordinates

    /*
    What does the contructor nead to do?
    1. take a image and divide it in to chunks, depending on offset?
    2. store both colison and texture data...
    3. store enimy location

    // std::vector<ChunkTest> är problematisk, behöver möjligen definiera mer ty kanske en destructor?
    Game_Map(std::vector<ChunkTest> l) {
        layout = l;
    }
    */

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