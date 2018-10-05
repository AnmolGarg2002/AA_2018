#include"jpegcheck.h"

img read_jpeg(char *filename)
{
	struct img i1;
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

	int width = static_cast<int>(cinfo.output_width);

	int height = static_cast<int>(cinfo.output_height);

	buffer = (*cinfo.mem->alloc_sarray)
	((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	unsigned char **image = new unsigned char*[cinfo.output_components];
	unsigned char *temp = new unsigned char[height * width * cinfo.output_components];
	
	int i;
	for(i=0; i<cinfo.output_components; ++i)
		image[i] = new unsigned char[height * width];

	// int *img = new int[1800000];
	// int i=0, row=0;
	int row_stride = cinfo.output_width * cinfo.output_components;
	int iter = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void) jpeg_read_scanlines(&cinfo, &temp, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
		// printf("%d\n", buffer[0][0]);
		for(i=0; i<row_stride; ++i)
			image[i%3][width*iter + i/3] = temp[i];
		
		temp+=row_stride;
		iter+=1;
	}

	temp -= iter*row_stride;
	delete[] temp;
	i1.width = width;
	i1.height = height;
	i1.image = image;
	return i1;
}

/*int main()
{
	struct img i1;
	char img_name[] = "1.jpg";
	i1 = read_jpeg(img_name);
	unsigned char **image = i1.image;
	printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n",
		image[0][0], image[0][1], image[0][2],
		image[1][0], image[1][1], image[1][2],
		image[2][0], image[2][1], image[2][2]);
	printf("Width = %d , Height = %d\n",i1.width,i1.height);
}*/
