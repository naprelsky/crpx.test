#pragma once
#include "resource.h"
#include "Service.h"

class CCropixMFCApp : public CWinApp
{
public:
	CCropixMFCApp();

public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CCropixMFCApp mfcApp;
