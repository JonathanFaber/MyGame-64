//#include "terrain.h"
#include "includes.h"

#ifndef __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__ 

double3 lastCamPos;
double3 camOffset;
double camOffsetLength;
double3 camDir;
double3 camDirRight;
XMVECTOR camPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camTarget;
XMVECTOR camUp;

XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

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
double speed;
double maxSpeed;
bool onlyForward;
bool running;
bool crouching;
double camHeight;
float terrainY0, terrainY1;
XMFLOAT3 faceNormal[2];

void UpdateCameraDirection()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	camDir.x = XMVectorGetX(camTarget);
	camDir.y = XMVectorGetY(camTarget);
	camDir.z = XMVectorGetZ(camTarget);

	camDirRight.x = XMVectorGetX(camRight);
	camDirRight.y = XMVectorGetY(camRight);
	camDirRight.z = XMVectorGetZ(camRight);
}

void UpdateCameraPosition()
{
	//camPosition = XMVectorSet(camPos.x, camPos.y, camPos.z, 0.0f);

	moveLeftRight = 0.0;
	moveBackForward = 0.0;

	camTarget = camPosition + camTarget;	

	camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
}

void DetectMouseInput()
{
	DIMOUSESTATE mouseCurrState;
	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	if (windowActive == true){
		if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
		{
			if (playingGame == true){
				camYaw += mouseLastState.lX * 0.002;
				camPitch += mouseCurrState.lY * 0.002;

				if(camPitch > 1.49)
					camPitch = 1.49;
				if(camPitch < -1.49)
					camPitch = -1.49;


				if (cursorPos.x < 50.0f)
					SetCursorPos(50, int(cursorPos.y));
				if (cursorPos.y < 50.0f)
					SetCursorPos(int(cursorPos.x), 50);
				if (cursorPos.x > float(Width)-50.0f)
					SetCursorPos(Width-50, int(cursorPos.y));
				if (cursorPos.y > float(Height)-50.0f)
					SetCursorPos(int(cursorPos.x), Height-50);
			}

			mouseLastState = mouseCurrState;
		}

		if(mouseCurrState.rgbButtons[0]){
			leftClick = true;
		}
		else{
			leftClick = false;
		}


		UpdateCameraDirection();
	}
}


void DetectKeyboardInput(double time)
{
	speed = 1000000.0*time;

	
	BYTE keyboardState[256];

	DIKeyboard->Acquire();
	DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if (windowActive == true){
		if(keyboardState[DIK_ESCAPE] & 0x80){
			if (pressing_ESCAPE == false){
				if (showPause == false){
					SetCursorPos(int(float(Width)/2), int(float(Height)/2));
					showPause = true;
					playingGame = false;
				}
				else{
					showPause = false;
					playingGame = true;

				}
			}
			pressing_ESCAPE = true;
		}
		else{
			pressing_ESCAPE = false;
		}

		if(keyboardState[DIK_LALT] & 0x80){
			showPause = true;
			playingGame = false;
			SetCursorPos(int(float(Width)/2), int(float(Height)/2));
		}
	}

	if (resume == true){
		resume = false;
		showPause = false;
		playingGame = true;
	}
	
	if (quit == true)
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	if(playingGame == true && windowActive == true){
		if(keyboardState[DIK_LCONTROL] & 0x80){
			crouching = true;
		}
		else if(keyboardState[DIK_LSHIFT] & 0x80){
			running = true;
			crouching = false;
		}
		else{
			running = false;
			crouching = false;
		}

		if(keyboardState[DIK_W] & 0x80)
		{
			if (crouching == true){
				moveBackForward += speed/8;
			}
			else if (running == true){
				moveBackForward += speed*8;
			}
			else{
				moveBackForward += speed;
			}
		}

		if(keyboardState[DIK_A] & 0x80)
		{
			if (crouching == true){
				moveLeftRight -= speed/8;
			}
			else if (running == true){
				moveLeftRight -= speed*4;
			}
			else{
				moveLeftRight -= speed;
			}
		}

		if(keyboardState[DIK_D] & 0x80)
		{
			if (crouching == true){
				moveLeftRight += speed/8;
			}
			else if (running == true){
				moveLeftRight += speed*4;
			}
			else{
				moveLeftRight += speed;
			}
		}

		if(keyboardState[DIK_S] & 0x80)
		{
			if (crouching == true){
				moveBackForward -= speed/8;
			}
			else if (running == true){
				moveBackForward -= speed*4;
			}
			else{
				moveBackForward -= speed;
			}
		}

		if (crouching == true) {
			camHeight = 1.0;
			maxSpeed = speed / 8;
		}
		else if (running == true) {
			camHeight = 1.75;
			maxSpeed = speed * 8;
		}
		else {
			camHeight = 1.75;
			maxSpeed = speed;
		}

		camOffset.x = moveBackForward*camDir.x + moveLeftRight*camDirRight.x;
		camOffset.y = moveBackForward*camDir.y + moveLeftRight*camDirRight.y;
		camOffset.z = moveBackForward*camDir.z + moveLeftRight*camDirRight.z;

		camPos.x += camOffset.x;
		camPos.y += camOffset.y;
		camPos.z += camOffset.z;

		/*
		if(keyboardState[DIK_SPACE] & 0x80 && y_vel == 0.0f)
		{
			camPos.y += 0.02f;
			y_vel = 10.0f;
		}

		if (moveBackForward != 0.0f){//Go the velocity of moveBackForward
			moveNormalized = normalize(XMFLOAT2(moveBackForward, moveLeftRight));
			moveBackForward *= positive(moveNormalized.x);
			moveLeftRight *= positive(moveNormalized.x);
		}

		camOffset.x = moveBackForward*camDir.x + moveLeftRight*camDirRight.x;
		camOffset.z = moveBackForward*camDir.z + moveLeftRight*camDirRight.z;

		camPos.x += camOffset.x;
		camPos.y += camOffset.y;
		camPos.z += camOffset.z;

		y_vel_old = y_vel;
		y_vel -= grav * time;
		camPos.y += (y_vel+y_vel_old)/2 * time;

		if (camPos.y < camHeight){
			camPos.y = camHeight;
			if (y_vel <= 0.0f){
				y_vel = 0.0f;
			}
		}
		*/

		//playerVelocity = vLength(XMFLOAT3(camPos.x - lastCamPos.x, camPos.y - lastCamPos.y, camPos.z - lastCamPos.z))/time;
		//playerVelocity = vLength(XMFLOAT2(camOffset.x, camOffset.z))/time;

		///////////////////
		//camPos.y = 50.0f;//
		///////////////////

		UpdateCameraPosition();

	}

	return;
}

void DetectInput(double time)
{
	DetectMouseInput();

	DetectKeyboardInput(time);
}




#endif