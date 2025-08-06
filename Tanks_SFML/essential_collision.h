#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "Standard_Vectorfunc.h"

//maby classes..
struct RectShapeIdPair {
    const int entity_id;
    sf::RectangleShape shape;
};

struct CircleShapeIdPair {
    const int entity_id;
    sf::CircleShape shape;
};

struct CollisionResponseData {
    float penetration;
    sf::Vector2f contact_normal;

    sf::Vector2f respons() {
        return penetration * contact_normal;
    }
};


/*
------------------------------------------------------------------------------------------------------------------
Rectangle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

std::array<sf::Vector2f, 4> get_vertecis_of_rectcol(sf::RectangleShape& colid_sprite);

std::array<sf::Vector2f, 2> normals_of_rect_withFunk(const std::array< sf::Vector2f, 4>& rect_vertices);

const std::array<float, 2> min_max_projection_distance(const sf::Vector2f& projection_axis,
                                                       const std::array<sf::Vector2f, 4>& rect_vertices);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<sf::Vector2f, 4>& rect1_vertecis,
                            const std::array<sf::Vector2f, 4>& rect2_vertecis);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<sf::Vector2f, 4>& rect1_vertecis,
                            const std::array<sf::Vector2f, 4>& rect2_vertecis,
                            CollisionResponseData& respons_data);

bool intersect(sf::RectangleShape& rect1, sf::RectangleShape& rect2);
bool collision(sf::RectangleShape& rect1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector);

/*
------------------------------------------------------------------------------------------------------------------
Circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect(sf::CircleShape& circle1, sf::CircleShape& circle2);
bool collision(sf::CircleShape& circle1, sf::CircleShape& circle2, sf::Vector2f& respons_vector);

/*
------------------------------------------------------------------------------------------------------------------
Circle V Rect collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<float, 2>& circle_vert_pos,
                            const std::array<sf::Vector2f, 4>& rect2_vertecis);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<float, 2>& circle_vert_pos,
                            const std::array<sf::Vector2f, 4>& rect2_vertecis,
                            CollisionResponseData& respons_data);

sf::Vector2f closest_polyVertex_to_point(sf::Vector2f& point, std::array<sf::Vector2f, 4>& rect2_vertecis);

bool intersect(sf::CircleShape& circle1, sf::RectangleShape& rect2);
bool collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector);