#pragma once
#include<iostream>
#include<limits>
#include <tuple>

#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "collision_self.h"

/*

Kommentar:
Kanske ska seöver att nyttja std::array


// returning raw pointers is not advisable by CHAT so maby we don't, instead we return structs or vectors

En Idè är att jag beräknar en swept collision för tanksen som rör sig, detta innebär att alla tansk collisions boxar
behöver förlängas i sinn hastighets riktning innan de chechas med SAT collisions algoritmen, 
där efter för flyttas objectet upp till den närmsta punkten av den koliderade objectet.
Hur hanterar jag tid aka vet när/om de skulle ha missat varandra... (vet inte...)

Eller om den swept obb'n intersectar med två stycken framför sig men den ena e framför den andra? 

*/

Rect_Vertecies get_vertecis_of_rectcol(sf::Sprite* colid_sprite) {

    //sf::Vector2f vertecis[4] = {};
    Rect_Vertecies R_V;
    sf::FloatRect localRect = colid_sprite->getLocalBounds();
    sf::Transform transformMatrix = colid_sprite->getTransform();
    sf::Vector2f position = localRect.position;//.getPosition();

    
    R_V.vertecis[top_left] = { position.x, position.y };
    R_V.vertecis[down_left] = { position.x , position.y + localRect.size.y }; // y direction is inverted
    R_V.vertecis[top_right] = { position.x + localRect.size.x, position.y };
    R_V.vertecis[down_right] = { position.x + localRect.size.x , position.y + localRect.size.y };
    
    for (sf::Vector2f& n : R_V.vertecis) {
        n = transformMatrix * n;
    }
    return R_V;
}

sf::Vector2f* normals_of_rect_withFunk(sf::Sprite* colid_sprite, Rect_Vertecies& Rect_verts) {

    // Function could be generalized to all 
    sf::Vector2f normals[2] = {};

    sf::Vector2f* verteces = Rect_verts.vertecis;
    normals[x_axis] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::top_right]);
    normals[y_axis] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::down_left]);

    return normals;
}

sf::Vector2f* normals_of_rect_withFunk(sf::Sprite* colid_sprite) {

    // Function could be generalized to all 
    sf::Vector2f normals[2] = {};

    sf::Vector2f* verteces = get_vertecis_of_rectcol(colid_sprite).vertecis;
    normals[x_axis] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::top_right]);
    normals[y_axis] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::down_left]);

    return normals;
}


sf::Vector2f* all_normals_of_rect(sf::Sprite* colid_sprite) {

    // Function could be generalized to all 
    enum Projection_normals { y_axis_up, y_axis_down, x_axis_right, x_axis_left };
    // Function could be generalized to all 
    sf::Vector2f normals[4] = {};

    sf::Vector2f* verteces = get_vertecis_of_rectcol(colid_sprite).vertecis;
    // tekniskt särr x_axis_up (0,1) och x_axis_down (0,-1)
    normals[y_axis_up] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::top_right]);
    normals[y_axis_down] = calc_normal_of_lineSegment(verteces[Vertex::down_right], verteces[Vertex::down_left]);

    normals[x_axis_right] = calc_normal_of_lineSegment(verteces[Vertex::top_left], verteces[Vertex::down_left]);
    normals[x_axis_left] = calc_normal_of_lineSegment(verteces[Vertex::down_right], verteces[Vertex::top_right]);

    return normals;
}

/*
std::array<sf::Vector2f,2> test_normals_of_rect_withFunk(sf::Sprite* colid_sprite) {

    // Function could be generalized to all 
    std::array<sf::Vector2f, 2> normals = {};

    sf::Vector2f* verteces = get_vertecis_of_rectcol(colid_sprite).vertecis;
    normals[x_axis] = calc_normal_of_lineSegment(verteces[top_left], verteces[top_right]);
    normals[y_axis] = calc_normal_of_lineSegment(verteces[top_right], verteces[down_right]);

    return normals;
}
*/

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

