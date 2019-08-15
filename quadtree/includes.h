#ifndef __INCLUDES_H_INCLUDED__
#define __INCLUDES_H_INCLUDED__ 

#define DIRECTINPUT_VERSION 0x0800

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


using namespace DirectX;


//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
ID3D11BlendState* Transparency;

ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* Wireframe;

ID3D11ShaderResourceView* texture[16];
ID3D11SamplerState* CubesTexSamplerState;

ID3D11Buffer* cbPerObjectBuffer;
ID3D11Buffer* cbPerPlanetBuffer;
ID3D11Buffer* cbPerFrameBufferVS;
ID3D11Buffer* cbPerFrameBufferPS;
ID3D11Buffer* cbInstancePosBuffer;

ID3D11Texture2D *BackBuffer11;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

ID3D11ShaderResourceView* smrv;

ID3D11DepthStencilState* DSLessEqual;
ID3D11DepthStencilState* DSLessEqualNoDepth;
ID3D11RasterizerState* RSCullNone;

//Global Declarations - Others//
LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;
HWND desktopHwnd = NULL;
HRESULT hr;

int Width  = 2560;
int Height = 1440;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationy;
XMMATRIX Rotationz;

XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection[4]; // 0 to 3 is smallest to biggest

XMMATRIX spriteView;
XMMATRIX spriteProjection;

XMVECTOR spritePosition;
XMVECTOR spriteTarget;
XMVECTOR spriteUp;


XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

XMFLOAT2 cursorPos;
bool displayCursor = true;
bool leftClick = false;
bool rightClick = false;

bool pressing_ESCAPE = false;

bool resume = false;
bool quit = false;

bool onResume = false;
bool onQuit = false;

bool showGame = true;
bool playingGame = true;
bool showPause = false;

bool windowActive = true;


static const int gradientChunkSize = 64;
static const int numGradients = 2;
XMFLOAT2 gradients[numGradients][numGradients];
int permutation[256];


int counter = 0;

const int chunkLength = 16;
const double minLength = chunkLength;
const double maxLength = chunkLength * 64;

//Function Prototypes//
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void UpdateScene(double time);
void DrawScene();


void StartTimer();
double GetTime();
double GetFrameTime();

bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
int messageloop();

bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};

cbPerObject cbPerObj;

struct cbPerPlanet
{
	// need different names if this and cbPerObject are in the same shader
	//XMMATRIX  WVP;
	//XMMATRIX World;
	XMFLOAT3 Translation;
	XMMATRIX View;
	XMMATRIX Proj;
};

cbPerPlanet cbPerPlanetObj;

struct cbPerFrameVS
{
	float timeElaps;
	XMFLOAT3 pad1;
};

double timeElaps = 0.0;

cbPerFrameVS constbuffPerFrameVS;

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 dir;
};

Light light;

struct cbPerFramePS
{
	Light  light;
	float pad2;
};

cbPerFramePS constbuffPerFramePS;

struct InstanceData
{
	InstanceData() {}
	XMFLOAT2 instancePos;
	XMFLOAT4 instanceHeights;
};


struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct ModelVertex	//Overloaded Vertex Structure
{
	ModelVertex() {}
	ModelVertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float tx, float ty, float tz, float btx, float bty, float btz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tx, ty, tz), bitangent(btx, bty, btz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 bitangent;
};

struct PlanetVertex	//Overloaded Vertex Structure
{
	PlanetVertex(){}
	PlanetVertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float h, float htype)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz), height(h), landTypeHeight(htype){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	float height;
	float landTypeHeight;
};

struct SpriteVertex	//Overloaded Vertex Structure
{
	SpriteVertex(){}
	SpriteVertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};


D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

D3D11_INPUT_ELEMENT_DESC modelLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC planetLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "HEIGHT",	0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LANDTYPEHEIGHT",	0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC spriteLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};

