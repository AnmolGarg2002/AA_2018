main:	gaussian median fourier

gaussian: gaussian_blur.cpp median fourier
	g++ gaussian_blur.cpp -ljpeg -o gaussian

median: median_blur.cpp
	g++ median_blur.cpp -ljpeg -o median

fourier: fourier_alpha.cpp
	g++ fourier_alpha.cpp -ljpeg -o fourier