const sf::Vector2f min_max_projection_distance(const sf::Vector2f& projection_axis,
    const Rect_Vertecies& R_V, sf::Vector2f& min_dist_vec, sf::Vector2f& max_dist_vec) {

    int min_vec_index = 0;
    int max_vec_index = 1;
    float min_distance = R_V.vertecis[0].dot(projection_axis);
    float max_distance = R_V.vertecis[1].dot(projection_axis);
    float tmp_distance{};

    if (min_distance > max_distance) {
        tmp_distance = min_distance;
        min_distance = max_distance;
        max_distance = tmp_distance;
        min_vec_index = 1;
        max_vec_index = 0;
    }

    for (int i = 2; i < 4; i++) {
        tmp_distance = R_V.vertecis[i].dot(projection_axis);

        if (min_distance > tmp_distance) {
            min_distance = tmp_distance;
            min_vec_index = i;
        }

        if (max_distance < tmp_distance) {
            max_distance = tmp_distance;
            max_vec_index = i;
        }
    }

    min_dist_vec = R_V.vertecis[min_vec_index];
    max_dist_vec = R_V.vertecis[max_vec_index];

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
        return true;
    }

    // IF C < A AND D > A (Overlap in order object 2 -> object 1)
    if (C <= A && D >= A) { // original (C <= A && D >= A)
        // Store the collison data
        return true;
    }

    return false;

}

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis,
    float& size_of_overlap,
    sf::Vector2f& contact_normal) {

    sf::Vector2f min1_vec{};
    sf::Vector2f max1_vec{};

    sf::Vector2f min_max_dist1 = min_max_projection_distance(projection_axis, rect1_vertecis, min1_vec, max1_vec);
    //sf::Vector2f min_max_dist1 = min_max_projection_distance(projection_axis, rect1_vertecis);
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


    float current_size_of_overlap = 0;
    // IF A < C AND B > C (Overlap in order object 1 -> object 2)
    if (A <= C && B >= C) { // original (A <= C && B >= C)
        // Store the collison data

        current_size_of_overlap = C - B;

        if (abs(current_size_of_overlap) < abs(size_of_overlap)) { // this abs uses cstdlib
            size_of_overlap =  current_size_of_overlap;
            contact_normal = projection_axis;

        }

        return true;
    }

    // IF C < A AND D > A (Overlap in order object 2 -> object 1)
    if (C <= A && D >= A) { // original (C <= A && D >= A)
        // Store the collison data
        current_size_of_overlap = A - D;

        // should use abs or multiply?
        if (abs(current_size_of_overlap) < abs(size_of_overlap)) {
            size_of_overlap = current_size_of_overlap;
            contact_normal = -projection_axis;
        }

        return true;
    }

    return false;

}

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
    const float* circle_vert_pos,
    const Rect_Vertecies rect2_vertecis,
    float& size_of_overlap,
    sf::Vector2f& contact_normal) {

   
    // borde kunna göra dot product på centern till project axeln och sedan addera/ subtrahera radien..
    /*
    sf::Vector2f min_max_dist1 = { circle_vertecis.vertecis[0].dot(projection_axis),
                                   circle_vertecis.vertecis[1].dot(projection_axis) };
    
    */

    sf::Vector2f min_max_dist2 = min_max_projection_distance(projection_axis, rect2_vertecis);

    float A = circle_vert_pos[0];
    float B = circle_vert_pos[1];

    float tmp = 0;
    if (A>B) { // swap
        tmp = A;
        A = B;
        B = tmp;
    }

    float C = min_max_dist2.x;
    float D = min_max_dist2.y;

    float current_size_of_overlap = 0;
    // IF A < C AND B > C (Overlap in order object 1 -> object 2)
    if (A <= C && B >= C) { // original (A <= C && B >= C)
        // Store the collison data

        current_size_of_overlap = C - B;

        if (abs(current_size_of_overlap) < abs(size_of_overlap)) { // this abs uses cstdlib
            size_of_overlap = current_size_of_overlap;
            contact_normal = projection_axis;

        }

        return true;
    }

    // IF C < A AND D > A (Overlap in order object 2 -> object 1)
    if (C <= A && D >= A) { // original (C <= A && D >= A)
        // Store the collison data
        current_size_of_overlap = A - D;

        // should use abs or multiply?
        if (abs(current_size_of_overlap) < abs(size_of_overlap)) {
            size_of_overlap = current_size_of_overlap;
            contact_normal = -projection_axis;
        }

        return true;
    }

    return false;

}

