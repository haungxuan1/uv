﻿// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

CPQPrinter thePQPrinter;

Camera::CCamera thePQCamera;

CPQPrinter& PQPrinter()
{
	return thePQPrinter;
}

Camera::CCamera& PQCamera()
{
	return thePQCamera;
}

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
