#include "EditorUtility.h"

_Singleton_impl_(EditorUtility);
_uuid_impl_(EditorUtility,"e61511cb-0aed-4b23-be8d-e9717f76f5c4");

void EditorUtility::quit()
{
	exit(0);
}
void EditorUtility::messageBox(std::string title, std::string msg)
{
	MessageBox(NULL, string2wstring(msg).c_str(), string2wstring(title).c_str(), MB_OK);
}
