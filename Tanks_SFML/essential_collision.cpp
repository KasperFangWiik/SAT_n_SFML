#pragma once
#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/System/Vector2.hpp>
#include "essential_collision.h"

/*

Kommentar:

TODOS?: 

2. colliding with different objects results in different Action class functions being called...
3. fix so that bullet bounces
4. (what should be the different acction when two moving ojects collide?)

Nead to make a collision Action class?

Useing the errorhandlning with:
std::array<T,N>::at 
Returns a reference to the element at specified location pos, with bounds checking.
If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
std::out_of_range

*/



/*
------------------------------------------------------------------------------------------------------------------
Pure SAT related:
------------------------------------------------------------------------------------------------------------------
*/

bool check_SAT_axis_overlap(const std::array<float, 2>& min_max_dist1,
                            const std::array<float, 2>& min_max_dist2) {

    const float min_vertex1 = min_max_dist1.at(0);
    const float max_vertex1 = min_max_dist1.at(1);

    const float min_vertex2 = min_max_dist2.at(0);
    const float max_vertex2 = min_max_dist2.at(1);

    return (min_vertex1 <= min_vertex2 && max_vertex1 >= min_vertex2) ||
           (min_vertex2 <= min_vertex1 && max_vertex2 >= min_vertex1);
}

/*
bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<float, 2>& min_max_dist1,
                            const std::array<float, 2>& min_max_dist2,
                            CollisionResponseData& respons_data) {

    float min_rect1 = min_max_dist1.at(0);
    float max_rect1 = min_max_dist1.at(1);

    float min_rect2 = min_max_dist2.at(0);
    float max_rect2 = min_max_dist2.at(1);

    float current_size_of_overlap = 0;

    // (Overlap in order object 1 -> object 2)
    if (min_rect1 <= min_rect2 && max_rect1 >= min_rect2) {

        current_size_of_overlap = min_rect2 - max_rect1;
        if (abs(current_size_of_overlap) < abs(respons_data.penetration)) { // abs from cstdlib, should use abs or multiply?
            respons_data.penetration = current_size_of_overlap;
            respons_data.contact_normal = projection_axis;
        }

        return true;
    }

    // (Overlap in order object 2 -> object 1)
    if (min_rect2 <= min_rect1 && max_rect2 >= min_rect1) {

        current_size_of_overlap = min_rect1 - max_rect2;
        if (abs(current_size_of_overlap) < abs(respons_data.penetration)) {
            respons_data.penetration = current_size_of_overlap;
            respons_data.contact_normal = -projection_axis;
        }

        return true;
    }

    return false;
}
*/


bool check_SAT_axis_overlap(
    const sf::Vector2f& projection_axis,
    const std::array<float, 2>& min_max_dist1,
    const std::array<float, 2>& min_max_dist2,
    CollisionResponseData& respons_data
) {

    const float min_vertex1 = min_max_dist1.at(0);
    const float max_vertex1 = min_max_dist1.at(1);

    const float min_vertex2 = min_max_dist2.at(0);
    const float max_vertex2 = min_max_dist2.at(1);

    //float current_size_of_overlap = 0.0f;

    // (Overlap in order object 1 -> object 2)
    const bool obj1_X_obj2 = min_vertex1 <= min_vertex2 && max_vertex1 >= min_vertex2;

    // (Overlap in order object 2 -> object 1)
    const bool obj2_X_obj1 = min_vertex2 <= min_vertex1 && max_vertex2 >= min_vertex1;

    if (obj1_X_obj2 || obj2_X_obj1) {

        float current_size_of_overlap = obj2_X_obj1 ? (min_vertex1 - max_vertex2) : (min_vertex2 - max_vertex1);
        if (std::abs(current_size_of_overlap) < std::abs(respons_data.penetration)) { // abs from cstdlib, should use abs or multiply?
            respons_data.penetration = current_size_of_overlap;
            respons_data.contact_normal = obj2_X_obj1 ? -projection_axis : projection_axis;
        }

        return true;
    }

    return false;
}

