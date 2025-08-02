#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>

// kan man enforca så att man måste skriva Vertex::top_left
enum Vertex { top_left, down_left, top_right, down_right };
enum Projection { x_axis, y_axis };

float vectf2_leng(sf::Vector2f vec);
float dot_product(const sf::Vector2f& vec1, const sf::Vector2f& vec2);
sf::Vector2f calc_normal_of_lineSegment(sf::Vector2f& starting_point, sf::Vector2f& end_point);
sf::Vector2f calc_normal_of_lineSegment(sf::Vector2f& starting_point, sf::Vector2f& end_point, bool direction);
float distance_between_points(const sf::Vector2f& point1, const sf::Vector2f& point2);
float squared_distance_between_points(const sf::Vector2f& point1, const sf::Vector2f& point2);
void print_SF2Dvec(sf::Vector2f vec);
float clamp(float input, float max, float min);
//float abs(float x);