D3D11_INPUT_ELEMENT_DESC instance_layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// Data from the instance buffer
	{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEHEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

struct double2 {
	double x;
	double y;

	double2() {}
	double2(double x, double y) : x(x), y(y) {}
};

struct double3 {
	double x;
	double y;
	double z;

	double3() { x = 0; y = 0; z = 0; }
	double3(double x, double y, double z) : x(x), y(y), z(z) {}
	double3(XMFLOAT3 v) : x(v.x), y(v.y), z(v.z) {}

	double3 operator+(const double3 &v) {
		return double3(this->x + v.x, this->y + v.y, this->z + v.z);
	}

	double3 operator-(const double3 &v) {
		return double3(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	double3 operator*(const double &v) {
		return double3(this->x * v, this->y * v, this->z * v);
	}

	double3 operator/(const double &v) {
		return double3(this->x / v, this->y / v, this->z / v);
	}

	XMFLOAT3 float3() {
		return XMFLOAT3(x, y, z);
	}
};

struct int2 {
	int x;
	int y;

	int2() {}
	int2(int x, int y) : x(x), y(y) {}
};

struct int3 {
	int x;
	int y;
	int z;

	int3() {}
	int3(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct vec2 {
	int x;
	int z;

	vec2() {}
	vec2(int x, int z) : x(x), z(z) {}
};


float positive(float num){
	if (num < 0.0f){
		num = -num;
	}
	return num;
}

double positive(double num1) {
	if (num1 < 0.0) {
		num1 = -num1;
	}
	return num1;
}

float negative(float num){
	if (num > 0.0f){
		num = -num;
	}
	return num;
}

float opposite(float num){
	return -num;
}

XMFLOAT3 normalize(XMFLOAT3 vector){
	float length = sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);

	if (length != 0.0f){
		vector.x = vector.x/length;
		vector.y = vector.y/length;
		vector.z = vector.z/length;
	}

	return vector;
}

double3 normalize(double3 vector2) {
	double length = sqrt(vector2.x*vector2.x + vector2.y*vector2.y + vector2.z*vector2.z);

	if (length != 0.0) {
		vector2.x = vector2.x / length;
		vector2.y = vector2.y / length;
		vector2.z = vector2.z / length;
	}

	return vector2;
}

XMFLOAT2 normalize(XMFLOAT2 vector1){
	float length1 = sqrt(vector1.x*vector1.x + vector1.y*vector1.y);

	if (length1 != 0.0f){
		vector1.x = vector1.x/length1;
		vector1.y = vector1.y/length1;
	}

	return vector1;
}

double vLength(double3 v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double vLength(XMFLOAT3 v1) {
	return sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
}

float dotProduct(XMFLOAT2 a, XMFLOAT2 b){
	return a.x*b.x + a.y*b.y;
}

float dotProduct(XMFLOAT3 a1, XMFLOAT3 b1){
	return a1.x*b1.x + a1.y*b1.y + a1.z*b1.z;
}

double dotProduct(double3 a2, double3 b2) {
	return a2.x*b2.x + a2.y*b2.y + a2.z*b2.z;
}

XMFLOAT3 crossProduct(XMFLOAT3 a, XMFLOAT3 b) {
	return XMFLOAT3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

double3 crossProduct(double3 a, double3 b) {
	return double3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

struct osn_context *ctx;

// camera

double3 camPos = double3(0.0, 0.0, -60.0);

double3 lastCamPos = camPos;
double3 camOffset;
double camOffsetLength;
double3 camDir;
double3 camDirRight;
XMVECTOR camPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camTarget;
XMVECTOR camUp;

XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
//XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMMATRIX camRotationMatrix;

double moveLeftRight = 0.0;
double moveBackForward = 0.0;
double2 moveNormalized;

double camYaw = 0.0;
double camPitch = 0.0;

double grav = 9.81;
double x_vel = 0.0;
double y_vel = 0.0;
double y_vel_old;
double z_vel = 0.0;

double playerVelocity;
double speed = 10.0;
double maxSpeed = 200.0;
double speedTime;
double maxspeedTime;
bool onlyForward;
bool running;
bool crouching;
double camHeight;

static INT8 edgeTable[256][12] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 1, 8, 1, 9,-1,-1,-1,-1,-1,-1},
	{10, 1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0, 1, 2,10,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 2, 9, 2,10,-1,-1,-1,-1,-1,-1},
	{ 3, 2, 8, 2,10, 8, 8,10, 9,-1,-1,-1},
	{11, 2, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 2, 0,11, 0, 8,-1,-1,-1,-1,-1,-1},
	{11, 2, 3, 0, 1, 9,-1,-1,-1,-1,-1,-1},
	{ 2, 1,11, 1, 9,11,11, 9, 8,-1,-1,-1},
	{10, 1, 3,10, 3,11,-1,-1,-1,-1,-1,-1},
	{ 1, 0,10, 0, 8,10,10, 8,11,-1,-1,-1},
	{ 0, 3, 9, 3,11, 9, 9,11,10,-1,-1,-1},
	{ 8,10, 9, 8,11,10,-1,-1,-1,-1,-1,-1},
	{ 8, 4, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 4, 3, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 1, 9, 0, 8, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 9, 4, 1, 4, 7, 1, 1, 7, 3,-1,-1,-1},
	{10, 1, 2, 8, 4, 7,-1,-1,-1,-1,-1,-1},
	{ 2,10, 1, 0, 4, 7, 0, 7, 3,-1,-1,-1},
	{ 4, 7, 8, 0, 2,10, 0,10, 9,-1,-1,-1},
	{ 2, 7, 3, 2, 9, 7, 7, 9, 4, 2,10, 9},
	{ 2, 3,11, 7, 8, 4,-1,-1,-1,-1,-1,-1},
	{ 7,11, 4,11, 2, 4, 4, 2, 0,-1,-1,-1},
	{ 3,11, 2, 4, 7, 8, 9, 0, 1,-1,-1,-1},
	{ 2, 7,11, 2, 1, 7, 1, 4, 7, 1, 9, 4},
	{ 8, 4, 7,11,10, 1,11, 1, 3,-1,-1,-1},
	{11, 4, 7, 1, 4,11, 1,11,10, 1, 0, 4},
	{ 3, 8, 0, 7,11, 4,11, 9, 4,11,10, 9},
	{ 7,11, 4, 4,11, 9,11,10, 9,-1,-1,-1},
	{ 9, 5, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 8, 4, 9, 5,-1,-1,-1,-1,-1,-1},
	{ 5, 4, 0, 5, 0, 1,-1,-1,-1,-1,-1,-1},
	{ 4, 8, 5, 8, 3, 5, 5, 3, 1,-1,-1,-1},
	{ 2,10, 1, 9, 5, 4,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3, 5, 4, 9,10, 1, 2,-1,-1,-1},
	{10, 5, 2, 5, 4, 2, 2, 4, 0,-1,-1,-1},
	{ 3, 4, 8, 3, 2, 4, 2, 5, 4, 2,10, 5},
	{11, 2, 3, 9, 5, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 4, 8,11, 2, 8, 2, 0,-1,-1,-1},
	{ 3,11, 2, 1, 5, 4, 1, 4, 0,-1,-1,-1},
	{ 8, 5, 4, 2, 5, 8, 2, 8,11, 2, 1, 5},
	{ 5, 4, 9, 1, 3,11, 1,11,10,-1,-1,-1},
	{ 0, 9, 1, 4, 8, 5, 8,10, 5, 8,11,10},
	{ 3, 4, 0, 3,10, 4, 4,10, 5, 3,11,10},
	{ 4, 8, 5, 5, 8,10, 8,11,10,-1,-1,-1},
	{ 9, 5, 7, 9, 7, 8,-1,-1,-1,-1,-1,-1},
	{ 0, 9, 3, 9, 5, 3, 3, 5, 7,-1,-1,-1},
	{ 8, 0, 7, 0, 1, 7, 7, 1, 5,-1,-1,-1},
	{ 1, 7, 3, 1, 5, 7,-1,-1,-1,-1,-1,-1},
	{ 1, 2,10, 5, 7, 8, 5, 8, 9,-1,-1,-1},
	{ 9, 1, 0,10, 5, 2, 5, 3, 2, 5, 7, 3},
	{ 5, 2,10, 8, 2, 5, 8, 5, 7, 8, 0, 2},
	{10, 5, 2, 2, 5, 3, 5, 7, 3,-1,-1,-1},
	{11, 2, 3, 8, 9, 5, 8, 5, 7,-1,-1,-1},
	{ 9, 2, 0, 9, 7, 2, 2, 7,11, 9, 5, 7},
	{ 0, 3, 8, 2, 1,11, 1, 7,11, 1, 5, 7},
	{ 2, 1,11,11, 1, 7, 1, 5, 7,-1,-1,-1},
	{ 3, 9, 1, 3, 8, 9, 7,11,10, 7,10, 5},
	{ 9, 1, 0,10, 7,11,10, 5, 7,-1,-1,-1},
	{ 3, 8, 0, 7,10, 5, 7,11,10,-1,-1,-1},
	{11, 5, 7,11,10, 5,-1,-1,-1,-1,-1,-1},
	{10, 6, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0,10, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 0, 1, 9, 5,10, 6,-1,-1,-1,-1,-1,-1},
	{10, 6, 5, 9, 8, 3, 9, 3, 1,-1,-1,-1},
	{ 1, 2, 6, 1, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3, 2, 6, 5, 2, 5, 1,-1,-1,-1},
	{ 5, 9, 6, 9, 0, 6, 6, 0, 2,-1,-1,-1},
	{ 9, 6, 5, 3, 6, 9, 3, 9, 8, 3, 2, 6},
	{ 3,11, 2,10, 6, 5,-1,-1,-1,-1,-1,-1},
	{ 6, 5,10, 2, 0, 8, 2, 8,11,-1,-1,-1},
	{ 1, 9, 0, 6, 5,10,11, 2, 3,-1,-1,-1},
	{ 1,10, 2, 5, 9, 6, 9,11, 6, 9, 8,11},
	{11, 6, 3, 6, 5, 3, 3, 5, 1,-1,-1,-1},
	{ 0, 5, 1, 0,11, 5, 5,11, 6, 0, 8,11},
	{ 0, 5, 9, 0, 3, 5, 3, 6, 5, 3,11, 6},
	{ 5, 9, 6, 6, 9,11, 9, 8,11,-1,-1,-1},
	{10, 6, 5, 4, 7, 8,-1,-1,-1,-1,-1,-1},
	{ 5,10, 6, 7, 3, 0, 7, 0, 4,-1,-1,-1},
	{ 5,10, 6, 0, 1, 9, 8, 4, 7,-1,-1,-1},
	{ 4, 5, 9, 6, 7,10, 7, 1,10, 7, 3, 1},
	{ 7, 8, 4, 5, 1, 2, 5, 2, 6,-1,-1,-1},
	{ 4, 1, 0, 4, 5, 1, 6, 7, 3, 6, 3, 2},
	{ 9, 4, 5, 8, 0, 7, 0, 6, 7, 0, 2, 6},
	{ 4, 5, 9, 6, 3, 2, 6, 7, 3,-1,-1,-1},
	{ 7, 8, 4, 2, 3,11,10, 6, 5,-1,-1,-1},
	{11, 6, 7,10, 2, 5, 2, 4, 5, 2, 0, 4},
	{11, 6, 7, 8, 0, 3, 1,10, 2, 9, 4, 5},
	{ 6, 7,11, 1,10, 2, 9, 4, 5,-1,-1,-1},
	{ 6, 7,11, 4, 5, 8, 5, 3, 8, 5, 1, 3},
	{ 6, 7,11, 4, 1, 0, 4, 5, 1,-1,-1,-1},
	{ 4, 5, 9, 3, 8, 0,11, 6, 7,-1,-1,-1},
	{ 9, 4, 5, 7,11, 6,-1,-1,-1,-1,-1,-1},
	{10, 6, 4,10, 4, 9,-1,-1,-1,-1,-1,-1},
	{ 8, 3, 0, 9,10, 6, 9, 6, 4,-1,-1,-1},
	{ 1,10, 0,10, 6, 0, 0, 6, 4,-1,-1,-1},
	{ 8, 6, 4, 8, 1, 6, 6, 1,10, 8, 3, 1},
	{ 9, 1, 4, 1, 2, 4, 4, 2, 6,-1,-1,-1},
	{ 1, 0, 9, 3, 2, 8, 2, 4, 8, 2, 6, 4},
	{ 2, 4, 0, 2, 6, 4,-1,-1,-1,-1,-1,-1},
	{ 3, 2, 8, 8, 2, 4, 2, 6, 4,-1,-1,-1},
	{ 2, 3,11, 6, 4, 9, 6, 9,10,-1,-1,-1},
	{ 0,10, 2, 0, 9,10, 4, 8,11, 4,11, 6},
	{10, 2, 1,11, 6, 3, 6, 0, 3, 6, 4, 0},
	{10, 2, 1,11, 4, 8,11, 6, 4,-1,-1,-1},
	{ 1, 4, 9,11, 4, 1,11, 1, 3,11, 6, 4},
	{ 0, 9, 1, 4,11, 6, 4, 8,11,-1,-1,-1},
	{11, 6, 3, 3, 6, 0, 6, 4, 0,-1,-1,-1},
	{ 8, 6, 4, 8,11, 6,-1,-1,-1,-1,-1,-1},
	{ 6, 7,10, 7, 8,10,10, 8, 9,-1,-1,-1},
	{ 9, 3, 0, 6, 3, 9, 6, 9,10, 6, 7, 3},
	{ 6, 1,10, 6, 7, 1, 7, 0, 1, 7, 8, 0},
	{ 6, 7,10,10, 7, 1, 7, 3, 1,-1,-1,-1},
	{ 7, 2, 6, 7, 9, 2, 2, 9, 1, 7, 8, 9},
	{ 1, 0, 9, 3, 6, 7, 3, 2, 6,-1,-1,-1},
	{ 8, 0, 7, 7, 0, 6, 0, 2, 6,-1,-1,-1},
	{ 2, 7, 3, 2, 6, 7,-1,-1,-1,-1,-1,-1},
	{ 7,11, 6, 3, 8, 2, 8,10, 2, 8, 9,10},
	{11, 6, 7,10, 0, 9,10, 2, 0,-1,-1,-1},
	{ 2, 1,10, 7,11, 6, 8, 0, 3,-1,-1,-1},
	{ 1,10, 2, 6, 7,11,-1,-1,-1,-1,-1,-1},
	{ 7,11, 6, 3, 9, 1, 3, 8, 9,-1,-1,-1},
	{ 9, 1, 0,11, 6, 7,-1,-1,-1,-1,-1,-1},
	{ 0, 3, 8,11, 6, 7,-1,-1,-1,-1,-1,-1},
	{11, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 7, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 0, 8, 3,11, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 9, 0, 1,11, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 7, 6,11, 3, 1, 9, 3, 9, 8,-1,-1,-1},
	{ 1, 2,10, 6,11, 7,-1,-1,-1,-1,-1,-1},
	{ 2,10, 1, 7, 6,11, 8, 3, 0,-1,-1,-1},
	{11, 7, 6,10, 9, 0,10, 0, 2,-1,-1,-1},
	{ 7, 6,11, 3, 2, 8, 8, 2,10, 8,10, 9},
	{ 2, 3, 7, 2, 7, 6,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 0, 7, 6, 0, 0, 6, 2,-1,-1,-1},
	{ 1, 9, 0, 3, 7, 6, 3, 6, 2,-1,-1,-1},
	{ 7, 6, 2, 7, 2, 9, 2, 1, 9, 7, 9, 8},
	{ 6,10, 7,10, 1, 7, 7, 1, 3,-1,-1,-1},
	{ 6,10, 1, 6, 1, 7, 7, 1, 0, 7, 0, 8},
	{ 9, 0, 3, 6, 9, 3, 6,10, 9, 6, 3, 7},
	{ 6,10, 7, 7,10, 8,10, 9, 8,-1,-1,-1},
	{ 8, 4, 6, 8, 6,11,-1,-1,-1,-1,-1,-1},
	{11, 3, 6, 3, 0, 6, 6, 0, 4,-1,-1,-1},
	{ 0, 1, 9, 4, 6,11, 4,11, 8,-1,-1,-1},
	{ 1, 9, 4,11, 1, 4,11, 3, 1,11, 4, 6},
	{10, 1, 2,11, 8, 4,11, 4, 6,-1,-1,-1},
	{10, 1, 2,11, 3, 6, 6, 3, 0, 6, 0, 4},
	{ 0, 2,10, 0,10, 9, 4,11, 8, 4, 6,11},
	{ 2,11, 3, 6, 9, 4, 6,10, 9,-1,-1,-1},
	{ 3, 8, 2, 8, 4, 2, 2, 4, 6,-1,-1,-1},
	{ 2, 0, 4, 2, 4, 6,-1,-1,-1,-1,-1,-1},
	{ 1, 9, 0, 3, 8, 2, 2, 8, 4, 2, 4, 6},
	{ 9, 4, 1, 1, 4, 2, 4, 6, 2,-1,-1,-1},
	{ 8, 4, 6, 8, 6, 1, 6,10, 1, 8, 1, 3},
	{ 1, 0,10,10, 0, 6, 0, 4, 6,-1,-1,-1},
	{ 8, 0, 3, 9, 6,10, 9, 4, 6,-1,-1,-1},
	{10, 4, 6,10, 9, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 4, 7, 6,11,-1,-1,-1,-1,-1,-1},
	{ 4, 9, 5, 3, 0, 8,11, 7, 6,-1,-1,-1},
	{ 6,11, 7, 4, 0, 1, 4, 1, 5,-1,-1,-1},
	{ 6,11, 7, 4, 8, 5, 5, 8, 3, 5, 3, 1},
	{ 6,11, 7, 1, 2,10, 9, 5, 4,-1,-1,-1},
	{11, 7, 6, 8, 3, 0, 1, 2,10, 9, 5, 4},
	{11, 7, 6,10, 5, 2, 2, 5, 4, 2, 4, 0},
	{ 7, 4, 8, 2,11, 3,10, 5, 6,-1,-1,-1},
	{ 4, 9, 5, 6, 2, 3, 6, 3, 7,-1,-1,-1},
	{ 9, 5, 4, 8, 7, 0, 0, 7, 6, 0, 6, 2},
	{ 4, 0, 1, 4, 1, 5, 6, 3, 7, 6, 2, 3},
	{ 7, 4, 8, 5, 2, 1, 5, 6, 2,-1,-1,-1},
	{ 4, 9, 5, 6,10, 7, 7,10, 1, 7, 1, 3},
	{ 5, 6,10, 0, 9, 1, 8, 7, 4,-1,-1,-1},
	{ 5, 6,10, 7, 0, 3, 7, 4, 0,-1,-1,-1},
	{10, 5, 6, 4, 8, 7,-1,-1,-1,-1,-1,-1},
	{ 5, 6, 9, 6,11, 9, 9,11, 8,-1,-1,-1},
	{ 0, 9, 5, 0, 5, 3, 3, 5, 6, 3, 6,11},
	{ 0, 1, 5, 0, 5,11, 5, 6,11, 0,11, 8},
	{11, 3, 6, 6, 3, 5, 3, 1, 5,-1,-1,-1},
	{ 1, 2,10, 5, 6, 9, 9, 6,11, 9,11, 8},
	{ 1, 0, 9, 6,10, 5,11, 3, 2,-1,-1,-1},
	{ 6,10, 5, 2, 8, 0, 2,11, 8,-1,-1,-1},
	{ 3, 2,11,10, 5, 6,-1,-1,-1,-1,-1,-1},
	{ 9, 5, 6, 3, 9, 6, 3, 8, 9, 3, 6, 2},
	{ 5, 6, 9, 9, 6, 0, 6, 2, 0,-1,-1,-1},
	{ 0, 3, 8, 2, 5, 6, 2, 1, 5,-1,-1,-1},
	{ 1, 6, 2, 1, 5, 6,-1,-1,-1,-1,-1,-1},
	{10, 5, 6, 9, 3, 8, 9, 1, 3,-1,-1,-1},
	{ 0, 9, 1, 5, 6,10,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3,10, 5, 6,-1,-1,-1,-1,-1,-1},
	{10, 5, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{11, 7, 5,11, 5,10,-1,-1,-1,-1,-1,-1},
	{ 3, 0, 8, 7, 5,10, 7,10,11,-1,-1,-1},
	{ 9, 0, 1,10,11, 7,10, 7, 5,-1,-1,-1},
	{ 3, 1, 9, 3, 9, 8, 7,10,11, 7, 5,10},
	{ 2,11, 1,11, 7, 1, 1, 7, 5,-1,-1,-1},
	{ 0, 8, 3, 2,11, 1, 1,11, 7, 1, 7, 5},
	{ 9, 0, 2, 9, 2, 7, 2,11, 7, 9, 7, 5},
	{11, 3, 2, 8, 5, 9, 8, 7, 5,-1,-1,-1},
	{10, 2, 5, 2, 3, 5, 5, 3, 7,-1,-1,-1},
	{ 5,10, 2, 8, 5, 2, 8, 7, 5, 8, 2, 0},
	{ 9, 0, 1,10, 2, 5, 5, 2, 3, 5, 3, 7},
	{ 1,10, 2, 5, 8, 7, 5, 9, 8,-1,-1,-1},
	{ 1, 3, 7, 1, 7, 5,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 0, 0, 7, 1, 7, 5, 1,-1,-1,-1},
	{ 0, 3, 9, 9, 3, 5, 3, 7, 5,-1,-1,-1},
	{ 9, 7, 5, 9, 8, 7,-1,-1,-1,-1,-1,-1},
	{ 4, 5, 8, 5,10, 8, 8,10,11,-1,-1,-1},
	{ 3, 0, 4, 3, 4,10, 4, 5,10, 3,10,11},
	{ 0, 1, 9, 4, 5, 8, 8, 5,10, 8,10,11},
	{ 5, 9, 4, 1,11, 3, 1,10,11,-1,-1,-1},
	{ 8, 4, 5, 2, 8, 5, 2,11, 8, 2, 5, 1},
	{ 3, 2,11, 1, 4, 5, 1, 0, 4,-1,-1,-1},
	{ 9, 4, 5, 8, 2,11, 8, 0, 2,-1,-1,-1},
	{11, 3, 2, 9, 4, 5,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 4, 3, 4, 2, 2, 4, 5, 2, 5,10},
	{10, 2, 5, 5, 2, 4, 2, 0, 4,-1,-1,-1},
	{ 0, 3, 8, 5, 9, 4,10, 2, 1,-1,-1,-1},
	{ 2, 1,10, 9, 4, 5,-1,-1,-1,-1,-1,-1},
	{ 4, 5, 8, 8, 5, 3, 5, 1, 3,-1,-1,-1},
	{ 5, 0, 4, 5, 1, 0,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 0, 4, 5, 9,-1,-1,-1,-1,-1,-1},
	{ 9, 4, 5,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 7, 4,11, 4, 9,11,11, 9,10,-1,-1,-1},
	{ 3, 0, 8, 7, 4,11,11, 4, 9,11, 9,10},
	{11, 7, 4, 1,11, 4, 1,10,11, 1, 4, 0},
	{ 8, 7, 4,11, 1,10,11, 3, 1,-1,-1,-1},
	{ 2,11, 7, 2, 7, 1, 1, 7, 4, 1, 4, 9},
	{ 3, 2,11, 4, 8, 7, 9, 1, 0,-1,-1,-1},
	{ 7, 4,11,11, 4, 2, 4, 0, 2,-1,-1,-1},
	{ 2,11, 3, 7, 4, 8,-1,-1,-1,-1,-1,-1},
	{ 2, 3, 7, 2, 7, 9, 7, 4, 9, 2, 9,10},
	{ 4, 8, 7, 0,10, 2, 0, 9,10,-1,-1,-1},
	{ 2, 1,10, 0, 7, 4, 0, 3, 7,-1,-1,-1},
	{10, 2, 1, 8, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 9, 1, 4, 4, 1, 7, 1, 3, 7,-1,-1,-1},
	{ 1, 0, 9, 8, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 3, 4, 0, 3, 7, 4,-1,-1,-1,-1,-1,-1},
	{ 8, 7, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 9,10, 8,10,11,-1,-1,-1,-1,-1,-1},
	{ 0, 9, 3, 3, 9,11, 9,10,11,-1,-1,-1},
	{ 1,10, 0, 0,10, 8,10,11, 8,-1,-1,-1},
	{10, 3, 1,10,11, 3,-1,-1,-1,-1,-1,-1},
	{ 2,11, 1, 1,11, 9,11, 8, 9,-1,-1,-1},
	{11, 3, 2, 0, 9, 1,-1,-1,-1,-1,-1,-1},
	{11, 0, 2,11, 8, 0,-1,-1,-1,-1,-1,-1},
	{11, 3, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 3, 8, 2, 2, 8,10, 8, 9,10,-1,-1,-1},
	{ 9, 2, 0, 9,10, 2,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3, 1,10, 2,-1,-1,-1,-1,-1,-1},
	{10, 2, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 1, 3, 8, 9, 1,-1,-1,-1,-1,-1,-1},
	{ 9, 1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 8, 0, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

#endif
