#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <typeinfo>
#include <string.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <limits>
#include <cfloat>
#include "image.hpp"

//Function to check if a number is a power of 2.
//Returns 1 if it is.
int ispow2(int num)
{
	if((num & (num-1)) == 0)
		return 1;
	return 0;
}

/*
	 This computes an in-place complex-to-complex FFT 
	 x and y are the real and imaginary arrays of 2^m points.
	 dir =  1 gives forward transform
	 dir = -1 gives reverse transform 
*/
void FFT(int dir, int m, double *x, double *y)
{
	 long nn,i,i1,j,k,i2,l,l1,l2;
	 double c1,c2,tx,ty,t1,t2,u1,u2,z;
	 
	 nn = 1;
	 for (i = 0; i < m; i++)
		 nn *= 2;
	 
	 /* Do the bit reversal */
	 i2 = nn >> 1;
	 j = 0;
	 for (i = 0; i < nn-1; i++) 
	 {
		 if (i < j) 
		 {
			 tx = x[i];
			 ty = y[i];
			 x[i] = x[j];
			 y[i] = y[j];
			 x[j] = tx;
			 y[j] = ty;
		 }
		 k = i2;
		 while (k <= j) 
		 {
			 j -= k;
			 k >>= 1;
		 }
		 j += k;
	 }

	 /* Compute the FFT */
	 c1 = -1.0;
	 c2 = 0.0;
	 l2 = 1;
	 for (l = 0; l < m; l++) 
	 {
		 l1 = l2;
		 l2 <<= 1;
		 u1 = 1.0;
		 u2 = 0.0;
		 for (j = 0; j < l1; j++) 
		 {
			 for (i = j; i < nn; i+=l2) 
			 {
				 i1 = i + l1;
				 t1 = u1 * x[i1] - u2 * y[i1];
				 t2 = u1 * y[i1] + u2 * x[i1];
				 x[i1] = x[i] - t1;
				 y[i1] = y[i] - t2;
				 x[i] += t1;
				 y[i] += t2;
				 if(isnan(x[i1]))
					x[i1] = 0.0;
				if(isnan(y[i1]))
					y[i1] = 0.0;
				if(isnan(x[i]))
					x[i] = 0.0;
				if(isnan(y[i]))
					y[i] = 0.0;
			 }
			 z =  u1 * c1 - u2 * c2;
			 u2 = u1 * c2 + u2 * c1;
			 u1 = z;
		 }
		 c2 = sqrt((1.0 - c1) / 2.0);
		 
		 if (dir == 1)
			 c2 = -c2;
		 c1 = sqrt((1.0 + c1) / 2.0);
	
	 }

	 /* Scaling for forward transform */
	 if (dir == 1) 
	 {
		 for (i = 0; i<nn; i++) 
		 {
			 x[i] /= (double)nn;
			 y[i] /= (double)nn;
		 }
	 }
}

//Function to do 2D FFT inplace given real and imaginary part arrays
//dir same as that for FFT above.
void doTransform(int dir, double *reals, double *imags, int width, int height, int m_row, int m_col)
{
	double *realrow = new double[width];
	double *imagrow = new double[width];
	int i, j;

	for(i=0; i<height; ++i)
	{
		for(j=0; j<width; ++j)
		{
			 realrow[j] = reals[j+i*width];
			 imagrow[j] = imags[j+i*width];
		}
		FFT(dir, m_row, realrow, imagrow);
		for(j=0; j<width; ++j)
		{
			 reals[j+i*width] = realrow[j];
			 imags[j+i*width] = imagrow[j];
		}
	}
	delete[] realrow;
	delete[] imagrow;
	// printf("Finished row transform\n");

	double *realcol = new double[height];
	double *imagcol = new double[height];

	for(i=0; i<width; ++i)
	{
		for(j=0; j<height; ++j)
		{
			 realcol[j] = reals[i+j*width];
			 imagcol[j] = imags[i+j*width];
		}
		FFT(dir, m_col, realrow, imagrow);
		for(j=0; j<height; ++j)
		{
			 reals[i+j*width] = realcol[j];
			 imags[i+j*width] = imagcol[j];
		}
	}
	delete[] realcol;
	delete[] imagcol;
	// printf("Finished column transform\n");
	 
}