/*
------------------------------------------------------------------------------------------------------------------
Rectangle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

std::array<sf::Vector2f, 4> get_vertecis_of_rectcol(sf::RectangleShape& colid_sprite) {

    std::array< sf::Vector2f, 4> vertices = {};
    sf::FloatRect localRect = colid_sprite.getLocalBounds();
    sf::Transform transformMatrix = colid_sprite.getTransform();
    sf::Vector2f position = localRect.position;


    vertices.at(top_left) = { position.x, position.y };
    vertices.at(down_left) = { position.x , position.y + localRect.size.y }; // y direction is inverted
    vertices.at(top_right) = { position.x + localRect.size.x, position.y };
    vertices.at(down_right) = { position.x + localRect.size.x , position.y + localRect.size.y };

    for (sf::Vector2f& n : vertices) {
        n = transformMatrix * n;
    }
    return vertices;
}

std::array<sf::Vector2f, 2> normals_of_rect_withFunk(const std::array< sf::Vector2f,4>& rect_vertices) {

    // Function could be generalized to all 
    std::array<sf::Vector2f, 2> normals= {};

    normals.at(x_axis) = calc_normal_of_lineSegment(rect_vertices.at(Vertex::top_left), rect_vertices.at(Vertex::top_right));
    normals.at(y_axis) = calc_normal_of_lineSegment(rect_vertices.at(Vertex::top_left), rect_vertices.at(Vertex::down_left));

    return normals;
}

// could generalize this one and probably overload it whith different parameters
const std::array<float, 2> min_max_projection_distance(const sf::Vector2f& projection_axis,
                                                       const std::array<sf::Vector2f, 4>& rect_vertices) {

    float min_distance = rect_vertices.at(0).dot(projection_axis);
    float max_distance = rect_vertices.at(1).dot(projection_axis);

    if (min_distance > max_distance) {
        std::swap(min_distance, max_distance);
    }

    float tmp_distance = {};
    for (int i = 2; i < 4; i++) {
        tmp_distance = rect_vertices.at(i).dot(projection_axis);

        if (min_distance > tmp_distance) {
            min_distance = tmp_distance;
        }

        if (max_distance < tmp_distance) {
            max_distance = tmp_distance;
        }
    }

    return { min_distance, max_distance };
}


bool intersect(sf::RectangleShape& rect1, sf::RectangleShape& rect2) {

    std::array<sf::Vector2f, 4> vertecis_rect1 = get_vertecis_of_rectcol(rect1);
    std::array<sf::Vector2f, 2> normals_1 = normals_of_rect_withFunk(vertecis_rect1);

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    int numb_of_rect1_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {

        const std::array<float, 2> min_max_dist1 = min_max_projection_distance(normals_1[i], vertecis_rect1);
        const std::array<float, 2> min_max_dist2 = min_max_projection_distance(normals_2[i], vertecis_rect2);

        if (!check_SAT_axis_overlap(min_max_dist1, min_max_dist2) ||
            !check_SAT_axis_overlap(min_max_dist1, min_max_dist2))
            return false;
    }

    return true;
}
bool collision(sf::RectangleShape& rect1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector) {

    std::array<sf::Vector2f, 4> vertecis_rect1 = get_vertecis_of_rectcol(rect1);
    std::array<sf::Vector2f, 2> normals_1 = normals_of_rect_withFunk(vertecis_rect1);

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    CollisionResponseData respons_data = { std::numeric_limits<float>::max(), {} };

    int numb_of_rect1_normals = normals_1.size();
    for (int i = 0; i < numb_of_rect1_normals; i++) {

        const std::array<float, 2> min_max_dist1 = min_max_projection_distance(normals_1[i], vertecis_rect1);
        const std::array<float, 2> min_max_dist2 = min_max_projection_distance(normals_2[i], vertecis_rect2);

        if (!check_SAT_axis_overlap(normals_1[i], min_max_dist1, min_max_dist2, respons_data) ||
            !check_SAT_axis_overlap(normals_2[i], min_max_dist1, min_max_dist2, respons_data))
            return false;
    }
    respons_vector = respons_data.penetration * respons_data.contact_normal;
    return true;
}

/*
------------------------------------------------------------------------------------------------------------------
Circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect(sf::CircleShape& circle1, sf::CircleShape& circle2) {

    sf::Vector2f center_cercle1 = circle1.getTransform() * circle1.getOrigin();
    sf::Vector2f center_cercle2 = circle2.getTransform() * circle2.getOrigin();

    float distance_between = distance_between_points(center_cercle1, center_cercle2);

    return (distance_between <= circle1.getRadius() + circle2.getRadius());
}
bool collision(sf::CircleShape& circle1, sf::CircleShape& circle2, sf::Vector2f& respons_vector) {

    sf::Vector2f center_cercle1 = circle1.getTransform() * circle1.getGeometricCenter();
    sf::Vector2f center_cercle2 = circle2.getTransform() * circle2.getGeometricCenter();

    CollisionResponseData respons_data = {};

    sf::Vector2f Dir_vector_from_1to2 = (center_cercle1 - center_cercle2);
    float distance_between = distance_between_points(center_cercle1, center_cercle2);

    if (Dir_vector_from_1to2 != sf::Vector2f{ 0.0, 0.0 })
        Dir_vector_from_1to2 = Dir_vector_from_1to2.normalized();

    float penetration = -(distance_between - circle1.getRadius() + circle2.getRadius());
    if (penetration > 0) {
        respons_data.penetration = -penetration;
        respons_data.contact_normal = Dir_vector_from_1to2;
        respons_vector = respons_data.penetration * respons_data.contact_normal;
        return true;
    }
    return false;
}

/*
------------------------------------------------------------------------------------------------------------------
Circle V Rect collisions related:
------------------------------------------------------------------------------------------------------------------
*/

