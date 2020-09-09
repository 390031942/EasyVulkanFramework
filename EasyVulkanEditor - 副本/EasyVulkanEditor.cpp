#include "EasyVulkanEditor.h"

EasyVulkanEditor::EasyVulkanEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);


	Py_Initialize();
	// 初始化线程支持  
	PyEval_InitThreads();
	// 启动子线程前执行，为了释放PyEval_InitThreads获得的全局锁，否则子线程可能无法获取到全局锁。  
	//PyEval_ReleaseThread(PyThreadState_Get());
	if (Py_IsInitialized())
	{
		try
		{
			EasyVulkanEditorUtility_PTR ptr(new EasyVulkanEditorUtility);

			//			register_ptr_to_python<testClass_PTR>();
			PyObject* pModule1 = PyImport_ImportModule("EasyVulkanEditor");

			PyRun_SimpleString("import sys");
			PyRun_SimpleString("sys.path.append('./')");

			
			object module(handle<>(borrowed(PyImport_AddModule("__main__"))));
			object dictionary = module.attr("__dict__");
			dictionary["editorKernal"] = ptr;


			object module1 = import("EasyEditor");
			object obj_dict = module1.attr("__dict__");

			module1.attr("__dict__")["editorKernal"] = ptr;
			exec_file("C:\\Users\\39003\\source\\repos\\EasyVulkanFramework\\x64\\Debug\\EditorDemo.py", dictionary, dictionary);

			std::string a = extract<std::string>(module1.attr("__dict__")["tstr"]);
			MenuItemMap = extract<dict>(module1.attr("__dict__")["MenuItemMap"]);

			auto klist = MenuItemMap.keys();
			auto vlist = MenuItemMap.values();

			for (int i = 0; i < len(klist); i++)
			{
				QAction* action = new QAction;
				std::string key = extract<std::string>(klist[i]);
				//std::string value = extract<std::string>(vlist[0]);

				auto values = split(key, "/");
				QMenu* menu = new QMenu(values[0].c_str());
				menu->setTitle(values[0].c_str());
				action->setText(values[1].c_str());
				action->setObjectName(key.c_str());
				menuBar()->addMenu(menu);
				menu->addAction(action);

				addActionOp(action);
			}

		}
		catch (error_already_set e)
		{
			PyObject* type = NULL, * value = NULL, * traceback = NULL;

			PyErr_Fetch(&type, &value, &traceback);

			if (type)
			{
				std::cout << PyExceptionClass_Name(type) << ": ";
			}

			if (value)
			{
				PyObject* line = PyObject_Str(value);
				if (line && (PyUnicode_Check(line)))
					std::cout << PyUnicode_1BYTE_DATA(line);
			}

			std::cout << std::endl;

			if (traceback)
			{
				for (PyTracebackObject* tb = (PyTracebackObject*)traceback;
					NULL != tb;
					tb = tb->tb_next)
				{
					PyObject* line = PyUnicode_FromFormat("  File \"%U\", line %d, in %U\n",
						tb->tb_frame->f_code->co_filename,
						tb->tb_lineno,
						tb->tb_frame->f_code->co_name);
					std::cout << PyUnicode_1BYTE_DATA(line) << std::endl;
				}
			}
		}
	}
}
