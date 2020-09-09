#include "PythonExecutor.h"

std::thread             PythonExecutor::mainThread;
std::queue<PythonEvent> PythonExecutor::eventQueue;
int                     PythonExecutor::count;
std::map<int, PREVIOUS_STAGE>  PythonExecutor::enabledStageMap;
void PythonExecutor::run()
{
	count = PythonModuleManager::getModuleCount();
	try
	{
		//start
		doEnableStageInit();
		doExecuteStart();

		//update
		while (true)
		{
			doEnabledStageCheck();
			doExecuteUpdate();
			doDestory();
		}
	}
	catch (boost::python::error_already_set e)
	{
		PyErr_Print();
	}
}

void PythonExecutor::init()
{
	mainThread = std::thread(&PythonExecutor::run);
	mainThread.detach();
}

void PythonExecutor::shutDown()
{

}