//Function to make the double array after the forward transform to an Image object(unsigned char array) that can be saved as a jpeg
Image convertForwardTransform(double *mat, int size, int width, int height, double min, double max)
{
	
	int i1=0,j1=0,i,j;
	unsigned char *temp = new unsigned char[size];
	double mid = (max + min)/2.0;
	for(i=0; i<size; ++i)
	{
		temp[i] = floor((mat[i] - mid)*255);
		if(temp[i]<0) 
			 temp[i] = 0;
		else if(temp[i]>255)
			 temp[i] = 255;
	}

	
	unsigned char *temp2 = (unsigned char*)calloc(size*2, sizeof(unsigned char));
	for(i=0; i<height; ++i){
		for(j=0; j<width;++j){
			if(i<height/2)
				i1= height/2 - i;
			else
				i1 = (3*height)/2 - i;
			if(j<width/2)
				j1 = width/2 - j;
			else
				j1 = (3*width)/2 - j;

			temp2[i1*width+j1] = temp[i*width+j];
		}
	}
	free(temp);

	Image conv = Image(width, height, 1, temp2);
	free(temp2);
	return conv;
}

//Function to make the double array after inverse transform to an Image object(unsigned char array) that can be saved as a jpeg
Image convertInverseTransform(double *mat, int size, int width, int height, double min, double max, int orig_width, int orig_height)
{
	
	int i,j;
	unsigned char *temp = new unsigned char[orig_width*orig_height];
	
	double maximum = (abs(max)>abs(min))?abs(max):abs(min);
	for(i=0; i<orig_height; ++i)
	{
		for(j=0; j<orig_width; ++j)
		{
			temp[j+i*orig_width] = floor((abs(mat[j+i*width])*255)/maximum);
		}
	}

	Image conv = Image(orig_width, orig_height, 1, temp);
	
	free(temp);
	return conv;
}

//Function to find the minimum and maximum values for the double arrays.
//Output used in the transform to Image functions above (convertForwardTransform and convertTransform)
void findMinMax(double *mat, int size, double *min, double *max)
{
	int i;
	for(i=0; i<size; ++i)
	{
		if(mat[i] > *max)
			*max = mat[i];
		else if(mat[i] < *min)
			*min = mat[i];
	}
}