const std::array<float, 2> min_max_projection_distance(const sf::Vector2f& projection_axis, const sf::Vector2f& circle_center, const float radius) {

    float circle_pos_on_axis = circle_center.dot(projection_axis);

    float min_dist = circle_pos_on_axis - radius;
    float max_dist = circle_pos_on_axis + radius;

    if (min_dist > max_dist) {
        std::swap(min_dist, max_dist);
    }
    return { min_dist, max_dist };
}

// kan generalisera denna... Med std::vector
sf::Vector2f closest_polyVertex_to_point(sf::Vector2f& point, std::array<sf::Vector2f, 4>& rect2_vertices) {

    sf::Vector2f closest_vertex{};
    float minnfloatVal = std::numeric_limits<float>::max();
    for (const sf::Vector2f& n : rect2_vertices) {
        float dist_to_center = squared_distance_between_points(n, point);
        if (minnfloatVal > dist_to_center) {
            minnfloatVal = dist_to_center;
            closest_vertex = n;
        }
    }
    return closest_vertex;
}

bool intersect(sf::CircleShape& circle1, sf::RectangleShape& rect2) {

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });
    //sf::Vector2f circle1_ceter = circle1.getTransform() * circle1.getGeometricCenter();//circle1.getOrigin();


    sf::Vector2f offset_ori = rect2.getOrigin() + (rect2.getSize() / 2.f); // { ori.x + sizeer.x/2 ,ori.y + sizeer.y /2 };
    sf::Vector2f rect2_ceter = rect2.getTransform() * offset_ori; // n.value.getOrigin();//n.value.getSize() / 2.f;// rect2.getOrigin();


    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);


    sf::Vector2f closest_vertex = closest_polyVertex_to_point(circle1_ceter, vertecis_rect2);

    sf::Vector2f circle_normal{};
    sf::Vector2f distance_difference_vector = (closest_vertex - circle1_ceter);
    if (distance_difference_vector != sf::Vector2f{ 0.0, 0.0 })
        circle_normal = distance_difference_vector.normalized();
    else
        circle_normal = distance_difference_vector;


    std::array<sf::Vector2f, 3> normals = { normals_2.at(0), normals_2.at(1), circle_normal };

    for (const sf::Vector2f& normal : normals) {

        const std::array<float, 2> min_max_dist1 = min_max_projection_distance(normal, circle1_ceter, radius);
        const std::array<float, 2> min_max_dist2 = min_max_projection_distance(normal, vertecis_rect2);

        if (!check_SAT_axis_overlap(min_max_dist1, min_max_dist2))
            return false;
    }
    return true;
}
bool collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector) {

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });
    //sf::Vector2f circle1_ceter = circle1.getTransform() * circle1.getGeometricCenter();//circle1.getOrigin();


    sf::Vector2f offset_ori = rect2.getOrigin() + (rect2.getSize() / 2.f); // { ori.x + sizeer.x/2 ,ori.y + sizeer.y /2 };
    sf::Vector2f rect2_ceter = rect2.getTransform() * offset_ori; // n.value.getOrigin();//n.value.getSize() / 2.f;// rect2.getOrigin();
    //sf::Vector2f rect2_ceter   = rect2.getTransform() * rect2.getOrigin();


    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    sf::Vector2f closest_vertex = closest_polyVertex_to_point(circle1_ceter, vertecis_rect2);
    
    sf::Vector2f circle_normal{};
    sf::Vector2f distance_difference_vector = (closest_vertex - circle1_ceter);
    if (distance_difference_vector != sf::Vector2f{ 0.0, 0.0 })
        circle_normal = distance_difference_vector.normalized();
    else
        circle_normal = distance_difference_vector;

    std::array<sf::Vector2f, 3> normals = { normals_2.at(0), normals_2.at(1), circle_normal };

    CollisionResponseData respons_data = { std::numeric_limits<float>::max(), {} };

    for (const sf::Vector2f& normal : normals) {

        const std::array<float, 2> min_max_dist1 = min_max_projection_distance(normal, circle1_ceter, radius);
        const std::array<float, 2> min_max_dist2 = min_max_projection_distance(normal, vertecis_rect2);

        if (!check_SAT_axis_overlap(normal, min_max_dist1, min_max_dist2, respons_data))
            return false;
    }
    respons_vector = respons_data.penetration * respons_data.contact_normal;
    return true;
}

