#include "RebotPlayer.h"
#include "RebotManager.h"
#include <map>

using namespace std;

int main(int argc, char ** argv)
{
	CRebotManager::getInstance()->startServer();
	CRebotManager::destroy();
	return 0;
}





