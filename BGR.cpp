#include "jpegcheck.h"

using namespace std; 
class BGR 
{ 
    public: 
  
    int width;
    int height;
    unsigned char **B;
    unsigned char **G;
    unsigned char **R;
    BGR(char *filename)
    {
    	struct img i1 = read_jpeg(filename);
    	this->width = i1.width;
		this->height = i1.height;
  		B = (unsigned char**)malloc(sizeof(char*)*i1.height);
  		G = (unsigned char**)malloc(sizeof(char*)*i1.height);
  		R = (unsigned char**)malloc(sizeof(char*)*i1.height);
  		int i,j,count=0;
  		for(i=0;i<i1.width;i++){
  			R[i] = (unsigned char*)malloc(sizeof(char)*i1.width);
  			G[i] = (unsigned char*)malloc(sizeof(char)*i1.width);
  			B[i] = (unsigned char*)malloc(sizeof(char)*i1.width);
  			for(j=0;j<i1.height;j++){
  				R[i][j] = i1.image[0][count];
  				G[i][j] = i1.image[1][count];
  				B[i][j] = i1.image[2][count];
  				count++;
  			}
  		}
    }
};
  
int main() {  
	char img_name[] = "green.jpg";
    BGR obj1(img_name);
    int i,j;
    for(i=0;i<obj1.width;i++){
    	for(j=0;j<obj1.height;j++){
    		printf("%d ",obj1.G[i][j]);
    	}
    	printf("\n");
    }
    return 0; 
} 
