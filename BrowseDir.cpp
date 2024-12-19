#include <stdlib.h>
#include "direct.h"
#include "string.h"
#include "io.h"
#include <stdio.h>
#include <iostream>
using namespace std;
class CBrowseDir
{
protected:
	
	char m_szInitDir[_MAX_PATH];
public:
	
	CBrowseDir();

	bool SetInitDir(const char *dir);
	
	bool BeginBrowse(const char *filespec);

protected:
	
	bool BrowseDir(const char *dir,const char *filespec);
	
	virtual bool ProcessFile(const char *filename);

	virtual void ProcessDir(const char *currentdir,const char *parentdir);
};

CBrowseDir::CBrowseDir()
{
	getcwd(m_szInitDir,_MAX_PATH);
	int len=strlen(m_szInitDir);
	if (m_szInitDir[len-1] != '\\')
		strcat(m_szInitDir,"\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{
	if (_fullpath(m_szInitDir,dir,_MAX_PATH) == NULL)
		return false;
	if (_chdir(m_szInitDir) != 0)
		return false;
	int len=strlen(m_szInitDir);
	if (m_szInitDir[len-1] != '\\')
		strcat(m_szInitDir,"\\");
	return true;
}

bool CBrowseDir::BeginBrowse(const char *filespec)
{
	ProcessDir(m_szInitDir,NULL);
	return BrowseDir(m_szInitDir,filespec);
}

bool CBrowseDir::BrowseDir(const char *dir,const char *filespec)
{
	_chdir(dir);
	long hFile;
	_finddata_t fileinfo;
	if ((hFile=_findfirst(filespec,&fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy(filename,dir);
				strcat(filename,fileinfo.name);
				cout << filename << endl;
				if (!ProcessFile(filename))
					return false;
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
}

	_chdir(dir);
	if ((hFile=_findfirst("*.*",&fileinfo)) != -1)
	{
		do
		{

			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,"..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy(subdir,dir);
					strcat(subdir,fileinfo.name);
					strcat(subdir,"\\");
					ProcessDir(subdir,dir);
					if (!BrowseDir(subdir,filespec))
						return false;
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

bool CBrowseDir::ProcessFile(const char *filename)
{
	return true;
}

void CBrowseDir::ProcessDir(const char *currentdir,const char *parentdir)
{
}


class CStatDir:public CBrowseDir
{
protected:
	int m_nFileCount; 
	int m_nSubdirCount; 
public:

	CStatDir()
	{

		m_nFileCount=m_nSubdirCount=0;
	}
	int GetFileCount()
	{
		return m_nFileCount;
	}


	int GetSubdirCount()
	{
		return m_nSubdirCount-1;
	}

protected:

	virtual bool ProcessFile(const char *filename)
	{
		m_nFileCount++;
		return CBrowseDir::ProcessFile(filename);
	}
	
	virtual void ProcessDir(const char *currentdir,const char *parentdir)
	{
		m_nSubdirCount++;
		CBrowseDir::ProcessDir(currentdir,parentdir);
	}
};

void main()
{
	//获取目录名
	char buf[256];
	printf("Please input the directory name:\n");
	gets(buf);
	freopen("directory.txt","w",stdout);

	//构造类对象
	CStatDir statdir;
	//设置要遍历的目录
	if (!statdir.SetInitDir(buf))
	{
		puts("The directory doesn't exist!");
		return;
	}
	//开始遍历
	statdir.BeginBrowse("*.*");
	// printf("文件总数: %d\n子目录总数:%d\n",statdir.GetFileCount(),statdir.GetSubdirCount());
}


