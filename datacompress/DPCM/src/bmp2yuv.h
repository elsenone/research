#ifndef __BMP2YUV_H__
#define __BMP2YUV_H__

#pragma once
#include <windows.h>
#include "bmp2yuvCommon.h"

typedef unsigned char u_char;

void Initialize(BMPImage* pBMP, PCMImage* pPCM);
void Uninitialize(BMPImage* pBMP, PCMImage* pPCM);

BOOL ReadBMP(const char* szPath, BMPImage* pBMP);
BOOL CreatePCMImage(const BMPImage* pBMP, PCMImage* pPCM);
BOOL WritePCM(const char* szPath, const PCMImage* pPCM);
BOOL WritePCM_back(const char* szPath, const PCMImage* pPCM);

BOOL Create1bitsPCMImage(const BMPImage* pBMP, PCMImage* pPCM);
BOOL Create2bitsPCMImage(const BMPImage* pBMP, PCMImage* pPCM);

#endif