#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include<concepts>

#include "Standard_Vectorfunc.h"
#include"Entity_remake.h" // concept IsCollideble is the only reason to include this, and i only use it in chunk for know... 

struct CollisionResponseData {
    float penetration;
    sf::Vector2f contact_normal;

    sf::Vector2f respons() const{ // const?
        return penetration * contact_normal;
    }
};

struct SweptCircleShape {
    sf::CircleShape& circle;
    sf::Vector2f dirV;
    float speed;
};

template<typename U>
concept IsCollideble = std::is_same_v< U, Id_Pair<sf::RectangleShape> > || std::is_same_v< U, Id_Pair<sf::CircleShape>>;



/*
------------------------------------------------------------------------------------------------------------------
Pure SAT related:
------------------------------------------------------------------------------------------------------------------
*/

bool check_SAT_axis_overlap(const std::array<float, 2>& min_max_dist1,
                            const std::array<float, 2>& min_max_dist2);

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<float, 2>& min_max_dist1,
                            const std::array<float, 2>& min_max_dist2,
                            CollisionResponseData& respons_data);
        
/*
------------------------------------------------------------------------------------------------------------------
Rectangle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

std::array<sf::Vector2f, 4> get_vertecis_of_rectcol(sf::RectangleShape& colid_sprite);

std::array<sf::Vector2f, 2> normals_of_rect_withFunk(const std::array< sf::Vector2f, 4>& rect_vertices);

const std::array<float, 2> min_max_projection_distance(const sf::Vector2f& projection_axis,
                                                       const std::array<sf::Vector2f, 4>& rect_vertices);

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
const std::array<float, 2> min_max_projection_distance(const sf::Vector2f& projection_axis, const sf::Vector2f& circle_center, const float radius);

sf::Vector2f closest_polyVertex_to_point(sf::Vector2f& point, std::array<sf::Vector2f, 4>& rect2_vertecis);

bool intersect(sf::CircleShape& circle1, sf::RectangleShape& rect2);
bool collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector);

/*
------------------------------------------------------------------------------------------------------------------
Rect V Circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect(sf::RectangleShape& rect1, sf::CircleShape& circle2);
bool collision(sf::RectangleShape& rect1, sf::CircleShape& circle2, sf::Vector2f& respons_vector);

/*
------------------------------------------------------------------------------------------------------------------
 Swept circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect_swept(SweptCircleShape& swept_circle1, sf::RectangleShape& rect2);