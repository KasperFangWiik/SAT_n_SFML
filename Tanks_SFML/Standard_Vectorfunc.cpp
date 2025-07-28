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
    sf::Vector2f normal = { dxy.y, dxy.x };
    //sf::Vector2f normal = { dxy.x , dxy.y }; // y is allready inverted

    // we are now not normalizing the normal
    return normal / vectf2_leng(normal); // should you be able to decide direction? / vectf2_leng(normal)
}

sf::Vector2f calc_normal_of_lineSegment(sf::Vector2f& starting_point, sf::Vector2f& end_point, bool direction) {

    sf::Vector2f dxy = end_point - starting_point;
    sf::Vector2f normal{};
    // (-dy, dx) and (dy, -dx)
    if (direction) {
        normal = { dxy.y, -dxy.x };// y_coordinates are allready inverted
    }
    else {
        normal = { dxy.y , dxy.x }; // y is allready inverted
    }

    // we are now not normalizing the normal
    return normal / vectf2_leng(normal); // should you be able to decide direction? / vectf2_leng(normal)
}

float distance_between_points(sf::Vector2f& point1, sf::Vector2f& point2) {
    return std::sqrt((point2.x - point1.x) * (point2.x - point1.x) + (point2.y - point1.y) * (point2.x - point1.x));
}

void print_SF2Dvec(sf::Vector2f vec) {
    std::cout << "Vector_output" << "{" << vec.x << ", " << vec.y << "}" << "\n";
}

float clamp(float input, float max, float min) {

    if (input < min)
        input = min;
    if (input > max)
        input = max;
    return input;
}

/*
float abs(float x) {

    if (x < 0)
        return -x;
    else
        return x;
}

*/