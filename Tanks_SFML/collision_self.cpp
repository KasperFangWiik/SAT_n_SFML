#pragma once
#include<iostream>
#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "collision_self.h"

/*

 Komment:
 It seams like the max and min function is not working korrectly for desiding whitch vertex is largest or smalest
 the simple min_max will only work for non rotated rectangles where we know witch vertexes that are right and 
 min or max, problem is probably the inverted y axis that SFML uses witch fucks with the sortning of dotproduct values..

 solution:
 use circles for bullets for know


// returning raw pointers is not advisable by CHAT so maby we don't, instead we return structs or vectors

struct Rect_Vertecies {
    sf::Vector2f top_left{};
    sf::Vector2f down_left{};
    sf::Vector2f top_right{};
    sf::Vector2f down_right{};
};

*/

Rect_Vertecies get_vertecis_of_rectcol(sf::Sprite* colid_sprite) {

    //sf::Vector2f vertecis[4] = {};
    Rect_Vertecies R_V;
    sf::FloatRect localRect = colid_sprite->getLocalBounds();
    sf::Transform transformMatrix = colid_sprite->getTransform();
    sf::Vector2f position = localRect.position;//.getPosition();

    
    R_V.vertecis[top_left] = position;
    R_V.vertecis[down_left] = { position.x , position.y + localRect.size.y }; // y direction is inverted
    R_V.vertecis[top_right] = { position.x + localRect.size.x, position.y };
    R_V.vertecis[down_right] = { position.x + localRect.size.x , position.y + localRect.size.y };
    
    for (sf::Vector2f& n : R_V.vertecis) {
        n = transformMatrix * n;
    }
    return R_V;
}

sf::Vector2f* normals_of_rect_withFunk(sf::Sprite* colid_sprite) {

    // Function could be generalized to all 
    sf::Vector2f normals[2] = {};

    sf::Vector2f* verteces = get_vertecis_of_rectcol(colid_sprite).vertecis;
    normals[x_axis] = calc_normal_of_lineSegment(verteces[top_left], verteces[top_right]);
    normals[y_axis] = calc_normal_of_lineSegment(verteces[top_right], verteces[down_right]);

    return normals;
}

const sf::Vector2f min_max_projection_distance(const sf::Vector2f& projection_axis,
                                               const Rect_Vertecies& R_V) {
    // dot_product
    float min_distance = R_V.vertecis[0].dot(projection_axis);
    float max_distance = R_V.vertecis[1].dot(projection_axis);
    float tmp_distance{};

    if (min_distance > max_distance) {
        tmp_distance = min_distance;
        min_distance = max_distance;
        max_distance = tmp_distance;
    }

    for (int i = 2; i < 4; i++) {
        tmp_distance = R_V.vertecis[i].dot(projection_axis);

        if (min_distance > tmp_distance) {
            min_distance = tmp_distance;
        }
        
        if (max_distance < tmp_distance) {
            max_distance = tmp_distance;
        }
    }

    return { min_distance, max_distance };
}

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const Rect_Vertecies rect1_vertecis,
                            const Rect_Vertecies rect2_vertecis) {

    sf::Vector2f min_max_dist1 = min_max_projection_distance(projection_axis, rect1_vertecis);
    sf::Vector2f min_max_dist2 = min_max_projection_distance(projection_axis, rect2_vertecis);


    float A = min_max_dist1.x;
    float B = min_max_dist1.y;
    
    float C = min_max_dist2.x;
    float D = min_max_dist2.y;

    //Overlap Test
    // Points go:
    //       +-------------+
    // +-----|-----+     2 |
    // | 1   |     |       |
    // |     +-----|-------+
    // +-----------+
    // A ----C---- B ----- D

    // how do i calculate the size of the overlap?
    // should probably
    // find reference face on A and a incident face on B with normals closesd to the projection_axis

    // IF A < C AND B > C (Overlap in order object 1 -> object 2)
    if (A <= C && B >= C) { // original (A <= C && B >= C)
        // Store the collison data
        
        float size_of_overlap = C - B;

        return true;
    }

    // IF C < A AND D > A (Overlap in order object 2 -> object 1)
    if (C <= A && D >= A) { // original (C <= A && D >= A)
        // Store the collison data

        float size_of_overlap = C - B;
        return true;
    }

    return false;

}


