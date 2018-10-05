#include<stdio.h>
#include<jpeglib.h>
#include<typeinfo>
#include<stdlib.h>

struct img{
	int width;
	int height;
	unsigned char **image;
};

img read_jpeg(char *filename);
