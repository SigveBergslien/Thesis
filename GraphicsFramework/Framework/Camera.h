#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "HeightMap.h"
#include <iostream>
#include <fstream>

#define MIN_SPEED 2.0f
#define MAX_SPEED_CAM 40.0f

#define HEIGHT_FROM_GROUND 15.0f
using namespace std;
class Camera
{
public:
	Camera(void)
	{
		yaw = 0.0f;
		pitch = 0.0f;
		speed = 1.0f;
		width = 0.0f;
		height = 0.0f;
		viewMode = false;
		freeView = false;
		viewModeSwitched = false;
		invertYEnabled = false;
		firstPerson = false;
		sinval = 0;
		defaultFPHeight = 0;
	}

	Camera(float pitch, float yaw, Vector3 position, float width, float height,bool replaying, float speed = 1.6f, bool viewMode = false, bool freeView = false)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->speed = speed;
		this->width = width;
		this->height = height;
		this->viewMode = viewMode;
		this->freeView = freeView;
		this->viewModeSwitched = false;
		this->invertYEnabled = false;
		this->firstPerson = false;
		this->sinval = 0;
		this->defaultFPHeight;
		replay=replaying;
		if(!replay){
		ofstream kbreplayFile;
		kbreplayFile.open ("kbReplay.txt");
		if(kbreplayFile.is_open())
			kbreplayFile<<"Keyboard Replay\n";
		kbreplayFile.close();
		}
	}

	~Camera(void) {
		//replayFile.close();
	}

	Vector3 GetPosition() const		{ return position; }
	void SetPosition(Vector3 pos)	{ position = pos; }

	float GetYaw() const			{ return yaw; }
	void SetYaw(float y)			{ yaw = y; }

	float GetPitch() const			{ return pitch; }
	void SetPitch(float p)			{ pitch = p; }

	float GetSpeed() const			{ return speed; }
	void SetSpeed(float s)			{ speed = s; }

	void SetDefaultFPHeight(float h) { defaultFPHeight = h; }

	void UpdateCamera(int msPast,float msec = 10.0f, Vector3 groundPos = Vector3(0.0f, 0.0f, 0.0f));
	void UpdateCameraFP(int msPast,float msec = 10.0f, Vector3 groundPos = Vector3(0.0, 0.0f, 0.0f));

	void setHeightMap(HeightMap* hm) { this->hm = hm; } 

	Matrix4 BuildViewMatrix();
	Matrix4 BuildCameraMatrix();

	void toggleViewMode();
	void toggleFreeView();
	void toggleInvertY();
	void toggleFirstPerson();

	void toggleReplay(){replay = !replay;}

protected:
	Vector3 position;		// Set to 0 ,0 ,0 by Vector3 constructor
	float yaw;
	float pitch;
	float speed;
	float width;
	float height;
	float sinval;
	float defaultFPHeight;

	bool viewMode;
	bool freeView;
	bool viewModeSwitched;
	bool invertYEnabled;
	bool firstPerson;

	Vector3 oldPosition;
	HeightMap* hm;

	//replay
	//ofstream replayFile;
	bool replay;
};