/*
------------------------------------------------------------------------------------------------------------------
Rect V Circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect(sf::RectangleShape& rect1, sf::CircleShape& circle2) {
    return intersect(circle2, rect1);
}
bool collision(sf::RectangleShape& rect1, sf::CircleShape& circle2, sf::Vector2f& respons_vector) {

    bool collide = collision(circle2, rect1, respons_vector);
    respons_vector *= -1.f;
    return collide;
}




//_________________________________________________________________________________________________________________________________________

/*
1. different actions happening when different objects collide or intersect
    How should i decided if we check for an collision or an intersection? Or what will happen when is happens?
    (make bullets bonce of walls)
2. collision between two moving objects. I have the Algorithm...
3. Reading a JSON File to creat a chunc and in future describe a map aka a collection of chunks
4. 

*/


/*
------------------------------------------------------------------------------------------------------------------
 Swept circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

/*

NY LÖSNING för swept circle collision:

testa en avlång rectangel i riktingen av circeln, sedan om båda två riktning och ortogonala axeln till riktingen overlapar

testa circel collision på närmaste vertexen aka min i min_max_projection_distance från båda axlarna fast projecserade på bara dirv axeln.
beroende på viken axel som är närmast längst directionen och krockar välj den respons vector + rör colidern/entityn tid.

*/

bool intersect_swept(SweptCircleShape& swept_circle1, sf::RectangleShape& rect2) {

    sf::CircleShape circle1 = swept_circle1.circle;
    sf::Vector2f dirV = swept_circle1.dirV;
    float speed = swept_circle1.speed;

    if (dirV != sf::Vector2f{ 0.0, 0.0 }) // dirV should allready be a normalized vector no need for normalization
        return intersect(circle1,rect2);

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);

    std::array<float, 2> min_max_dist1 = min_max_projection_distance(dirV, circle1_ceter,radius);//min_max_projection_distance(dirV, rect1_vertecis); // need distance
    min_max_dist1.at(1) += speed;

    std::array<float, 2> min_max_dist2 = min_max_projection_distance(dirV, vertecis_rect2);

    // maby i should make a constructior that does this from get go?
    CollisionResponseData respons_Data = { std::numeric_limits<float>::max(), {} }; // probably don't need respons data for intersection.

    // should check if just dirV axis overlap then we should check for collision at the overlap, so need to move collider(and entity?) to min overlap possition and check circle collision

    // find the normal vector to dirV
    sf::Vector2f min_dist_vertex2{};
    sf::Vector2f dirV_normal = { dirV.y, -dirV.x }; 
    std::array<float, 2> min_max_with1 = min_max_projection_distance(dirV_normal, circle1_ceter, radius); // this should get the fake rectangle projection distances
    std::array<float, 2> min_max_with2 = min_max_projection_distance(dirV_normal, vertecis_rect2);
    // I need to get the vertexes that hade the min overlap!!!

    if (!check_SAT_axis_overlap(min_max_dist1, min_max_dist2)  ||
        !check_SAT_axis_overlap(dirV_normal, min_max_with1, min_max_with2, respons_Data)){
        return false; // all need to overlap other wise no collision
    }


    /*
    what i need heare is the point in dirV direction that the "sweept rectangle" intersects the other normal rectangle
    and then check the intersection or collision for it.

    Lets say that testing all normals leeds to finding the exakt point of contact between the 
    */

    float min_overlap_vec2_float = min_dist_vertex2.dot(dirV);

    circle1.setPosition(circle1.getPosition() + dirV * min_overlap_vec2_float);
    if (intersect(circle1, rect2))
        return true;

    return false;

}

