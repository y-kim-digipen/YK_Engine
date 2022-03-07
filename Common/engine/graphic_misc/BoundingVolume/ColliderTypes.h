/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ColliderTypes.h
Purpose: Define collider types used for collision testing
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Mar 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_COLLIDERTYPES_H
#define ENGINE_COLLIDERTYPES_H

enum class ColliderTypes
{
    AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
};

#endif //ENGINE_COLLIDERTYPES_H
