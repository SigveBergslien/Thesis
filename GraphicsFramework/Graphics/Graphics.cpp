#pragma comment(lib, "Framework.lib")
//Audio-specific library includes. 

#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")

#include <iostream>
#include <fstream>
#include "../Framework/window.h"
#include "../Framework/PhysicsThread.h"
#include "../Framework/Thread.h"
#include "../Framework/SoundSystem.h"
#include "../Framework/Sound.h"
#include "LoadingRenderer.h"
#include "LoadingThread.h"
#include "MenuRenderer.h"
#include "Renderer.h"

int main(bool a)
{	
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow( hWnd, SW_HIDE );
	
	Window w("Herding game", 720, 480, false);

		if(!w.HasInitialised())
		{
			return -1;
		}
		//int seed = (unsigned int)w.GetTimer()->GetMS() * (unsigned)1000;
		//srand(seed);
		//

		////for replay
		//ofstream msreplayFile;
		//msreplayFile.open ("seeds.txt");
		//if(msreplayFile.is_open())
		//	msreplayFile<<seed<<"_\n";
		//msreplayFile.close();

		w.LockMouseToWindow(true);
		w.ShowOSPointer(false);
		bool LoadingScreen = false; 
	
	//------------------------------------
	//uncomment the following to show menu
	//------------------------------------
	LoadingRenderer loadingRendererA(w, LoadingScreen);
	LoadingThread loadingThreadA(loadingRendererA, w);
	loadingThreadA.initialise();
	loadingThreadA.start();
	MenuRenderer menuRenderer(w);
	loadingThreadA.stop();
	loadingThreadA.join();	

	while(w.UpdateWindow() && !menuRenderer.start)
	{
		if (menuRenderer.exitFlag)
			return 0;
		menuRenderer.RenderScene();
		menuRenderer.UpdateScene(w.GetTimer()->GetTimedMS());
	}
	//end menu
	
	LoadingScreen = true;
	
	LoadingRenderer loadingRendererB(w, LoadingScreen);
	LoadingThread loadingThreadB(loadingRendererB, w);
	
	loadingThreadB.initialise();
	loadingThreadB.start();
	
	Renderer renderer(w);
	
	if(!renderer.HasInitialised())
	{
		return -1;
	}
	renderer.setEndGame(false);
	loadingThreadB.stop();
	loadingThreadB.join();
	
	unsigned int fps = 0;
	unsigned long int startTime = (unsigned long int)time(NULL);
	unsigned long int nowTime = (unsigned long int)time(NULL);	
	
	while(w.UpdateWindow() && !renderer.getGameOver())
	{		
		int msec = w.GetTimer()->GetMS();
		if (renderer.getEndGame())
		{
			
			renderer.setEndSceneOver(true);
			renderer.setEndGame(false);
			renderer.setPause(false);
		}
		// Toggle view mode
		/*if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C))
		{
			renderer.toggleViewMode();
		}*/

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
		{
			renderer.setReplay();
		}

		// Toggle free view
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F))
		{
			renderer.toggleFreeView();
		}

		// Toggle bump mapping
		/*if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
		{
			renderer.toggleBumpmapping();
		}*/

		//pause
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_ESCAPE))
		{
			renderer.togglePause();
		}

		//This is how to trigger sounds
		/*if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_Z))
		{
			renderer.toggleSound(); 
		}*/

		renderer.SetCurrentTime(msec);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
		fps++;

		nowTime = (unsigned long int)time(NULL);
		if((nowTime - startTime) >= 1)
		{
			renderer.SetFPS(fps / (nowTime - startTime));
			startTime = (unsigned long int)time(NULL);
			fps = 0;
		}

	}	
	return 0;
}