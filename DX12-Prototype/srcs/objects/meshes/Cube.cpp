#include "PrecompiledHeaders.h"
#include "objects/meshes/Cube.h"

#pragma region Public Methods

Cube::Cube()
{
	vertices =
	{
        // back face
        { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 0
        { Float3(-1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 1
        { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 2
        { Float3(1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, -1.0f) }, // 3

        // front face
        { Float3(-1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 4
        { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 5
        { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 6
        { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 0.0f, 1.0f) }, // 7

        // left face
        { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 8
        { Float3(-1.0f,  -1.0f, 1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 9
        { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 10
        { Float3(-1.0f, 1.0f,  -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(-1.0f, 0.0f, 0.0f) }, // 11

        // right face
        { Float3(1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 12
        { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 13
        { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 14
        { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(1.0f, 0.0f, 0.0f) }, // 15

        // up face
        { Float3(-1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 16
        { Float3(-1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 17
        { Float3(1.0f,  1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 18
        { Float3(1.0f,  1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, 1.0f, 0.0f) }, // 19

        // down face
        { Float3(-1.0f, -1.0f, -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 20
        { Float3(1.0f, -1.0f,  -1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 21
        { Float3(1.0f, -1.0f,  1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }, // 22
        { Float3(-1.0f, -1.0f, 1.0f), Float3(0.78f, 0.549f, 0.941f), Float3(0.0f, -1.0f, 0.0f) }  // 23
	};

    indices =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };
}

#pragma endregion