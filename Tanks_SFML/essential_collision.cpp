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

bool check_SAT_axis_overlap(const sf::Vector2f& projection_axis,
                            const std::array<float, 2>& min_max_dist1,
                            const std::array<float, 2>& min_max_dist2) {

    const float min_rect1 = min_max_dist1.at(0);
    const float max_rect1 = min_max_dist1.at(1);

    const float min_rect2 = min_max_dist2.at(0);
    const float max_rect2 = min_max_dist2.at(1);

    return (min_rect1 <= min_rect2 && max_rect1 >= min_rect2) ||
        (min_rect2 <= min_rect1 && max_rect2 >= min_rect1);
}

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

        if (!check_SAT_axis_overlap(normals_1[i], min_max_dist1, min_max_dist2) ||
            !check_SAT_axis_overlap(normals_2[i], min_max_dist1, min_max_dist2))
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

    float distance_between = distance_between_points(center_cercle1, center_cercle2);
    sf::Vector2f Dir_vector_from_1to2 = (center_cercle1 - center_cercle2).normalized();

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

        if (!check_SAT_axis_overlap(normal, min_max_dist1, min_max_dist2))
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

/*
------------------------------------------------------------------------------------------------------------------
 Swept circle collisions related:
------------------------------------------------------------------------------------------------------------------
*/

bool intersect_swept(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f dirV, float speed) {


    if (dirV != sf::Vector2f{ 0.0, 0.0 }) // dirV should allready be a normalized vector no need for normalization
        return intersect(circle1,rect2);

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });

    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);

    std::array<float, 2> min_max_dist1 = min_max_projection_distance(dirV, circle1_ceter,radius);//min_max_projection_distance(dirV, rect1_vertecis); // need distance
    min_max_dist1.at(1) += speed;

    std::array<float, 2> min_max_dist2 = min_max_projection_distance(dirV, vertecis_rect2);

    // should check if just dirV axis overlap then we should check for collision at the overlap, so need to move collider(and entity?) to min overlap possition and check circle collision
    //if (!check_SAT_axis_overlap_ny(dirV, )
    return true;

}



/*
// functions started for a less expensive circle to rectangle collisions
sf::Vector2f closest_point_on_poly_to_circle(sf::Vector2f& center_point, std::array<sf::Vector2f, 4>& vertices) {

    enum Axeses {
        y_axis_up,
        y_axis_down,
        x_axis_right,
        x_axis_left
    };

    std::array<sf::Vector2f, 4> edg;
    edg[y_axis_up] = vertices[Vertex::top_right] - vertices[Vertex::top_left];
    edg[y_axis_down] = vertices[Vertex::down_right]- vertices[Vertex::down_left];

    edg[x_axis_right] = (vertices[Vertex::top_left] - vertices[Vertex::down_left]);
    edg[x_axis_left] = (vertices[Vertex::down_right] - vertices[Vertex::top_right]);


    sf::Vector2f closest_vertex{};
    float minnfloatVal = std::numeric_limits<float>::max();
    for (const sf::Vector2f& n : edg) {
        float dist_to_center = center_point.dot(n) ;//squared_distance_between_points(n, point);
        if (minnfloatVal > dist_to_center) {
            minnfloatVal = dist_to_center;
            closest_vertex = n;
        }
    }

    sf::Vector2f closest_point = (minnfloatVal / closest_vertex.lengthSquared()) * closest_vertex;
    float dist = (closest_point - center_point).length();

    return closest_point;
}


CollisionResponseData closest_Vecpoint_on_poly_to_circle(sf::Vector2f& center_point, std::array<sf::Vector2f, 4>& vertices) {

    enum Axeses {
        y_axis_up,
        y_axis_down,
        x_axis_right,
        x_axis_left
    };

    CollisionResponseData col_data{};
    std::array<sf::Vector2f, 4> edg;
    edg[y_axis_up] = vertices[Vertex::top_right] - vertices[Vertex::top_left];
    edg[y_axis_down] = vertices[Vertex::down_right] - vertices[Vertex::down_left];

    edg[x_axis_right] = (vertices[Vertex::top_left] - vertices[Vertex::down_left]);
    edg[x_axis_left] = (vertices[Vertex::down_right] - vertices[Vertex::top_right]);


    sf::Vector2f closest_vertex{};
    float minnfloatVal = std::numeric_limits<float>::max();
    for (const sf::Vector2f& n : edg) {
        float dist_to_center = center_point.dot(n);//squared_distance_between_points(n, point);
        if (minnfloatVal > dist_to_center) {
            minnfloatVal = dist_to_center;
            closest_vertex = n;
        }
    }

    sf::Vector2f closest_point = (minnfloatVal / closest_vertex.lengthSquared()) * closest_vertex;
    float dist = (closest_point - center_point).length();

    col_data.penetration = dist;
    col_data.contact_normal = closest_point;
    return col_data;
}


bool intersect(sf::CircleShape& circle1, sf::RectangleShape& rect2) {

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });
    //sf::Vector2f circle1_ceter = circle1.getTransform() * circle1.getGeometricCenter();//circle1.getOrigin();


    sf::Vector2f offset_ori = rect2.getOrigin() + (rect2.getSize() / 2.f); // { ori.x + sizeer.x/2 ,ori.y + sizeer.y /2 };
    sf::Vector2f rect2_ceter = rect2.getTransform() * offset_ori; // n.value.getOrigin();//n.value.getSize() / 2.f;// rect2.getOrigin();


    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    sf::Vector2f vec_to_closest_point = closest_point_on_poly_to_circle(circle1_ceter, vertecis_rect2);

    float dist = (vec_to_closest_point - circle1_ceter).length();

    return dist < radius;

}

bool collision(sf::CircleShape& circle1, sf::RectangleShape& rect2, sf::Vector2f& respons_vector) {

    float radius = circle1.getRadius();
    sf::Vector2f circle1_ceter = circle1.getTransform() * (circle1.getOrigin() + sf::Vector2f{ radius, radius });
    //sf::Vector2f circle1_ceter = circle1.getTransform() * circle1.getGeometricCenter();//circle1.getOrigin();


    sf::Vector2f offset_ori = rect2.getOrigin() + (rect2.getSize() / 2.f); // { ori.x + sizeer.x/2 ,ori.y + sizeer.y /2 };
    sf::Vector2f rect2_ceter = rect2.getTransform() * offset_ori; // n.value.getOrigin();//n.value.getSize() / 2.f;// rect2.getOrigin();


    std::array<sf::Vector2f, 4> vertecis_rect2 = get_vertecis_of_rectcol(rect2);
    std::array<sf::Vector2f, 2> normals_2 = normals_of_rect_withFunk(vertecis_rect2);

    CollisionResponseData colid_dat = closest_Vecpoint_on_poly_to_circle(circle1_ceter, vertecis_rect2);

    if (colid_dat.penetration < radius) {
        respons_vector = colid_dat.contact_normal * (colid_dat.penetration - radius);
        return true;
    }

    return false;
}
*/


