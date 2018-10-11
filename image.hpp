#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <typeinfo>
#include <string.h>
#include <unistd.h>
#include <bits/stdc++.h>

using namespace std;
/* High level class Image */

class Image
{

public:

	int width;
	int height;
	int components;
	unsigned char *image; //contains image pixels in form R,G,B,R,G,B,... in case of RGB image
	char *filename;

	Image(const char *name) //constructor takes filepath as parameter
	{
		int size = strlen(name);
		filename = new char[size];
		strcpy(filename, name);
		read_jpeg();
		
	}


	Image(Image &src){
		width = src.width;
		height = src.height;
		components = src.components;
		filename = src.filename;

		int size = width*height*components;
		image = new unsigned char[size];
		
		copy(src.image,src.image+size, image);
		
	}

	Image(int wid , int hei, int component, unsigned char *img){
		width = wid;
		height = hei;
		components = component;
		filename = "";

		int size = width*height*components;
		image = new unsigned char[size];
		
		copy(img,img+size, image);
		
	}

	Image(int wid , int hei, int component){

		width = wid;
		height = hei;
		components = component;
		filename = "";
		image = new unsigned char[width*height*components]();

	}
	
	unsigned char **read_jpeg() //puts jpeg filename in image
	{
		struct jpeg_decompress_struct cinfo;
		FILE * infile;

		JSAMPARRAY buffer;

		infile = fopen(filename, "rb");

		struct jpeg_error_mgr jerr; 

		cinfo.err = jpeg_std_error(&jerr);

		jpeg_create_decompress(&cinfo);

		jpeg_stdio_src(&cinfo, infile);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		width = static_cast<int>(cinfo.output_width);

		height = static_cast<int>(cinfo.output_height);

		components = cinfo.output_components;

		// printf("Height %d\nWidth %d\nComponents %d\n", height, width, components);
		// image = new unsigned char*[cinfo.output_components];
		image = new unsigned char[height * width * components];
		
		// int i;
		// for(i=0; i<cinfo.output_components; ++i)
		// 	image[i] = new unsigned char[height * width];

		int row_stride = width * components;
		int iter = 0;
		while (cinfo.output_scanline < cinfo.output_height) {
			
			(void) jpeg_read_scanlines(&cinfo, &image, 1);
			
			// for(i=0; i<row_stride; ++i)
			// 	image[i%components][width*iter + i/components] = temp[i];
			
			image += row_stride;
			iter += 1;
		}

		image -= iter*row_stride;
		// delete[] temp;
	}

	void write_jpeg(const char *savename, int quality=100) //writes image with name savename with passed quality (1-100)
	{
	  
	  struct jpeg_compress_struct cinfo;
	  
	  struct jpeg_error_mgr jerr;
	  
	  FILE * outfile;		/* target file */
	  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	  int row_stride;		/* physical row width in image buffer */

	  
	  cinfo.err = jpeg_std_error(&jerr);
	  
	  jpeg_create_compress(&cinfo);

	  
	  if ((outfile = fopen(savename, "wb")) == NULL) {
	    fprintf(stderr, "can't open %s\n", savename);
	    exit(1);
	  }
	  jpeg_stdio_dest(&cinfo, outfile);

	   
	  cinfo.image_width = width; 	/* image width and height, in pixels */
	  cinfo.image_height = height;
	  cinfo.input_components = components;		/* # of color components per pixel */
	  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	  
	  jpeg_set_defaults(&cinfo);
	  
	  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	  
	  jpeg_start_compress(&cinfo, TRUE);

	  
	  row_stride = width * components;	/* JSAMPLEs per row in image */

	  while (cinfo.next_scanline < cinfo.image_height) {
	    
	    row_pointer[0] = &image[cinfo.next_scanline * row_stride];
	    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	  }


	  jpeg_finish_compress(&cinfo);
	 
	  fclose(outfile);

	  jpeg_destroy_compress(&cinfo);

	}

	void display(const char *savename) //function to display image after saving at path savename
	{

		write_jpeg(savename); //saves jpeg first
		/* eog <savename> & - displays image on linux machines in background
		so that execution can continue */
		char eog[] = "eog ";
		char *command = new char[10+strlen(savename)];
		strcpy(command, eog);
		strcat(command, savename);
		char temp[] = " &";
		strcat(command, temp);
		system(command);

	}

	~Image()
	{
		// delete[] image;
		// delete[] filename;
	}

};