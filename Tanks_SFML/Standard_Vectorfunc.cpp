#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "Standard_Vectorfunc.h"

// kan man enforca så att man måste skriva Vertex::top_left
// enum Vertex { top_left, down_left, top_right, down_right };
// enum Projection { x_axis, y_axis };

float vectf2_leng(sf::Vector2f vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float dot_product(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

sf::Vector2f calc_normal_of_lineSegment(sf::Vector2f& starting_point, sf::Vector2f& end_point) {

    sf::Vector2f dxy = end_point - starting_point;
    sf::Vector2f normal = { dxy.y, -dxy.x };
    //sf::Vector2f normal = { dxy.x , dxy.y }; // y is allready inverted

    // vectf2_leng({ dxy.y, -dxy.x } whant to make shure that the normal is a unit vector...
    return normal / vectf2_leng(normal); // should you be able to decide direction?
}

float distance_between_points(sf::Vector2f& point1, sf::Vector2f& point2) {
    return std::sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.x - point1.x));
}