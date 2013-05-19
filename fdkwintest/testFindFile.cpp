#ifndef __TESTFINDFILE_H
#define __TESTFINDFILE_H
#include <iostream>
#include <fdkwin/FindFile.h>

class MyFileFinderHandler
	: public fdkwin::FindFileHandler
{
public:
	CString m_path;

	virtual void handleFile(const WIN32_FIND_DATA& findData)
	{
		if (fdkwin::isDirectory(findData))
		{	
			CString fullFile = m_path + "\\" + findData.cFileName;
			std::wcout << fullFile.GetString() << std::endl;
			MyFileFinderHandler myHandler;
			myHandler.m_path = fullFile;
			fdkwin::findFile(fullFile + _T("\\*.txt"), myHandler);
		}
		else
		{
			CString fullFile = m_path + "\\" + findData.cFileName;
			std::wcout << fullFile.GetString() << std::endl;
		}		
	}
};

void testFindFile()
{
	MyFileFinderHandler myHandler;
	myHandler.m_path = _T("C:\\Windows\\System32");
	fdkwin::findFile(myHandler.m_path + _T("\\*.txt"), myHandler);
}

#endif
