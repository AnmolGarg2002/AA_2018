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
using namespace std; 

int ispow2(int num)
{
	if((num & (num-1)) == 0)
		return 1;
	return 0;
}

void _medianfilter(unsigned char* image, unsigned char* result, int N, int M,int ker)
{
	unsigned char window[ker*ker];
	int sh = ker/2;
   for (int m = sh; m < M - sh; ++m)
   {
      for (int n = sh; n < N - sh; ++n)
      {
         int k = 0;
         for (int i = m - sh; i <= m + sh; ++i){
            for (int j = n - sh; j <= n + sh; ++j){
               window[k] = (int)image[i * N + j];
               k++;
            }
         }
         std::sort(window, window+sizeof(window));
         result[m * N + n] = window[ker*ker/2];
      }
   }
}

Image median_blur(Image source, int ker){
	int width = source.width;
	int height = source.height;

	Image tar(width, height, source.components, source.image);
	_medianfilter(source.image, tar.image, width, height, ker);
	return(tar);
}

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


double* cap(double *real,int radius, int size_h,int size_w, int option)
{
	double *result = (double*)malloc(size_h*size_w*sizeof(double));
	if(option == 0)
	{
	for(int i=0;i<size_h;i++){
		for(int j=0;j<size_w;j++){
			if(pow(i,2) + pow(j,2) < pow(radius/2,2) || pow(j-size_w,2) + pow(i-size_h,2) < pow(radius/2,2) ||
				pow(j,2) + pow(i-size_h,2) < pow(radius/2,2) || pow(j-size_w,2) + pow(i,2) < pow(radius/2,2) )
			{
				result[i*size_w+j]=0.0;
			}
			else
			{
				result[i*size_w+j]=real[i*size_w+j];
			}
		}
	}
	}
	else if(option == 1)
	{
	for(int i=0;i<size_h;i++){
		for(int j=0;j<size_w;j++){
			if(pow(i,2) + pow(j,2) < pow(radius/2,2) || pow(j-size_w,2) + pow(i-size_h,2) < pow(radius/2,2) ||
				pow(j,2) + pow(i-size_h,2) < pow(radius/2,2) || pow(j-size_w,2) + pow(i,2) < pow(radius/2,2) )
			{
				result[i*size_w+j]=real[i*size_w+j];
			}
			else
			{
				result[i*size_w+j]=0.0;
			}
		}
	}
	}
	return result;
}


//Function to perform the 2D transform and return real and imaginary
Image CT_FFT2D(Image &img, int m_row, int m_col, int orig_width, int orig_height, int cap_r, int option)
{
	if(!ispow2(img.width) || !ispow2(img.height))
	{
		printf("Width or height is not a power of 2.\nAborting.\n");
		exit(1);
	}
	if(img.components!=1)
	{
		printf("Not a grayscale image.\nAborting.\n");
		exit(2);
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

	doTransform(1, reals, imags, width, height, m_row, m_col);

	reals = cap(reals, cap_r, height, width, option);
	imags = cap(imags, cap_r, height, width, option);

	double max = DBL_MIN;
	double min = DBL_MAX;
	// printf("Before :\t%lf\t%lf\t\n", max, min);
	findMinMax(reals, size, &min, &max);
	
	Image realpart = convertForwardTransform(reals, size, width, height, min, max);
	//invert(realpart,"conv_real.jpg");
	time_t timer;
    struct tm y2k = {0};
    double seconds;
    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
    time(&timer);  /* get current time; same as: timer = time(NULL)  */
    seconds = difftime(timer,mktime(&y2k));

    char result_path[50];
    sprintf (result_path, "./uploads/edge_real_%d.jpg",int(seconds));
	// realpart.display("./test_images/fourier_real.jpg", SAVE_FIRST, GRAYSCALE);
	realpart.write_jpeg(result_path, GRAYSCALE);
	printf("%s\n", result_path);

	doTransform(-1, reals, imags, width, height, m_row, m_col);
	max = DBL_MIN;
	min = DBL_MAX;
	
	findMinMax(reals, size, &min, &max);
	
	sprintf (result_path, "./uploads/edge_inv_%d.jpg",int(seconds));
	Image realpart_inv = convertInverseTransform(reals, size, width, height, min, max, orig_width, orig_height);
	// realpart_inv.display("./test_images/fourier_real_inv.jpg", SAVE_FIRST, GRAYSCALE);
	realpart_inv.write_jpeg(result_path, GRAYSCALE);
	printf("%s\n", result_path);

	return realpart_inv;

}


void closest_m(Image &img, double val, double val2, int &width, int &height)
{
	// double val = (double)img.width;
	double logval = log2(val);
	double intval;
	double fracval = modf(logval, &intval);
	if(intval-fracval == intval)
		width = intval;
	else
		width = intval+1;

	val = val2;
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

Image getpow2img(Image &orig, int width_exp, int height_exp)
{
	// printf("%d\t%d\n", width_exp, height_exp);

	int width = floor(pow(2,width_exp));
	int height = floor(pow(2,height_exp));

	// printf("%d\t%d\n", width, height);
	// if(width == orig.width && height == orig.height)
	// 	return orig;

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


int main(int argc,char *argv[]){
	const char *str = argv[1]; // image to work
	int k = atoi (argv[2]); // kernel size for median blur (odd no only)
	int r = atoi (argv[3]); // radius of selection ( cap )
	int c = atoi (argv[4]); // c = 1 for compression , 0 for edges
    Image im1(str);
    Image im2 = median_blur(im1,k);
    double val = (double)im2.width;
    double val2 = (double)im2.height;
    im2.write_jpeg("./test_images/median.jpg", GRAYSCALE);
    // Image orig_img("./test_images/median.jpg");
    double *reals = NULL;
	double *imags = NULL;
	int dir = 1;
    int dimX=0, dimY=0;
    // printf("%d\n", im2.width);
	closest_m(im2, val, val2, dimX, dimY);
	Image img = getpow2img(im2, dimX, dimY);
	Image imag_fourier = CT_FFT2D(img, dimX, dimY, im2.width, im2.height,r,c);
    return 0;
}
