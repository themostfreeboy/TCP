#pragma once

#ifndef __MD5_FILE_H
#define __MD5_FILE_H

#include <stdio.h>
#include <math.h>
#include <string.h>

unsigned long rol( unsigned long a, int n);
long FileSize(const char* filename);//获取文件大小，调用此函数时此文件必须处于未打开读写状态
void Fill( int count, unsigned long bit_size, unsigned char buffer[1088]);
void GetM(unsigned long M[], int begin, unsigned char buffer[1088]);
unsigned long md5_file_calc(const char* filename, unsigned char md5[16]);//计算filename文件的MD5值，存储到md5数组中(128位/8=16byte)，返回值=文件大小
void md5_file_print(const unsigned char md5[16]);//显示计算的文件的md5的值

#endif
