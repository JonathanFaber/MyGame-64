#include "includes.h"
#include "shaders.h"
#include "camera.h"
#include "root.h"
#include "sky.h"
#include "timer.h"
#include "sprites.h"
#include "text.h"
#include "model.h"

Root* octTree;

int WINAPI WinMain(HINSTANCE hInstance,	//Main windows function
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine,
	int nShowCmd)
{

	if(!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitializeDirect3d11App(hInstance))	//Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitScene())	//Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if(!InitDirectInput(hInstance))
	{
		MessageBox(0, L"Direct Input Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloop();

	CleanUp();    

	return 0;
}

bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	typedef struct _WNDCLASS {
		UINT cbSize;
		UINT style;
		WNDPROC lpfnWndProc;
		int cbClsExtra;
		int cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",	
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}


	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"Oct Tree",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	//AllowSetForegroundWindow(ASFW_ANY);

	//if (windowActive == true)
		ShowWindow(hwnd, ShowWnd);
	//else
	//	ShowWindow(hwnd, SW_MINIMIZE);
	//	EnableWindow(hwnd, FALSE);

	UpdateWindow(hwnd);

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{	
	//Describe our SwapChain Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 144;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;	// integrated graphics does not support more than 0
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd; 
	swapChainDesc.Windowed = true; 
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create DXGI factory to enumerate adapters///////////////////////////////////////////////////////////////////////////
	IDXGIFactory1 *DXGIFactory;

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);	

	// Use the first adapter	
	IDXGIAdapter1 *Adapter;

	hr = DXGIFactory->EnumAdapters1(0, &Adapter);

	DXGIFactory->Release();	

	//Create our Direct3D 11 Device and SwapChain//////////////////////////////////////////////////////////////////////////
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL, NULL,	D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//Release the Adapter interface
	Adapter->Release();

	//Create our BackBuffer and Render Target
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer11 );
	hr = d3d11Device->CreateRenderTargetView( BackBuffer11, NULL, &renderTargetView );

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width     = Width;
	depthStencilDesc.Height    = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 8;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	//Create the Depth/Stencil View
	d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	d3d11Device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	return true;
}


bool InitDirectInput(HINSTANCE hInstance)
{
	hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL); 

	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	//hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	//hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY);
	hr = DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	//ShowCursor(FALSE);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return true;
}

void CleanUp()
{
	SwapChain->SetFullscreenState(false, NULL);
	PostMessage(hwnd, WM_DESTROY, 0, 0);

	//Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();

	for (int i = 0; i < 9; i++)
		texture[i]->Release();

	BackBuffer11->Release();

	cbPerFrameBufferVS->Release();
	cbPerFrameBufferPS->Release();
	cbPerPlanetBuffer->Release();

	CubesTexSamplerState->Release();

	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
    
	octTree->cleanUp();
	//grass.cleanUp();
	testModel.cleanUp();
	//sprucetree.cleanUp();

	cleanUpText();

	//
	cleanUpShaders();
	//

	smrv->Release();

	DSLessEqual->Release();
	DSLessEqualNoDepth->Release();
	RSCullNone->Release();
}


