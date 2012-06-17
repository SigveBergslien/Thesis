#include "LoadingThread.h"



void LoadingThread::stop()
{
	running = false;
}

void LoadingThread::initialise()
{
	
	running = true;
}

void LoadingThread::run()
{
	/*w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	if (loadingRenderer.getLoadingScreen())
	while(w.UpdateWindow() && running)
	{
		
		loadingRenderer.UpdateScene(w.GetTimer()->GetTimedMS());
		loadingRenderer.RenderScene();
	}

	else
	while(w.UpdateWindow() && loadingRenderer.getLoadingStatus())
	{		
		loadingRenderer.UpdateScene(w.GetTimer()->GetTimedMS());
		loadingRenderer.RenderScene();
	}*/
	

}