bool check_SAT_axis_overlap( sf::Vector2f& projection_axis,
    const Rect_Vertecies rect1_vertecis,
    const Rect_Vertecies rect2_vertecis,
    float& size_of_overlap,
    sf::Vector2f& contact_normal,
    float& center_distance,
    sf::Vector2f& rect1_center,
    sf::Vector2f& min_veritise_along_normal,
    sf::Vector2f& max_veritise_along_normal) {

    sf::Vector2f min1_vec{};
    sf::Vector2f max1_vec{};

    sf::Vector2f min_max_dist1 = min_max_projection_distance(projection_axis, rect1_vertecis, min1_vec, max1_vec);
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

    float current_size_of_overlap;
    sf::Vector2f test1 = {};

    // IF A < C AND B > C (Overlap in order object 1 -> object 2)
    if (A <= C && B >= C) { // original (A <= C && B >= C)
        
        // Store the collison data
        current_size_of_overlap = C - B;

        if (current_size_of_overlap < size_of_overlap) {
            size_of_overlap = current_size_of_overlap;
            contact_normal = projection_axis;
            
            rect1_center =  max1_vec + contact_normal * current_size_of_overlap - max1_vec ; // calculate the contact point
            min_veritise_along_normal = min1_vec;
            max_veritise_along_normal = max1_vec;
            }
         // std::cout << "projection_axis:" << "{" << projection_axis.x << ", " << projection_axis.y << "}" << "\n";
        return true;
    }

    // IF C < A AND D > A (Overlap in order object 2 -> object 1)
    if (C <= A && D >= A) { // original (C <= A && D >= A)
        // Store the collison data

        current_size_of_overlap = A - D; // varför A - D här...

        // behöver hitta sidorna närmast till normalen

        if (current_size_of_overlap < size_of_overlap) {
            size_of_overlap = current_size_of_overlap;
            contact_normal = -projection_axis;

            rect1_center =  min1_vec + contact_normal * current_size_of_overlap - min1_vec; // (the vector with A as dor product) + contact_normal * current_size_of_overlap;    
            min_veritise_along_normal = min1_vec;
            max_veritise_along_normal = max1_vec;
        }
        return true;
    }

    return false;

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


bool intersect_rect(sf::Sprite* rect1, sf::Sprite* rect2) {

    sf::Vector2f* normals_rect1 = normals_of_rect_withFunk(rect1);
    sf::Vector2f normals_1[2] = { normals_rect1[0], normals_rect1[1] }; // { { 1.0f, 0.0f }, { 0.0f, 1.0f } };
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

bool simple_rect_collision(sf::Sprite* rect1, sf::Sprite* rect2, sf::Vector2f& respons_vector) {

    
    Rect_Vertecies vertecis_rect1 = get_vertecis_of_rectcol(rect1);
    sf::Vector2f* normals_rect1 = normals_of_rect_withFunk(rect1, vertecis_rect1);
    sf::Vector2f normals_1[2] = { normals_rect1[0], normals_rect1[1] }; // { { 1.0f, 0.0f }, { 0.0f, 1.0f } };

    Rect_Vertecies vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    sf::Vector2f* normals_rect2 = normals_of_rect_withFunk(rect2, vertecis_rect2); // should probably return a std::array<type, size>
    sf::Vector2f normals_2[2] = { normals_rect2[0], normals_rect2[1] };

    float min_axis_overlap = std::numeric_limits<float>::max();
    sf::Vector2f contact_normal = {};

    int numb_of_rect1_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {
        if (!check_SAT_axis_overlap(normals_1[i], vertecis_rect1, vertecis_rect2, min_axis_overlap, contact_normal) ||
            !check_SAT_axis_overlap(normals_2[i], vertecis_rect1, vertecis_rect2, min_axis_overlap, contact_normal))
            return false;
    }

    /*
    we should not need to get the perpendicular vector to the contact_normal
    
    */
    respons_vector = contact_normal * min_axis_overlap;
    //respons_vector = { respons_vector.x, -respons_vector.y };
    return true;
}

void get_clamped_vertex(Vertex_pair& Incident_Face, Vertex_pair& Reference_Face) {

    float In_x0 = Incident_Face.vertecis[0].x;
    float In_y0 = Incident_Face.vertecis[0].y;

    float In_x1 = Incident_Face.vertecis[1].x;
    float In_y1 = Incident_Face.vertecis[1].y;

    float Re_x0 = Reference_Face.vertecis[0].x;
    float Re_y0 = Reference_Face.vertecis[0].y;

    float k = (In_x1 - In_x0) / (In_y1 - In_y0); // (x1-x0)/(y1-y0) = k
    float m = (k * In_x0) - In_y0;  // m = k * x0 - y0

    float y_inc_clamped = k * Re_x0 + m; // y_clampted = kx0 + m

    // Kolla nu om y_inc_clamped finns på linjen.. kan inte vara mindre än min eller större än max
    if (!(y_inc_clamped < Re_y0 || y_inc_clamped > Re_y0)) {
        // uppdate to new clamped vertex
        In_x0 = Re_x0;
        In_y0 = y_inc_clamped;
    }
    Incident_Face.vertecis[0] = { In_x0 ,In_y0 };


    float Re_x1 = Reference_Face.vertecis[1].x;
    float Re_y1 = Reference_Face.vertecis[1].y;

    y_inc_clamped = k * Re_x1+ m;

    if (!(y_inc_clamped < Re_y1 || y_inc_clamped > Re_y1)) {
        // uppdate to new clamped vertex
        In_x1 = Re_x0;
        In_y1 = y_inc_clamped;
    }

    Incident_Face.vertecis[1] = { In_x1 ,In_y1 };
}

// can i minnimize the copying of values but allso keep the readabuility
sf::Vector2f get_clamped_vertex(float In_x0, float In_y0, float In_x1, float In_y1, float Re_x, float Re_y) {

    float k = (In_x1 - In_x0) / (In_y1 - In_y0); // (x1-x0)/(y1-y0) = k
    float m = (k * In_x0) - In_y0;  // m = k * x0 - y0

    float y_inc_clamped = k * Re_x + m; // y_clampted = kx0 + m

    // Kolla nu om y_inc_clamped finns på linjen.. kan inte vara mindre än min eller större än max
    if (!(y_inc_clamped < Re_y || y_inc_clamped > Re_y)) {
        // uppdate to new clamped vertex
        In_x0 = Re_x;
        In_y0 = y_inc_clamped;
    }
    return { In_x0,  In_y0 };
}

bool colid_Rotated_rectangles(sf::Sprite* rect1, sf::Sprite* rect2, sf::Vector2f& respons_vector) {

    sf::Vector2f* normals_rect1 = all_normals_of_rect(rect1);
    sf::Vector2f normals_1[4] = { normals_rect1[0], normals_rect1[1],  normals_rect1[2], normals_rect1[3] }; // { { 1.0f, 0.0f }, { 0.0f, 1.0f } };
    Rect_Vertecies vertecis_rect1 = get_vertecis_of_rectcol(rect1);


    sf::Vector2f* normals_rect2 = all_normals_of_rect(rect2); // should probably return a std::array<type, size>
    sf::Vector2f normals_2[4] = { normals_rect1[0], normals_rect1[1], normals_rect1[2], normals_rect1[3] };
    Rect_Vertecies vertecis_rect2 = get_vertecis_of_rectcol(rect2);

    float min_axis_overlap = 0;
    sf::Vector2f contact_normal = {};
    float start_distance = rect1->getPosition().x * rect1->getPosition().y;

    /*

    When two polytopes are colliding, the separating-axis test can also assist in computing
    contact information. Instead of exiting early when an overlap is detected on
    an axis, all axes are tested for overlap. After all axes have been tested, the axis with
    the least (normalized) overlap can be used as the contact normal,and the overlap can
    be used to estimate the penetration depth. With some extra work,contact points can
    also be computed with the help of the separating axis.

     Identifying the Signi cant Faces
     This is accomplished by selecting
     the vertex furthest along the collision normal.
    */


    //  Identifying the Signi cant Faces:
    sf::Vector2f furthest_vertice_rect1 = {}; // colider...
    sf::Vector2f closest_vertice_rect1 = {};

    sf::Vector2f furthest_vertice_rect2 = {};
    sf::Vector2f closest_vertice_rect2 = {};

    sf::Vector2f center_rect1 = {};//rect1->getTransform() * rect1->getOrigin();

    int numb_of_rect1_normals = 4;
    for (int i = 0; i < numb_of_rect1_normals; i++) {


        if (!check_SAT_axis_overlap(normals_1[i], vertecis_rect1, vertecis_rect2, min_axis_overlap, contact_normal, start_distance, center_rect1, closest_vertice_rect1,furthest_vertice_rect1))
            return false;
    }
    //  Identifying the Signicant_Faces_vertex_pair1:
    sf::Vector2f Signicant_Faces_vertex_pair1[2] = { furthest_vertice_rect1,closest_vertice_rect1 };

    for (int i = 0; i < numb_of_rect1_normals; i++) {


        if (!check_SAT_axis_overlap(normals_2[i], vertecis_rect1, vertecis_rect2, min_axis_overlap, contact_normal, start_distance, center_rect1, closest_vertice_rect2, furthest_vertice_rect2))
            return false;
    }
    sf::Vector2f Signicant_Faces_vertex_pair2[2] = { furthest_vertice_rect2,closest_vertice_rect2 };

    sf::Vector2f collision_normal = { -contact_normal.y, contact_normal.x };  //{ contact_normal.y, -contact_normal.x };
    
    sf::Vector2 rect1_normal = calc_normal_of_lineSegment(Signicant_Faces_vertex_pair1[0], 
                                                          Signicant_Faces_vertex_pair1[1]);

    sf::Vector2 rect2_normal = calc_normal_of_lineSegment(Signicant_Faces_vertex_pair2[0],
                                                          Signicant_Faces_vertex_pair2[1]);

    sf::Vector2f Reference_Face[2] = {};
    sf::Vector2f Incident_Face[2] = {};

    if (rect1_normal.dot(collision_normal) > rect2_normal.dot(collision_normal)) {
        Reference_Face[0] = Signicant_Faces_vertex_pair1[0]; 
        Reference_Face[1] = Signicant_Faces_vertex_pair1[1];

        Incident_Face[0] = Signicant_Faces_vertex_pair2[0]; 
        Incident_Face[1] = Signicant_Faces_vertex_pair2[1];
    }
    else {
        Reference_Face[0] = Signicant_Faces_vertex_pair2[0]; 
        Reference_Face[1] = Signicant_Faces_vertex_pair2[1];

        Incident_Face[0] = Signicant_Faces_vertex_pair1[0]; 
        Incident_Face[1] = Signicant_Faces_vertex_pair1[1];
    }
    
    Vertex_pair Incident = { Incident_Face[0],Incident_Face[1] };
    Vertex_pair Reference = { Reference_Face[0],Reference_Face[1] };
    get_clamped_vertex(Incident, Reference);

    /*
    Incident_Face[0] = get_clamped_vertex(Incident_Face[0].x, Incident_Face[0].y, Incident_Face[1].x, Incident_Face[1].y, Reference_Face[0].x, Reference_Face[0].y );

    Incident_Face[1] = get_clamped_vertex(Incident_Face[0].x, Incident_Face[0].y, Incident_Face[1].x, Incident_Face[1].y, Reference_Face[1].x, Reference_Face[1].y);
    */

    /*
    
    To do this we compute which of the two signi cant faces have a normal that is closest to parallel
    with that of the collision normal. Consider Figure 4. In this case the normal of the face indicated by
    a blue line is closest to parallel and, as such, that face becomes the reference face.

    */




    // vet här nu vad contact normalen och axis overlap är så testa att använda?

    //min_axis_overlap /= 10;
    respons_vector = contact_normal;// { contact_normal.y, -contact_normal.x };
    respons_vector = { contact_normal.y, -contact_normal.x }; // { respons_vector.y, respons_vector.x };
    //respons_vector *= min_axis_overlap;
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


bool minVertex_overlap_to_circle(sf::Vector2f& circle_center, Rect_Vertecies& rect2_vertecis, 
                                 float radia, float& min_axis_overlap, sf::Vector2f& contact_normal) {
    

    sf::Vector2f closest_vertex{};
    float minnfloatVal = std::numeric_limits<float>::max();
    float minDist{};

    for(const sf::Vector2f n: rect2_vertecis.vertecis){
        float dist_to_center = distance_between_points(n, circle_center);
        if (minnfloatVal > dist_to_center) {
            minnfloatVal = dist_to_center;
            closest_vertex = n;
            minDist = dist_to_center;
        }
    }

    float overlap = minDist - radia; // if negative then overlap
    bool is_overlapping = 0 > (overlap);

    if (is_overlapping && abs(min_axis_overlap) > abs(overlap)) {
        contact_normal = closest_vertex - circle_center;
        min_axis_overlap = overlap;
        return true; // return is_overlapping;
    }

    return is_overlapping; // might overlap but not be the smallest right?
    
}

sf::Vector2f closest_polyVertex_to_circle(sf::Vector2f& circle_center, Rect_Vertecies& rect2_vertecis) {

    sf::Vector2f closest_vertex{};
    float minnfloatVal = std::numeric_limits<float>::max();
    for (const sf::Vector2f n : rect2_vertecis.vertecis) {
        float dist_to_center = distance_between_points(n, circle_center);
        if (minnfloatVal > dist_to_center) {
            minnfloatVal = dist_to_center;
            closest_vertex = n;
        }
    }
    return closest_vertex;
}

// i need to check so that center is correct... SVAR JA
// float radius = x_y_axesVal.y / 2.0f; 
bool circle_rect_collision(sf::Shape* circle1, sf::Sprite* rect2, sf::Vector2f& respons_vector) {

    sf::Vector2f circle1_ceter = circle1->getTransform() * circle1->getGeometricCenter(); // might just need .getOrigin() if centrum correctly set...
    sf::Vector2f rect2_ceter = rect2->getTransform() * rect2->getOrigin();

    sf::FloatRect circle1_globalRect = circle1->getGlobalBounds();
    sf::Vector2f x_y_axesVal = circle1_globalRect.size;
    float radius = x_y_axesVal.y / 2.0f;

    Rect_Vertecies vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    sf::Vector2f* normals_rect2 = normals_of_rect_withFunk(rect2, vertecis_rect2); // should probably return a std::array<type, size>
    sf::Vector2f normals_rect2test2[2]{ normals_rect2[0], normals_rect2[1] };

    // varför "korrupteras" normals_rect2 efter vi passerar vertecis_rect2 till closest_polyVertex_to_circle functionen?
    sf::Vector2f closest_vertex = closest_polyVertex_to_circle(circle1_ceter, vertecis_rect2);
    sf::Vector2f circle_normal = (closest_vertex - circle1_ceter).normalized();
   
    sf::Vector2f normals[3] = { normals_rect2test2[0], normals_rect2test2[1], circle_normal };

    float min_axis_overlap = std::numeric_limits<float>::max();
    sf::Vector2f contact_normal = {};

    int numb_of_normals = 3;
    for (int i = 0; i < numb_of_normals; i++) {

        float circle_pos_on_axis = circle1_ceter.dot(normals[i]);
        float circle_vert_pos[2] = { circle_pos_on_axis - radius, circle_pos_on_axis + radius };
        if (!check_SAT_axis_overlap(normals[i], circle_vert_pos, vertecis_rect2, min_axis_overlap, contact_normal) )
            return false;
    }

    respons_vector = contact_normal * min_axis_overlap;

    return true;
}

bool intersect_circles(sf::CircleShape* circle1, sf::CircleShape* circle2) {

    sf::Vector2f center_cercle1 = circle1->getTransform() * circle1->getGeometricCenter();
    sf::Vector2f center_cercle2 = circle2->getTransform() * circle2->getGeometricCenter();

    float distance_between = distance_between_points(center_cercle1, center_cercle2);

    // returns if they collide but not where...
    return (distance_between <= circle1->getRadius() + circle2->getRadius());
}

// returns collisiondata or takes an collision input
bool collision_circles(sf::CircleShape& circle1, sf::CircleShape& circle2, CollisuinData& colid_data) {

    sf::Vector2f center_cercle1 = circle1.getTransform() * circle1.getGeometricCenter();
    sf::Vector2f center_cercle2 = circle2.getTransform() * circle2.getGeometricCenter();

    float distance_between = distance_between_points(center_cercle1, center_cercle2);
    sf::Vector2f Dir_vector_from_1to2 = (center_cercle1 - center_cercle2).normalized();

    float penetration = -(distance_between - circle1.getRadius() + circle2.getRadius());
    if (penetration > 0) {
        colid_data.normal = Dir_vector_from_1to2;
        colid_data.penetration = -penetration;
        colid_data.pointOnPlane = {}; // no point on a plane...
        return true;
    }

    return false;
}

/*
1. collision with circle and OBB
2. fix so that bullet bounces
3. change functions collisions with just shapes... Not sprites
4. colliding with different objects results in different Action class functions being called...
5. (what should be the different acction when two moving ojects collide?)

Nead to make a collision Action class.
*/

/*
// example of having one function for collision and a nother for the collision resolution 
void circles_collision_resolution(sf::CircleShape& circle1, sf::CircleShape& circle2, CollisuinData& colid_data) {

    sf::Vector2f center_cercle1 = circle1.getTransform() * circle1.getGeometricCenter();
    sf::Vector2f center_cercle2 = circle2.getTransform() * circle2.getGeometricCenter();

    float distance_between = distance_between_points(center_cercle1, center_cercle2);
    sf::Vector2f Dir_vector_from_1to2 = (center_cercle1 - center_cercle2).normalized();

    float penetration = -(distance_between - circle1.getRadius() + circle2.getRadius());
    if (penetration > 0) {
        colid_data.normal = Dir_vector_from_1to2;
        colid_data.penetration = -penetration;
        colid_data.pointOnPlane = {}; // no point on a plane...
        return true;
    }

    return false;
}
*/