#pragma once

//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <memory>
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>
#include <stdlib.h>
#include <thread>
#include <sstream>
#include <fstream>
#include <istream>
#include <dinput.h>
#include <vector>
#include "boost/lexical_cast.hpp"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include "FW1FontWrapper.h"
#include "Importer.hpp"      // C++ importer interface
#include "scene.h"           // Output data structure
#include "postprocess.h"     // Post processing flags
#include "vector3.h"
#include "simplexnoise.h"
#include "transvoxel.h"


using namespace DirectX;
