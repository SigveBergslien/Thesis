#include "Camera.h"

void Camera::UpdateCamera(int msPast,float msec, Vector3 groundPos)
{
	// clamp msec value
	msec = min(msec, MIN_SPEED);

	// get values
	speed += (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS));
	speed -= (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS));
	
	if (viewMode || freeView)
	{
		if(!invertYEnabled) pitch += Window::GetMouse()->GetRelativePosition().y;
		else  pitch -= Window::GetMouse()->GetRelativePosition().y;

		yaw += Window::GetMouse()->GetRelativePosition().x;
	}
	else
	{
		if ( Window::GetMouse()->ButtonHeld(MOUSE_MIDDLE) )
		{
			yaw += Window::GetMouse()->GetRelativePosition().x;
		}
		yaw += (Window::GetKeyboard()->KeyDown(KEYBOARD_E) * 2.0f);
		yaw -= (Window::GetKeyboard()->KeyDown(KEYBOARD_Q) * 2.0f);
	}

	// clamp values
	speed = min(speed, MAX_SPEED_CAM);
	speed = max(speed, MIN_SPEED);

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}

	// calculate position
	if (viewMode || freeView)		// First person view mode
	{
		if ( Window::GetKeyboard()->KeyDown(KEYBOARD_W) )
		{
			position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec * speed;
		}
		if ( Window::GetKeyboard()->KeyDown(KEYBOARD_S) )
		{
			position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec * speed;
		}

		if ( Window::GetKeyboard()->KeyDown(KEYBOARD_A) )
		{
			position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec * speed;
		}
		if ( Window::GetKeyboard()->KeyDown(KEYBOARD_D) )
		{
			position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec * speed;
		}

		if (!freeView)
		{
			// interpolate and set y axis value
			float deltaY = fabs(groundPos.y) - fabs(position.y);
			position.y = groundPos.y + HEIGHT_FROM_GROUND - (deltaY / 2.0f);
		}

	}
	else	// Strategic view mode
	{
		if ( !Window::GetMouse()->ButtonHeld(MOUSE_MIDDLE) )
			{
			if ( Window::GetKeyboard()->KeyDown(KEYBOARD_W) || Window::GetMouse()->GetAbsolutePosition().y == 0.0f )
			{
				position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec * speed;
			}
			if ( Window::GetKeyboard()->KeyDown(KEYBOARD_S) || Window::GetMouse()->GetAbsolutePosition().y == height )
			{
				position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec * speed;
			}

			if ( Window::GetKeyboard()->KeyDown(KEYBOARD_A) || Window::GetMouse()->GetAbsolutePosition().x == 0.0f )
			{
				position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec * speed;
			}
			if ( Window::GetKeyboard()->KeyDown(KEYBOARD_D) || Window::GetMouse()->GetAbsolutePosition().x == width )
			{
				position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec * speed;
			}
		}
	}

	// allows free movement
	if (freeView)
	{
		if ( Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		{
			position.y += msec * speed;
		}
		if ( Window::GetKeyboard()->KeyDown (KEYBOARD_SPACE))
		{
			position.y -= msec * speed;
		}
	}

	// controls mouse wheel movement on Strategic view mode
	if (!viewMode && !freeView && !firstPerson)
	{
		pitch -= (Window::GetMouse()->GetWheelMovement());
		position.y -= (Window::GetMouse()->GetWheelMovement() * 25.0f);

		//pitch = min(35.0f, pitch);
		//pitch = max(25.0f, pitch);
		//
		//position.y = min(500.0f, position.y);
		//position.y = max(250.0f, position.y);
	}

	// set default values when view mode is switched
	if (viewMode && viewModeSwitched && !freeView)
	{
		viewModeSwitched = false;
	}
	else if (!viewMode && viewModeSwitched && !freeView && !firstPerson)
	{
		position.y = 300.0f;
		pitch = 50;
		yaw = 140;
		viewModeSwitched = false;
	}
	toggleInvertY();
}

