#include "includes.h"
#include "planet.h"

#ifndef __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__ 

void UpdateCameraDirection()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	//camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

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
		if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY) || (mouseCurrState.lZ != mouseLastState.lZ)){
			if (playingGame == true){
				camYaw += mouseLastState.lX * 0.002;
				camPitch += mouseCurrState.lY * 0.002;

				speed *= pow(2, mouseCurrState.lZ * 0.002);

				//if (maxSpeed > 10.0)
				//	maxSpeed = (vLength(camPos) - 4194304.0)/4;
				//else if (maxSpeed < 10.0)
				//	maxSpeed = 10.0;

				//if (speed > maxSpeed)
				//	speed = maxSpeed;

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
	speedTime = speed*time;

	
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
				moveBackForward += speedTime/8;
			}
			else if (running == true){
				moveBackForward += speedTime*8;
			}
			else{
				moveBackForward += speedTime;
			}
		}

		if(keyboardState[DIK_A] & 0x80)
		{
			if (crouching == true){
				moveLeftRight -= speedTime/8;
			}
			else if (running == true){
				moveLeftRight -= speedTime*4;
			}
			else{
				moveLeftRight -= speedTime;
			}
		}

		if(keyboardState[DIK_D] & 0x80)
		{
			if (crouching == true){
				moveLeftRight += speedTime/8;
			}
			else if (running == true){
				moveLeftRight += speedTime*4;
			}
			else{
				moveLeftRight += speedTime;
			}
		}

		if(keyboardState[DIK_S] & 0x80)
		{
			if (crouching == true){
				moveBackForward -= speedTime/8;
			}
			else if (running == true){
				moveBackForward -= speedTime*4;
			}
			else{
				moveBackForward -= speedTime;
			}
		}

		if (crouching == true) {
			camHeight = 1.0;
			maxspeedTime = speedTime / 8;
		}
		else if (running == true) {
			camHeight = 1.75;
			maxspeedTime = speedTime * 8;
		}
		else {
			camHeight = 1.75;
			maxspeedTime = speedTime;
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

		double3 terrainPos = normalize(camPos);
		terrainPos.x *= maxLength;
		terrainPos.y *= maxLength;
		terrainPos.z *= maxLength;

		terrainPoint.generateTerrainPoint(normalize(camPos));


		terrainPos.x += terrainPoint.terrain.x;
		terrainPos.y += terrainPoint.terrain.y;
		terrainPos.z += terrainPoint.terrain.z;

		if (vLength(camPos) < vLength(terrainPos) + camHeight) {
			camPos = terrainPos;
			double3 temp = normalize(camPos);
			camPos.x += temp.x * camHeight;
			camPos.y += temp.y * camHeight;
			camPos.z += temp.z * camHeight;
		}

		playerVelocity = vLength(double3(camPos.x - lastCamPos.x, camPos.y - lastCamPos.y, camPos.z - lastCamPos.z))/time;

		///////////////////
		//camPos.y = 50.0f;//
		///////////////////

		UpdateCameraPosition();


		lastCamPos = camPos;
	}

	return;
}

void DetectInput(double time)
{
	DetectMouseInput();

	DetectKeyboardInput(time);
}




#endif