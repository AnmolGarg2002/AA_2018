#include "BGR.hpp"
#include <stdlib.h>
#include <bits/stdc++.h> 
using namespace std; 

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

void median_blur(Image &source, double radius ,char *savename) {
    int width = source.width;
    int height = source.height;
    
    BGR *src = new BGR(source);
    BGR *tar = new BGR(source.width, source.height, source.components);
    
    int ker=3;//always odd
    
    unsigned char *R_src = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    unsigned char *G_src = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    unsigned char *B_src = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    unsigned char *R_tar = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    unsigned char *G_tar = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    unsigned char *B_tar = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
    int k=0;
    for(int i=0;i<height;i++)
    {
    	for(int j=0;j<width;j++)
    	{
    		R_src[i*width+j]=src->R[i][j];
    		G_src[i*width+j]=src->G[i][j];
    		B_src[i*width+j]=src->B[i][j];
    		R_tar[i*width+j]=src->R[i][j];
    		G_tar[i*width+j]=src->G[i][j];
    		B_tar[i*width+j]=src->B[i][j];
    	}
    }
    _medianfilter(R_src,R_tar,width,height,ker);
    _medianfilter(G_src,G_tar,width,height,ker);
    _medianfilter(B_src,B_tar,width,height,ker);
  	for(int i=0;i<height;i++)
    {
    	for(int j=0;j<width;j++)
    	{
    		tar->R[i][j]=R_tar[k];
    		tar->G[i][j]=G_tar[k];
    		tar->B[i][j]=B_tar[k];
    		k++;
    	}
    }
    free(R_src);
    free(G_src);
    free(B_src);
    free(R_tar);
    free(G_tar);
    free(B_tar);
    tar->save(savename);
}

int main(){
    Image im1("./test_images/2.jpg");
    median_blur(im1, 3 , "./test_images/median.jpg"); 
    return 0;
}
