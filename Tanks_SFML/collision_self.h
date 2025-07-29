#pragma once
#include<iostream>
#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "Standard_Vectorfunc.h"

struct Rect_Vertecies {
    sf::Vector2f vertecis[4] = {};
};

struct CollisuinData {
    float penetration;
    sf::Vector2f normal; // contact normal?
    sf::Vector2f pointOnPlane;
};

struct Vertex_pair{
    sf::Vector2f vertecis[2] = {};
};

//sf::Vector2f* get_vertecis_of_rectcol(sf::Sprite* colid_sprite);

Rect_Vertecies get_vertecis_of_rectcol(sf::Sprite* colid_sprite);

sf::Vector2f* normals_of_rect_withFunk(sf::Sprite* colid_sprite);

sf::Vector2f* all_normals_of_rect(sf::Sprite* colid_sprite);

const sf::Vector2f min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies& R_V);

const sf::Vector2f min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies& R_V, sf::Vector2f& min_dist_vec, sf::Vector2f& max_dist_vec);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis,
    float& size_of_overlap,
    sf::Vector2f& contact_normal);

bool check_SAT_axis_overlap(sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis,
    float& size_of_overlap,
    sf::Vector2f& contact_normal,
    float& center_distance,
    sf::Vector2f& rect1_center,
    sf::Vector2f& min_veritise_along_normal,
    sf::Vector2f& max_veritise_along_normal);

const sf::Vector2f simple_min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies R_V);

bool intersect_rect(sf::Sprite* rect1, sf::Sprite* rect2);
bool simple_rect_collision(sf::Sprite* rect1, sf::Sprite* rect2, sf::Vector2f& respons_vector);
void get_clamped_vertex(Vertex_pair& Incident_Face, Vertex_pair& Reference_Face);
sf::Vector2f get_clamped_vertex(float In_x0, float In_y0, float In_x1, float In_y1, float Re_x, float Re_y);

bool colid_Rotated_rectangles(sf::Sprite* rect1, sf::Sprite* rect2, sf::Vector2f& respons_vector);

bool circle_rect_collision(sf::Shape* circle1, sf::Sprite* rect2, sf::Vector2f& respons_vector); // sf::CircleShape& circle1

bool intersect_circles(sf::CircleShape* circle1, sf::CircleShape* circle2);

bool collision_circles(sf::CircleShape& circle1, sf::CircleShape& circle2, CollisuinData& colid_data);