bool InitScene()
{
	initSpriteTexCoord();

	/////////////
	//SwapChain->SetFullscreenState(true, NULL);
	/////////////


	skybox.CreateSphere(10, 10);
	
	//
	createShaders();
	//

	light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
	light.ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	////////////////////////
	octTree = new Root();
	//sprucetree.init(std::string("models/Pine_4m.obj"));
	//grass.create();
	testModel.load("models/nanosuit/", "nanosuit.obj");

	menuButton_resume.create(spriteTexCoord_Box);
	menuButton_quitWorld.create(spriteTexCoord_Box);


	////////////////////////

	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	//Create the buffer to send to the cbuffer in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerPlanet);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerPlanetBuffer);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrameVS);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBufferVS);

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFramePS);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerFrameBufferPS);

	//Camera information
	camPosition = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

	//Set the Projection matrix
	camProjection[0] = XMMatrixPerspectiveFovLH(0.4f*3.14f, float(Width) / float(Height), 0.1f, 1000.0f);
	camProjection[1] = XMMatrixPerspectiveFovLH(0.4f*3.14f, float(Width) / float(Height), 900.0f, 90000000.0f);
	camProjection[2] = XMMatrixPerspectiveFovLH(0.4f*3.14f, float(Width) / float(Height), 80000000.0f, 8000000000000.0f);
	camProjection[3] = XMMatrixPerspectiveFovLH(0.4f*3.14f, float(Width) / float(Height), 7000000000000.0f, 700000000000000000.0f);


	//sprite information
	spritePosition = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	spriteTarget = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	spriteUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the sprite View matrix
	spriteView = XMMatrixLookAtLH( spritePosition, spriteTarget, spriteUp );

	//Set the sprite Projection matrix
	spriteProjection = XMMatrixOrthographicLH(Width, Height, 0.01f, 1.0f);

	/*
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );

	rtbd.BlendEnable			 = false;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = true;	
	blendDesc.RenderTarget[0] = rtbd;

	d3d11Device->CreateBlendState(&blendDesc, &Transparency);
	//*/

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = false;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0] = rtbd;

	d3d11Device->CreateBlendState(&blendDesc, &Transparency);

	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/grass1.jpg",
		NULL, NULL, &texture[0], NULL );
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/text.png",
		NULL, NULL, &texture[1], NULL );
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/stone.png",
		NULL, NULL, &texture[2], NULL );
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/Water.jpg",
		NULL, NULL, &texture[3], NULL );
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/Snow.png",
		NULL, NULL, &texture[4], NULL );
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/grassalpha.png",
		NULL, NULL, &texture[5], NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/SandTexture.png",
		NULL, NULL, &texture[6], NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/notexture_diff.png",
		NULL, NULL, &texture[7], NULL);
	hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/notexture_norm.png",
		NULL, NULL, &texture[8], NULL);

	//Tell D3D we will be loading a cube texture
	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//Load the texture
	ID3D11Texture2D* SMTexture = 0;
	hr = D3DX11CreateTextureFromFile(d3d11Device, L"textures/skymap1.dds", 
		&loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);

	//Create the textures description
	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	//Tell D3D We have a cube texture, which is an array of 2D textures
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	//Create the Resource view
	hr = d3d11Device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;

	//Create the Sample State
	hr = d3d11Device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.MultisampleEnable = false;
	cmdesc.FrontCounterClockwise = true;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;

	hr = d3d11Device->CreateRasterizerState(&cmdesc, &CWcullMode);

	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &RSCullNone);

	cmdesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &Wireframe);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	d3d11Device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	//////////////////////////////////////////////////////////////////
	D3D11_DEPTH_STENCIL_DESC noDepthStencilDesc;
	ZeroMemory(&noDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	noDepthStencilDesc.DepthEnable = false;
	noDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	d3d11Device->CreateDepthStencilState(&noDepthStencilDesc, &DSLessEqualNoDepth);
	//////////////////////////////////////////////////////////////////

	createText();

	return true;
}

float timeSinceFpsCalc = 0.0f;
int fpsCounter;
//bool timePassed = true;

