#pragma once
#include<iostream>
#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "Standard_Vectorfunc.h"

struct Rect_Vertecies {
    sf::Vector2f vertecis[4] = {};
};

//sf::Vector2f* get_vertecis_of_rectcol(sf::Sprite* colid_sprite);

Rect_Vertecies get_vertecis_of_rectcol(sf::Sprite* colid_sprite);

sf::Vector2f* normals_of_rect_withFunk(sf::Sprite* colid_sprite);

const sf::Vector2f min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies& R_V);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis);

bool intersect_rect(sf::Sprite* rect1, sf::Sprite* rect2);

const sf::Vector2f simple_min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies R_V);


bool intersect_nonRot_rect(sf::Sprite* rect1, sf::Sprite* rect2);

bool colid_Rotated_rectangles(sf::Sprite* rect1, sf::Sprite* rect2);