bool intersect_rect(sf::Sprite* rect1, sf::Sprite* rect2) {

    sf::Vector2f* normals_rect1 = normals_of_rect_withFunk(rect1);
    Rect_Vertecies vertecis_rect1 = get_vertecis_of_rectcol(rect1);

    sf::Vector2f* normals_rect2 = normals_of_rect_withFunk(rect2);
    Rect_Vertecies vertecis_rect2 = get_vertecis_of_rectcol(rect2);

    int numb_of_rect1_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {
        if (!check_SAT_axis_overlap(normals_rect1[i], vertecis_rect1, vertecis_rect2))
            return false;
    }

    // upprepar kod...
    int numb_of_rect2_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {
        if (!check_SAT_axis_overlap(normals_rect2[i], vertecis_rect2, vertecis_rect1))
            return false;
    }

    return true;
}


const sf::Vector2f simple_min_max_projection_distance(const sf::Vector2f& projection_axis,
                                                const Rect_Vertecies R_V) {

    
    if (projection_axis == sf::Vector2f{ 1.0f,0.0f }) {
        float min_distance = R_V.vertecis[top_left].dot(projection_axis);
        float max_distance = R_V.vertecis[top_right].dot(projection_axis);
        return { min_distance, max_distance };
    }
    else {
        float min_distance =  R_V.vertecis[top_right].dot(projection_axis);
        float max_distance = R_V.vertecis[down_right].dot(projection_axis);
        return { min_distance, max_distance };
    }

    return { -1, -1 };
}

bool colid_Rotated_rectangles(sf::Sprite* rect1, sf::Sprite* rect2) {
    
    sf::Vector2f* normals_rect1 = normals_of_rect_withFunk(rect1);
    sf::Vector2f normals_1[2] = { normals_rect1[0], normals_rect1[1]}; // { { 1.0f, 0.0f }, { 0.0f, 1.0f } };
    Rect_Vertecies vertecis_rect1 = get_vertecis_of_rectcol(rect1);


    sf::Vector2f* normals_rect2 = normals_of_rect_withFunk(rect2); // should probably return a std::array<type, size>
    sf::Vector2f normals_2[2] = { normals_rect1[0], normals_rect1[1] };
    Rect_Vertecies vertecis_rect2 = get_vertecis_of_rectcol(rect2);


    int numb_of_rect1_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {
        if (!check_SAT_axis_overlap(normals_1[i], vertecis_rect1, vertecis_rect2) ||
            !check_SAT_axis_overlap(normals_2[i], vertecis_rect1, vertecis_rect2))
                return false;
    }

    return true;
}


/*
----------------------------------------------------------------------------------------------
reddit post kanske lägger lite ljus på saken:
https://www.reddit.com/r/gamedev/comments/1k9dh2/how_to_use_the_separating_axis_theorem_for/


Values needed to calculate theas to deside the collision respons:

- The contact point P where the intersection has been detected

-  The contact normal N which represents the direction vector along which the intersecting object
   needs to move to resolve the collision.

- The penetration depth, i.e. the minimum distance along the normal that the intersecting object
  must move.

-------------------------

// capsule form:
sf::Vector2f Normal = normalize(startpoint – endpoint);
sf::Vector2f Line_thickness = a_Normal * a.radius;
sf::Vector2f end_circle = startpoint + Line_thickness;
sf::Vector2f start_circle = endpoint - Line_thickness;

// how do we 

calculate AABB betwee

----------------------------------------------------------------------------------------------
*/

bool intersect_circles(sf::CircleShape* circle1, sf::CircleShape* circle2) {

    sf::Vector2f center_cercle1 = circle1->getTransform() * circle1->getGeometricCenter();
    sf::Vector2f center_cercle2 = circle2->getTransform() * circle2->getGeometricCenter();

    float distance_between = distance_between_points(center_cercle1, center_cercle2);

    // returns if they collide but not where...
    return (distance_between == circle1->getRadius() + circle2->getRadius());
}