/*
dotproduct(otragonal projection on to line segment) then difference in radius but it's omore complicated when difference is large then line length
Look at ray marching function...

bool intersect_swept(SweptCircleShape& circle1, SweptCircleShape& circle2, sf::Vector2f dirV, float speed) {


    if (dirV != sf::Vector2f{ 0.0, 0.0 }) // dirV should allready be a normalized vector no need for normalization
        return intersect(circle1,rect2);

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);

    std::array<float, 2> min_max_dist1 = min_max_projection_distance(dirV, circle1_ceter,radius);//min_max_projection_distance(dirV, rect1_vertecis); // need distance
    min_max_dist1.at(1) += speed;

    std::array<float, 2> min_max_dist2 = min_max_projection_distance(dirV, vertecis_rect2);

    // maby i should make a constructior that does this from get go?
    CollisionResponseData respons_Data = { std::numeric_limits<float>::max(), {} };

    // should check if just dirV axis overlap then we should check for collision at the overlap, so need to move collider(and entity?) to min overlap possition and check circle collision

    // find the normal vector to dirV
    sf::Vector2f min_dist_vertex2{};
    sf::Vector2f dirV_normal = { dirV.y, -dirV.x };
    std::array<float, 2> min_max_with1 = min_max_projection_distance(dirV_normal, circle1_ceter, radius); // this should get the fake rectangle projection distances
    std::array<float, 2> min_max_with2 = min_max_projection_distance(dirV_normal, vertecis_rect2, min_dist_vertex2);
    // I need to get the vertexes that hade the min overlap!!!

    if (!check_SAT_axis_overlap(dirV, min_max_dist1, min_max_dist2, respons_Data)  ||
        !check_SAT_axis_overlap(dirV_normal, min_max_with1, min_max_with2, respons_Data)){
        return false; // all need to overlap other wise no collision
    }


    //sf::Vector2f closest_min_overlap_vec1{};
    float min_overlap_vec1_float = min_max_dist2.at(0); // closest point range

    //sf::Vector2f furdest_min_overlap_vec2{}; // this one from dirV_normal
    float min_overlap_vec2_float = min_dist_vertex2.dot(dirV);
    // check for intersection on both vertexes that had min overlap on both normals but

    if (min_overlap_vec2_float < min_overlap_vec1_float) {
        //std::swap(closest_min_overlap_vec1, furdest_min_overlap_vec2);
        std::swap(min_overlap_vec2_float, min_overlap_vec1_float);
    }

    circle1.setPosition(circle1.getPosition()+ dirV * min_overlap_vec1_float);
    if (intersect(circle1, rect2))
        return true;

    circle1.setPosition(circle1.getPosition() + dirV * min_overlap_vec2_float);
    if (intersect(circle1, rect2))
        return true;

    return false;



*/


/*

distance vector = dirV * speed * dt

in linjer motion:
d = t*( v2 + v1) / 2
we assume v1 = 0
v2 = v1 + at
a = (v2-v1)/dt

this works cause t = dt
v2 = t*(v2)/dt
d = t*(v2) / 2
t = dt =>

d = dt*(v2)/2
lets say v2 = dv... beacuse we are looking att the smallest possible interval


speed = (v2)/2 ot dv/2
// speed is here cainda equal to (v2-v1)/2 but because dt or in other words  the difference sample between v2 and v1 the smallest possible speed = dv/2
    void dirMove(float dt) { // move entity in the direction of direction vector
        moveEnt(dirV * speed * dt); // scales the drection vector by the speed
    }

*/

bool check_SAT_axis_overlap(const sf::Vector2f& projectionAxis,
                                const std::array<float, 2>& minMaxDist1,
                                const std::array<float, 2>& minMaxDist2,
                                const sf::Vector2f& velocityVec,
                                const float dt) {

    // assume for know that we first do this at t0

    const float min_rect1 = minMaxDist1.at(0);
    const float max_rect1 = minMaxDist1.at(1);

    const float min_rect2 = minMaxDist2.at(0);
    const float max_rect2 = minMaxDist2.at(1);

    // check collision at t0
    if ((min_rect1 <= min_rect2 && max_rect1 >= min_rect2) ||
        (min_rect2 <= min_rect1 && max_rect2 >= min_rect1))
        return true;

    // borde beräknas korrect va som änn händer
    float rect1_center = ((max_rect1 - min_rect1) / 2) + min_rect1;

    float velocityProjection = velocityVec.dot(projectionAxis);
    float distance_moved = velocityProjection * dt; // i can't add this to both object's then there would be no difference, i'm just looking for the time they are  last inter sect
    
    // calculate the direction of velocity
    float movedMinRect1Neg = min_rect1 - distance_moved;
    float movedMinRect1Pos = min_rect1 + distance_moved;

    // check for future overalp, aka overlap after moved. 
    return !(min_rect1 > max_rect2 && distance_moved > rect1_center && !(movedMinRect1Neg < max_rect2) ||
             min_rect1 < max_rect2 && distance_moved < rect1_center && !(movedMinRect1Pos > max_rect2));
}