//Function to perform the 2D transform and return real and imaginary
int FFTFromImage(Image img, int m_row, int m_col, int orig_width, int orig_height)
{
	if(!ispow2(img.width) || !ispow2(img.height))
	{
		return -1;
	}
	
	if(img.components!=1)
	{
		return -2;
	}
	
	int width = img.width;
	int height = img.height;
	int size = width*height;
	double *reals = new double[size];
	double *imags = new double[size];

	int i,j;
	double c;
	for(i=0; i<size; ++i)
	{
			 c = (double)img.image[i];
			 if(c>255.0)
				c=255.0;
			else if(c<0.0)
				c=0.0;
			reals[i] = c;
	}
	
	char *path_forward_real = (char *)calloc(200, sizeof(char));
	char *path_forward_imag = (char *)calloc(200, sizeof(char));
	char *path_inverse = (char *)calloc(200, sizeof(char));
	
	// strcat(path_forward_real, "uploads/");
	// strcat(path_forward_imag, "uploads/");
	// strcat(path_inverse, "uploads/");

	// int l = strlen(path_forward_real);

	double max_real = DBL_MIN;
	double min_real = DBL_MAX;
	double max_imag = DBL_MIN;
	double min_imag = DBL_MAX;

	try
	{
		for(i=0; !(img.filename[i] == '.' && img.filename[i+1] == 'j'); ++i)
		{
			path_forward_real[i] = img.filename[i];
			path_forward_imag[i] = img.filename[i];
			path_inverse[i] = img.filename[i];
		}

		path_forward_real[i] = '\0';
		path_forward_imag[i] = '\0';
		path_inverse[i] = '\0';

		// printf("%s\n", path_forward_real);
		doTransform(1, reals, imags, width, height, m_row, m_col);

		findMinMax(reals, size, &min_real, &max_real);
		findMinMax(imags, size, &min_imag, &max_imag);

		Image realpart = convertForwardTransform(reals, size, width, height, min_real, max_real);
		const char *savename_real_forward = strcat(path_forward_real, "_forward_real.jpg");
		realpart.write_jpeg(savename_real_forward, GRAYSCALE);
		printf("%s\n", savename_real_forward);
		Image imagpart = convertForwardTransform(imags, size, width, height, min_imag, max_imag);
		const char *savename_imag_forward = strcat(path_forward_imag, "_forward_imag.jpg");
		imagpart.write_jpeg(savename_imag_forward, GRAYSCALE);
		printf("%s\n", savename_imag_forward);
		
		doTransform(-1, reals, imags, width, height, m_row, m_col);

		max_real = DBL_MIN;
		min_real = DBL_MAX;
		findMinMax(reals, size, &min_real, &max_real);
		Image realpart_inv = convertInverseTransform(reals, size, width, height, min_real, max_real, orig_width, orig_height);
		const char *savename_real_inverse = strcat(path_inverse, "_inverse_real.jpg");
		realpart_inv.write_jpeg(savename_real_inverse, GRAYSCALE);
		printf("%s\n", savename_real_inverse);
	}
	catch(int x)
	{
		return x;
	}
	
	return 0;
}


void closest_m(Image img, int &width, int &height)
{
	double val = img.width;
	double logval = log2(val);
	double intval;
	double fracval = modf(logval, &intval);
	if(intval-fracval == intval)
		width = intval;
	else
		width = intval+1;

	val = img.height;
	logval = log2(val);

	fracval = modf(logval, &intval);
	if(intval-fracval == intval)
		height = intval;
	else
		height = intval+1;
	if(width>height)
		height = width;
	else if(height>width)
		width = height;
}


Image getpow2img(Image orig, int width_exp, int height_exp)
{
	// printf("%d\t%d\n", width_exp, height_exp);

	int width = floor(pow(2,width_exp));
	int height = floor(pow(2,height_exp));

	// printf("%d\t%d\n", width, height);
	if(width == orig.width && height == orig.height)
		return orig;

	Image pow2 = Image(width, height, orig.components);
	if(orig.filename != NULL && strlen(orig.filename)>0)
	{
	pow2.filename = new char[strlen(orig.filename)];
	strcpy(pow2.filename, orig.filename);
	}

	int i,j;
	int size = width*height*orig.components, orig_width = orig.width, orig_height = orig.height;

	for(i=0; i<size; ++i)
		pow2.image[i] = 0;

	for(i=0; i<orig_height; ++i)
	for(j=0; j<orig_width; ++j)
		 pow2.image[j+i*width] = orig.image[j+i*orig_width];

	return pow2;
}

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		printf("Please enter input filename as argument.\n");
		return -1;
	}

	const char *filepath = argv[1];
	double *reals = NULL;
	double *imags = NULL;
	int dir = 1;
	Image orig_img = Image();
	try
	{
		int size = strlen(filepath);
	    orig_img.filename = new char[size];
	    strcpy(orig_img.filename, filepath);
	    orig_img.read_jpeg();
	}
	catch(int x)
	{
		return x;
	}
	int dimX=0, dimY=0;
	closest_m(orig_img, dimX, dimY);
	Image img = getpow2img(orig_img, dimX, dimY);
	int status = FFTFromImage(img, dimX, dimY, orig_img.width, orig_img.height);
	printf("%d\n", status);

	return status;
}