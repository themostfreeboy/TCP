#pragma once

#ifndef __MD5_FILE_H
#define __MD5_FILE_H

#include <stdio.h>
#include <math.h>
#include <string.h>

unsigned long rol( unsigned long a, int n);
long FileSize(const char* filename);//��ȡ�ļ���С�����ô˺���ʱ���ļ����봦��δ�򿪶�д״̬
void Fill( int count, unsigned long bit_size, unsigned char buffer[1088]);
void GetM(unsigned long M[], int begin, unsigned char buffer[1088]);
unsigned long md5_file_calc(const char* filename, unsigned char md5[16]);//����filename�ļ���MD5ֵ���洢��md5������(128λ/8=16byte)������ֵ=�ļ���С
void md5_file_print(const unsigned char md5[16]);//��ʾ������ļ���md5��ֵ

#endif