void Camera::UpdateCameraFP(int msPast,float msec, Vector3 groundPos)
{
	//bool moving = false;
	//if(!replay)
	//{
	//	float tmp_x = Window::GetMouse()->GetRelativePosition().x;
	//	float tmp_y = Window::GetMouse()->GetRelativePosition().y;
	//	if(tmp_x !=0||tmp_y!=0)
	//	{
	//		ofstream msreplayFile;
	//		msreplayFile.open ("msReplay.txt",ios::app);
	//		if(msreplayFile.is_open())
	//			msreplayFile<<msPast<<"_y_"<<Window::GetMouse()->GetRelativePosition().y<<"_x_"<<Window::GetMouse()->GetRelativePosition().x<<"_ms_"<<msec<<"\n";
	//		msreplayFile.close();

	//		if(!invertYEnabled) pitch += tmp_y*2*(msec/15);
	//		else  pitch -= tmp_y*(msec/15);

	//		yaw += (tmp_x)*(msec/15);

	//		pitch = min(pitch, 90.0f);
	//		pitch = max(pitch, -90.0f);

	//		if (yaw < 0)
	//		{
	//			yaw += 360.0f;
	//		}
	//		if (yaw > 360.0f)
	//		{
	//			yaw -= 360.0f;
	//		}
	//	}
	//}
	//else{
	//	//cout<<msPast<<endl;
	//	float tmp_x = 0;
	//	float tmp_y = 0;
	//	int readMS =0;
	//	int prevReadMS =0;
	//	string line;
	//	string line2;
	//	ifstream msreplayFile ("msReplay.txt");
	//	ifstream kbreplayFile ("kbReplay.txt");
	//	if (msreplayFile.is_open()&&kbreplayFile.is_open())
	//	{
	//		bool readNext = true;
	//		while( (readNext))
	//		{
	//			tmp_x = 0;
	//			tmp_y = 0;
	//			msreplayFile.seekg(msReadPos);
	//			string tmp="";
	//			getline (msreplayFile,line);
	//			int counter=0;
	//			bool neg = false;
	//			for(int i =0;i <line.length();i++){
	//				if (counter ==0)
	//				{
	//					if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						prevReadMS = readMS;
	//						readMS = atoi(tmp.c_str());
	//						//read Keyboard
	//						bool readingkb = true;
	//						while(readingkb)
	//						{
	//							kbreplayFile.seekg(kbReadPos);
	//							string tmp2="";
	//							getline (kbreplayFile,line2);
	//							if(line2=="")
	//								readingkb = false;
	//							for(int j =0;j <line2.length();j++){
	//								if(line2[j]=='s')
	//								{
	//									readingkb=false;
	//									break;
	//								}
	//								if(line2[j]!='_')
	//								{
	//									tmp2+=line2[j];
	//								}
	//								else
	//								{
	//									int kbreadMS = atoi(tmp2.c_str());
	//									if(kbreadMS<=msPast)
	//									{
	//										if(kbreadMS<readMS&&kbreadMS>=prevReadMS)
	//										{
	//											tmp2="";
	//											for(int k =j+1;k <line2.length();k++)
	//											{
	//												if(line2[k]!='_')
	//												{
	//													tmp2+=line2[k];
	//												}
	//												else
	//												{
	//													int tmpmsec = atoi(tmp2.c_str());
	//													cout<<tmpmsec<<endl;
	//													//cout<<msec<<"_";
	//													tmp2= line2[k+3];
	//													//cout<<tmp2<<endl;
	//													if(line2[k+3] == 'W')
	//													{
	//														position = oldPosition;
	//														position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * tmpmsec/12 * speed/2;
	//														moving = true;
	//													}
	//													else if(line2[k+3] == 'S')
	//													{
	//														position = oldPosition;
	//														position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * tmpmsec/12 * speed/2;
	//														moving = true;
	//													}
	//													else if(line2[k+3] == 'A')
	//													{
	//														position = oldPosition;
	//														position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * tmpmsec/12 * speed/2;
	//														moving = true;
	//													}
	//													else if(line2[k+3] == 'D')
	//													{
	//														position = oldPosition;
	//														position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * tmpmsec/12 * speed/2;
	//														moving = true;
	//													}
	//													else
	//														readingkb = false;
	//													kbReadPos += k+6;
	//												}
	//											}
	//										}
	//										else 
	//											readingkb = false;
	//									}
	//									else
	//										readingkb = false;
	//								}
	//							}
	//						}
	//						//finished reading Keyboard
	//						if(readMS<=msPast)
	//						{
	//							tmp="";
	//							i+=2;
	//							counter ++;
	//						}
	//						else
	//						{
	//							readNext = false;
	//							break;
	//						}
	//					}
	//				}
	//				else if(counter==1)
	//				{
	//					if(line[i]=='-')
	//					{
	//						neg = true;
	//					}
	//					else if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						tmp_y = atof(tmp.c_str());
	//						if(neg)tmp_y = -tmp_y;
	//						tmp="";
	//						i+=2;
	//						counter ++;
	//						neg = false;
	//					}
	//				}
	//				else if(counter==2)
	//				{
	//					if(line[i]=='-')
	//					{
	//						neg = true;
	//					}
	//					else if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						tmp_x = atof(tmp.c_str());
	//						if(neg)tmp_x = -tmp_x;
	//						tmp ="";
	//						for(int l = i+4; l<line.size()-1;l++)
	//						{
	//							tmp+= line[l];
	//						}
	//						//cout<<tmp<<endl;
	//						float tmpMsec = atof(tmp.c_str());
	//						cout<<"msec:"<<tmpMsec<<endl;
	//						//cout<<tmp_x<<endl;
	//						msReadPos += i+13;

	//						if(!invertYEnabled) pitch += tmp_y*2*(tmpMsec/15);
	//						else  pitch -= tmp_y*(tmpMsec/15);
	//						yaw += (tmp_x)*(tmpMsec/15);

	//						pitch = min(pitch, 90.0f);
	//						pitch = max(pitch, -90.0f);

	//						if (yaw < 0)
	//						{
	//							yaw += 360.0f;
	//						}
	//						if (yaw > 360.0f)
	//						{
	//							yaw -= 360.0f;
	//						}
	//						break;
	//					}
	//				}
	//			}
	//		}
	//	msreplayFile.close();
	//	}
	//}
	//// clamp msec value

	//// get values
	//speed += (Window::GetKeyboard()->KeyDown(KEYBOARD_PLUS));
	//speed -= (Window::GetKeyboard()->KeyDown(KEYBOARD_MINUS));

	//// clamp values
	//speed = min(speed, MAX_SPEED_CAM);
	//speed = max(speed, MIN_SPEED);

	//oldPosition = position;
	//Vector3 tmp;
	//float buffer = 40;

	//if(!replay)
	//{
	//	if ( Window::GetKeyboard()->KeyDown(KEYBOARD_W) )
	//	{	
	//		tmp = Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec/7 * speed/2;
	//		tmp.Normalise();
	//		tmp = tmp * buffer;
	//		position += tmp;
	//		if(hm->getGroundPos(position).y > 5.0f) position = oldPosition;
	//		else
	//		{
	//			ofstream kbReplayFile;
	//			kbReplayFile.open ("kbReplay.txt",ios::app);
	//			if(kbReplayFile.is_open())
	//				kbReplayFile<<msPast<<"_"<<msec<< "_kbW\n";
	//			kbReplayFile.close();

	//			position = oldPosition;
	//			position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec/12 * speed/2;
	//			moving = true;
	//		}
	//	}
	//	if ( Window::GetKeyboard()->KeyDown(KEYBOARD_S) )
	//	{
	//		tmp = Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec/12 * speed/2;
	//		tmp.Normalise();
	//		tmp = tmp * buffer;
	//		position -= tmp;

	//		if(hm->getGroundPos(position).y > 5.0f) position = oldPosition;
	//		else
	//		{
	//			ofstream kbReplayFile;
	//			kbReplayFile.open ("kbReplay.txt",ios::app);
	//			if(kbReplayFile.is_open())
	//				kbReplayFile<<msPast<<"_"<<msec<< "_kbS\n";
	//			kbReplayFile.close();

	//			position = oldPosition;
	//			position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(0.0f, 0.0f, -1.0f) * msec/12 * speed/2;
	//			moving = true;
	//		}
	//	}

	//	if ( Window::GetKeyboard()->KeyDown(KEYBOARD_A) )
	//	{
	//		tmp = Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec/12 * speed/2;
	//		tmp.Normalise();
	//		tmp = tmp * buffer;
	//		position += tmp;

	//		if(hm->getGroundPos(position).y > 5.0f) position = oldPosition;
	//		else
	//		{
	//			ofstream kbReplayFile;
	//			kbReplayFile.open ("kbReplay.txt",ios::app);
	//			if(kbReplayFile.is_open())
	//				kbReplayFile<<msPast<< "_"<<msec<<"_kbA\n";
	//			kbReplayFile.close();

	//			position = oldPosition;
	//			position += Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec/12 * speed/2;
	//			moving = true;
	//		}
	//	}
	//	if ( Window::GetKeyboard()->KeyDown(KEYBOARD_D) )
	//	{
	//		tmp = Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec/12 * speed/2;
	//		tmp.Normalise();
	//		tmp = tmp * buffer;
	//		position -= tmp;

	//		if(hm->getGroundPos(position).y > 5.0f) position = oldPosition;
	//		else
	//		{
	//			ofstream kbReplayFile;
	//			kbReplayFile.open ("kbReplay.txt",ios::app);
	//			if(kbReplayFile.is_open())
	//				kbReplayFile<<msPast<<"_"<<msec<< "_kbD\n";
	//			kbReplayFile.close();
	//			position = oldPosition;
	//			position -= Matrix4::Rotation(-yaw, Vector3(0.0f, 1.0f, 0.0f)) * Vector3(-1.0f, 0.0f, 0.0f) * msec/12 * speed/2;
	//			moving = true;
	//		}
	//	}
	//}

	//if(moving)
	//{
	//	sinval += 0.1f;

	//	(sinval >= 360) ? sinval = 0.0f : sinval = sinval;

	//	position.y += (sin(sinval + 0.1f)) * 0.25f;
	//}
	//else
	//{
	//	position.y = defaultFPHeight;
	//}
	//toggleInvertY();
	//Window::GetMouse()->SetAbsolutePosition(width/2, height/2);
}


Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(-1, 0, 0)) *
		   Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		   Matrix4::Translation(-position);
};

Matrix4 Camera::BuildCameraMatrix()
{
		return Matrix4::Translation(position) *
		   Matrix4::Rotation(yaw, Vector3(0, -1, 0));
};

void Camera::toggleViewMode()
{
	viewMode = !viewMode;
	viewModeSwitched = true;
};

void Camera::toggleFreeView()
{
	freeView = !freeView;
};

void Camera::toggleInvertY()
{
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F2))
	{
		invertYEnabled = !invertYEnabled;
	}
}

void Camera::toggleFirstPerson()
{
	firstPerson = !firstPerson;
	(firstPerson) ? freeView = true : freeView = false;
	if(!firstPerson)
	{
		position.y = 300.0f;
		position.x = position.x - 130;
		position.z = position.z - 130;
		pitch = 50;
		yaw = 140;
	}
	else
	{
		Window::GetMouse()->SetAbsolutePosition(width/2, height/2);
	}
}