bool check_SAT_axis_overlap(const sf::Vector2f& projectionAxis,
                            const std::array<float, 2>& minMaxDist1,
                            const std::array<float, 2>& minMaxDist2,
                            const sf::Vector2f& velocityVec, 
                            float& firstOverlapTime,
                            float& lastOverlapTime,
                            const float dt) {

    // assume for know that we first do this at t0

    bool overlap = false;

    const float min_rect1 = minMaxDist1.at(0);
    const float max_rect1 = minMaxDist1.at(1);
    
    // borde beräknas korrect va som änn händer
    float rect1_center = ((max_rect1 - min_rect1) / 2) + min_rect1;

    const float min_rect2 = minMaxDist2.at(0);
    const float max_rect2 = minMaxDist2.at(1);

    float  velocityProjection = velocityVec.dot(projectionAxis);
    float distance_moved = velocityProjection * dt; // i can't add this to both object's then there would be no difference, i'm just looking for the time they are  last inter sect

    if ((min_rect1 <= min_rect2 && max_rect1 >= min_rect2) ||
        (min_rect2 <= min_rect1 && max_rect2 >= min_rect1)) {
        overlap = true;
        firstOverlapTime = 0;//know we know that they first intersect at t0

        // how do i calculate the last time they overlaped... what if one passes the other then maby i just say that the last point was the others last
       
        // velocityVec is assumed to be (dirV2*speed2) - (dirV1*speed1), but could i just use (dirV2*speed2*dt) - (dirV1*speed1*dt) ? 
        // having allready multiplide with dt might or might not help with roundning errors becaous speed is to large right?

    }

    //  If they are separated and moving apart on any of the separating axes(or , in fact, on any axis whatever), then you know that there is no future collision.

    // calculate the direction of velocity
    float movedMinRect1Neg = distance_moved - min_rect1;
    float movedMinRect1Pos = distance_moved + min_rect1;

    // check if overal
    return !(min_rect1 > max_rect2 && distance_moved > rect1_center && !(movedMinRect1Neg < max_rect2) ||
             min_rect1 < max_rect2 && distance_moved < rect1_center && !(movedMinRect1Pos > max_rect2));
}

/*
Projecting each of the two bodies and the relative
velocity vector onto a particular separating axis at t0 gives two 1-D
intervals and a 1-D velocity

Behöver alltså testa intersection/collision innan move... Vilket för tillfället verkar få objecten att stutsa samman
Så får istället flytta "bak" punkterna på axlarna... Eller ändra på så att vi inte fixar collisionerna efteråt...
*/
bool intersect_of_moving_shapes(sf::RectangleShape& rect1, sf::RectangleShape& rect2, sf::Vector2f velosety_vec1, sf::Vector2f velosety_vec2) {

    sf::Vector2f velocity_vec = velosety_vec2 - velosety_vec1; // a velosety_vec1or2 = DirV*speed*dt

    std::array<sf::Vector2f, 4> vertecis_rect1 = get_vertecis_of_rectcol(rect1);
    std::array<sf::Vector2f, 2> normals_1 = normals_of_rect_withFunk(vertecis_rect1);

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    int numb_of_rect1_normals = 2;
    for (int i = 0; i < numb_of_rect1_normals; i++) {

        const std::array<float, 2> min_max_dist1 = min_max_projection_distance(normals_1[i], vertecis_rect1);
        const std::array<float, 2> min_max_dist2 = min_max_projection_distance(normals_2[i], vertecis_rect2);

        if (!check_SAT_axis_overlap(min_max_dist1, min_max_dist2) ||
            !check_SAT_axis_overlap(min_max_dist1, min_max_dist2))
            return false;
    }

    return true;
}



