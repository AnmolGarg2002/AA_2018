#include "image.hpp"
#define C char
#define UC unsigned C 

using namespace std; 
class BGR:public Image{
    public: 
        UC **B;
        UC **G;
        UC **R;

    BGR(Image &source):
    Image(source)
    {
        B = (UC**)malloc(sizeof(C*)*height);
  		G = (UC**)malloc(sizeof(C*)*height);
  		R = (UC**)malloc(sizeof(C*)*height);
  		int i,j,count=0;
  		for(i=0;i<height;i++){
  			R[i] = (UC*)malloc(sizeof(C)*width);
  			G[i] = (UC*)malloc(sizeof(C)*width);
  			B[i] = (UC*)malloc(sizeof(C)*width);
  			for(j=0;j<width;j++){
  				R[i][j] = image[count];
  				G[i][j] = image[count+1];
  				B[i][j] = image[count+2];
  				count+=components;
  			}
  		}
    }

    BGR(int width, int height, int components):
    Image(width,height,components){

        // for(int i = 0; i < height*width*components; ++i){
        //     image[i] = (UC) 0;
        // }
        B = (UC**)malloc(sizeof(C*)*height);
  		G = (UC**)malloc(sizeof(C*)*height);
  		R = (UC**)malloc(sizeof(C*)*height);
  		int i,j,count=0;
  		for(i=0;i<height;i++){
  			R[i] = (UC*)malloc(sizeof(C)*width);
  			G[i] = (UC*)malloc(sizeof(C)*width);
  			B[i] = (UC*)malloc(sizeof(C)*width);
  			for(j=0;j<width;j++){
  				R[i][j] = image[count];
  				G[i][j] = image[count+1];
  				B[i][j] = image[count+2];
  				count+=components;
  			}
  		}
    }

    BGR(const BGR &source):Image(source.width, source.height, source.components, source.image)
    {
        B = (UC**)malloc(sizeof(C*)*height);
  		G = (UC**)malloc(sizeof(C*)*height);
  		R = (UC**)malloc(sizeof(C*)*height);
  		int i,j,count=0;
  		for(i=0;i<height;i++){
  			R[i] = (UC*)malloc(sizeof(C)*width);
  			G[i] = (UC*)malloc(sizeof(C)*width);
  			B[i] = (UC*)malloc(sizeof(C)*width);
  			for(j=0;j<width;j++){
  				R[i][j] = image[count];
  				G[i][j] = image[count+1];
  				B[i][j] = image[count+2];
  				count+=components;
  			}
  		}
    }

    void save(char* filename){

        int count = 0;
        for(int i = 0; i < height; ++i){
            for(int j = 0; j < width; ++j){

                     image[count++] = R[i][j];
                     image[count++] = G[i][j];
                     image[count++] = B[i][j];
                }
        }

        write_jpeg(filename);
    }

    // ~BGR(){
    //     cout<<"Destructor called\n";
    // }
};
