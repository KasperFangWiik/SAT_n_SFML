#pragma once

// i allready inclue Entity_remake.h in essential_collision.h so do i need to include it again?
#include "Entity_remake.h"
#include "essential_collision.h"

/*
Different ways of implementing actions like effects of collisions/intersections?

1. Action is a pure virtual class and we call on static functions that does stuff like take two entity_id's or colliders and calculate the respons

2. Have a collection of general functions that we categorise as actions by collecting them in this file...

both 1 and 2 need a way of knowing what "type" the entity or entity's are so that means we some how need entity types 

kan ha en ny vector av entity_id's och om ett id ligger i denna så är den av denna typ... Verkar trash då man måste typ lite igenom alla verctorer

ha en enum som representerar alla typer som en entity kan vara och sedan kan man stora denna i entity classen eller så gör man en 
vector och checkar id paret och värdet där är typen, men detta gör det svårare att classifiera en entity som mer än en "sort"/"typ" av entity.






*/
class Action {

};