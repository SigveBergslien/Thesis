#include "../Framework/Thread.h"
#include "LoadingRenderer.h"
#include "../Framework/Window.h"
#include "../Framework/HeightMap.h"
class LoadingThread : public Thread
{
public:
	LoadingThread(LoadingRenderer& lr,	Window& win) : w(win), loadingRenderer(lr){};
	~LoadingThread(){};
	virtual void run();
	void stop();
	void initialise();
private:
	bool running;
	LoadingRenderer& loadingRenderer;
	Window& w;

};



