#include "md5_file.h"

const unsigned long k_md5[64] =
{ 
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8, 
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

const int s[64] = {
7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

unsigned long rol( unsigned long a, int n)
{
	return ((a<<n)|(a>>(32-n)));
}

long FileSize(const char* filename)//获取文件大小，调用此函数时此文件必须处于未打开读写状态
{
	FILE *fp = NULL;
	errno_t err;
	err = fopen_s(&fp, filename, "rb");
	if (err != 0)
	{
		return -1;//文件打开失败
	}
    fseek(fp,0,SEEK_SET);
    fseek(fp,0,SEEK_END);
    long filesize=ftell(fp);
	rewind(fp);
	fclose(fp);
	return filesize;
}


void Fill( int count, unsigned long bit_size, unsigned char buffer[1088])
{
	if (count%64 < 56)
	{
		buffer[count ++] = 128;
		while( count%64 < 56)
		{
			buffer[count] = 0;
			count ++;
		}
	}
	else
	{
		if(56 <= count%64 )
		{
			buffer[count ++] = 128;
			
			for( int i = 120 - count%64; i > 0; i --) 
			{
				buffer[count] = 0;
				count ++;
			}

		}
	}			

	for( int i = count; i < count + 8; i ++)
	{
		buffer[i] = bit_size & 0xff;
		bit_size >>= 8;
	}
}


void GetM(unsigned long M[], int begin, unsigned char buffer[1088])
{
	for( int i = 0; i < 16; i ++)
	{
		for( int j = 3; j >= 0; j --)     
		{
			M[i] <<= 8;
			M[i] |= buffer[begin + i*4 + j];
		}
	}
}


unsigned long md5_file_calc(const char* filename, unsigned char md5[16])//计算filename文件的MD5值，存储到md5数组中(128位/8=16byte)，返回值=文件大小
{

	//检测filename文件是否存在
	errno_t err_read;
	FILE *fp = NULL;
	err_read = fopen_s(&fp, filename, "rb");
	if (err_read != 0)
	{
		return 0;//文件打开失败
	}
	fclose(fp);

	unsigned int fread_time = 0, round = 0, count = 0, begin = 0;
	unsigned long f_size;
	unsigned long AA, BB, CC, DD;
	unsigned long M[16];
	unsigned char buffer[1088];
	unsigned long h[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
	
	f_size = FileSize(filename);

	fread_time = f_size/1024 + 1;
	
	unsigned long f, g, temp32;	

	err_read = fopen_s(&fp, filename, "rb");
	if (err_read != 0)
	{
		return 0;//文件打开失败
	}

	for( int k = 0; k < fread_time; k ++)
	{
		count = fread( &buffer, sizeof( char), 1024, fp);
		
		if( count%64 < 56) 
		{
			if( count%64 == 0)
			{
				round = count/64;
			}
			else round = count/64 + 1;
		}
		else
		{
			round = count/64 + 2;
		}
		
		if( (count == 1024 && feof( fp)) || count < 1024)
		{
			Fill( count, f_size*8, buffer);
		}

		begin = 0;
		
		for(int i = 1; i <= round; i ++)
		{
			AA = h[0];
			BB = h[1];
			CC = h[2];
			DD = h[3];

			GetM( M, begin, buffer);
			begin += 64;

			for( int j = 0; j < 64; j ++)
			{
				if( 0 <= j && j <= 15)
				{
					f = (BB & CC) | ((~BB) & DD);
					g = j;
				}
				if( 16 <= j && j <= 31)
				{
					f = (DD & BB) | ((~DD) & CC);
					g = (5*j + 1)%16;
				}
				if( 32 <= j && j <= 47)
				{
					f = BB ^ CC ^ DD;
					g = (3*j + 5)%16;
				}
				if( 48 <= j && j <= 63)
				{
					f = CC ^ (BB | (~DD));
					g = (7*j)%16;
				}

				temp32 = DD;
				DD = CC;
				CC = BB;
			
				unsigned temp = AA + f + M[g] + k_md5[j];
				BB = rol( temp, s[j]) + BB;
				AA = temp32;
			}
			
			h[0] += AA;
			h[1] += BB;
			h[2] += CC;
			h[3] += DD;
		}
	}
	
	for( int i = 0; i < 4; i ++)
	{
		for( int j = 0; j < 4; j ++)
		{
			md5[i*4 + j] = (h[i]>>((j)*8)) & 0xff;
		}
	}
	fclose(fp);
	return f_size;
}

void md5_file_print(const unsigned char md5[16])//显示计算的文件的md5的值
{
	printf("md5:");
	for (int i = 0; i < 16; i++)
	{
		printf("%02x", md5[i]);
	}
	printf("\n");
}