void UpdateScene(double time)
{
	POINT cursorPosition;
	GetCursorPos(&cursorPosition);
	cursorPos = XMFLOAT2(float(cursorPosition.x), float(cursorPosition.y));

	timeElaps += time;
	timeSinceFpsCalc += float(time);
	fpsCounter++;

	octTree->update(camPos);
	//grass.update(camPos);
	skybox.updateSphere();
	testModel.update();
	//sprucetree.update();

	if(timeSinceFpsCalc > 1.0f){
		timeSinceFpsCalc = 0.0f;
		
		FPS.setText(fpsCounter);

		fpsCounter = 0;
	}
	
	if (showPause == true){
		menuText_resume.setText("Resume Game");
		menuText_quitWorld.setText("Quit World");
	}
	

	if (showPause == true){
		menuButton_resume.spriteScale = XMFLOAT2(600.0f, 100.0f);
		menuButton_resume.spritePos = XMFLOAT2(0.0f, 200.0f);

		menuButton_resume.update();
		onResume = menuButton_resume.onButton;
	}
	else{
		onResume = false;
	}

	if (onResume == true && leftClick == true)
		resume = true;


	if (showPause == true){
		menuButton_quitWorld.spriteScale = XMFLOAT2(600.0f, 100.0f);
		menuButton_quitWorld.spritePos = XMFLOAT2(0.0f, -200.0f);

		menuButton_quitWorld.update();
		onQuit = menuButton_quitWorld.onButton;
	}
	else{
		onQuit = false;
	}

	if (onQuit == true && leftClick == true)
		quit = true;

}

void DrawScene()
{
	//ID3D11Asynchronous *gpuTime;
	//d3d11DevCon->Begin(gpuTime);

	//Clear our render target and depth/stencil view
	float bgColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);	
	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	constbuffPerFrameVS.timeElaps = float(timeElaps);
	d3d11DevCon->UpdateSubresource(cbPerFrameBufferVS, 0, NULL, &constbuffPerFrameVS, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(1, 1, &cbPerFrameBufferVS);
	d3d11DevCon->GSSetConstantBuffers(1, 1, &cbPerFrameBufferVS);

	constbuffPerFramePS.light = light;
	//constbuffPerFrame.timeElaps = 1.0f;
	d3d11DevCon->UpdateSubresource( cbPerFrameBufferPS, 0, NULL, &constbuffPerFramePS, 0, 0 );
	d3d11DevCon->PSSetConstantBuffers(0, 1, &cbPerFrameBufferPS);	

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	//Set the default blend state (no blending) for opaque objects
	d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);

	d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);

	// draw everything with camProjection[1]
	// CLEAR DEPTH STENCIL

	octTree->drawFar();

	d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// draw everything with camProjection[0]

	//skybox.drawSphere();

	//sprucetree.draw();
	testModel.draw();

	//"fine-tune" the blending equation
	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Set the blend state for transparent objects
	d3d11DevCon->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	octTree->drawClose();

	// Transparent with depth
	//grass.draw();

	d3d11DevCon->OMSetDepthStencilState(DSLessEqualNoDepth, 0);
	// Transparent no depth

	FPS.draw(32.0f, 50.0f, 100.0f, 0xff000000);

	
	if (showPause == true) {
		menuText_resume.draw(32.0f, 50.0f, 200.0f, 0xff000000);
		menuText_quitWorld.draw(32.0f, 50.0f, 920.0f, 0xff000000);

		menuButton_resume.draw();
		menuButton_quitWorld.draw();
	}
	

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

int messageloop(){
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while(true)
	{
		BOOL PeekMessageL( 
			LPMSG lpMsg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg
			);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}
		else{
			// run game code    
			frameCount++;
			if(GetTime() > 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}	

			frameTime = GetFrameTime();

			if (GetForegroundWindow() == hwnd)
				windowActive = true;
			else
				windowActive = false;

			DetectInput(frameTime);

			if (playingGame == true && displayCursor == true){
				displayCursor = false;
				ShowCursor(FALSE);
			}
			else if (playingGame == false && displayCursor == false){
				displayCursor = true;
				ShowCursor(TRUE);
			}

			UpdateScene(frameTime);
			DrawScene();

			//if (IsWindowEnabled(hwnd) == TRUE)
			//	windowActive = true;

			//if (windowActive == true)
				//ShowWindow(hwnd, SW_SHOW);
				//EnableWindow(hwnd, TRUE);
			//else
				//CloseWindow(hwnd);
				//ShowWindow(hwnd, SW_SHOWMINNOACTIVE);
				//EnableWindow(hwnd, FALSE);

			//UpdateWindow(hwnd);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch( msg ) {
	case WM_DESTROY:
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}
