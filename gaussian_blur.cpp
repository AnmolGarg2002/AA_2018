#include "BGR.hpp"



//Utility function to display the image
void display(BGR *source){
    int size = source->height*source->width*source->components;
    cout<<"Filename : "<<source->filename<<endl;
    cout<<"Height : "<<source->height<<endl;
    cout<<"Width : "<<source->width<<endl;
    cout<<"Components : "<<source->components<<endl;
    for(int i = 0; i < source->height; ++i){ 
        for(int j = 0; j < source->width ; j++)
            cout<<(int)source->R[i][j]<<" "<< (int)source->G[i][j]<<" "<<(int)source->B[i][j]<<" ";
    }

    cout<<endl<<"---------------------------------------------"<<endl;
}

pair<int, int> get_pixel(BGR *source, int index) {
    pair<int, int> result;
    result.first = index / source->width;
    result.second = index % source->width;
    return result;
}

void set_pixel(BGR *source, int index, int r, int g, int b) {
    int i = index / source->width;
    int j = index % source->width;

    if (r < 0) r = 0;
    if (r > 255) r = 255;

    if (g < 0) g = 0;
    if (g > 255) g = 255;

    if (b < 0) b = 0;
    if (b > 255) b = 255;

    source->R[i][j] = r;
    source->G[i][j] = g;
    source->B[i][j] = b;

 
}

int *boxes_for_gauss(double sigma, int n) {
    double w_ideal = sqrt((12 * sigma * sigma / n) + 1);
    int wl = (int)floor(w_ideal);
    if (wl % 2 == 0) wl--;

    int wu = wl + 2;

    double m_ideal = (12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) / (-4 * wl - 4);
    int m = round(m_ideal);


    int *sizes = new int[n]();
    for (int i = 0; i < n; i++) {
        sizes[i] = i < m ? wl : wu;
    }
    return sizes;
}

void box_blur_h(BGR *source, BGR *target, int w, int h, int radius) {


    double iarr = (double)1 / (radius + radius + 1);
    for (int i = 0; i < h; i++) {

        int ti = i * w;
        int li = ti;
        int ri = ti + radius;
        pair<int,int> fv = get_pixel(source, ti);
        pair<int,int> lv = get_pixel(source, ti + w - 1);

        unsigned currennt_r = source->R[fv.first][fv.second] * (radius + 1);
        unsigned currennt_g = source->G[fv.first][fv.second] * (radius + 1);
        unsigned currennt_b = source->B[fv.first][fv.second] * (radius + 1);

        
        

        for (int j = 0; j < radius; j++) {
            pair<int,int> pixel = get_pixel(source, ti + j);
            currennt_r += source->R[pixel.first][pixel.second];
            currennt_g += source->G[pixel.first][pixel.second];
            currennt_b += source->B[pixel.first][pixel.second];
            
            
        }


        
        for (int j = 0; j <= radius; j++) {
            //  cout<<"Reached here <= radiuas"<<endl;
            pair<int, int> pixel = get_pixel(source, ri++);
            currennt_r += (source->R[pixel.first][pixel.second] - source->R[fv.first][fv.second]);
            currennt_g += (source->G[pixel.first][pixel.second] - source->G[fv.first][fv.second]);
            currennt_b += (source->B[pixel.first][pixel.second] - source->B[fv.first][fv.second]);
            // cout<<"R:" <<currennt_r<< " "<<"G:" <<currennt_g<< " "<<"B:" <<currennt_b<< " "<<endl;

            set_pixel(target, ti++, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);
        }



        for (int j = radius + 1; j < w - radius; j++) {
            pair<int,int> first_pixel = get_pixel(source, ri++);
            pair<int,int> second_pixel = get_pixel(source, li++);

            currennt_r += (source->R[first_pixel.first][first_pixel.second] - source->R[second_pixel.first][second_pixel.second]);
            currennt_g += (source->G[first_pixel.first][first_pixel.second] - source->G[second_pixel.first][second_pixel.second]);
            currennt_b += (source->B[first_pixel.first][first_pixel.second] - source->B[second_pixel.first][second_pixel.second]);

            set_pixel(target, ti++, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);
        }
       

        for (int j = w - radius; j < w; j++) {
            pair<int,int> pixel = get_pixel(source, li++);

            currennt_r += (source->R[lv.first][lv.second] - source->R[pixel.first][pixel.second]);
            currennt_g += (source->G[lv.first][lv.second] - source->G[pixel.first][pixel.second]);
            currennt_b += (source->B[lv.first][lv.second] - source->B[pixel.first][pixel.second]);

           set_pixel(target, ti++, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);
        }

        
    }

}

void box_blur_t(BGR *source,BGR *target, int w, int h, int radius) {

   
    double iarr = (double)1 / (radius + radius + 1);
    for (int i = 0; i < w; i++) {
        int ti = i;
        int li = ti;
        int ri = ti + radius * w;

        pair<int,int> fv = get_pixel(source, ti);
        pair<int,int> lv = get_pixel(source, ti + w * (h - 1));

        unsigned currennt_r = source->R[fv.first][fv.second] * (radius + 1);
        unsigned currennt_g = source->G[fv.first][fv.second] * (radius + 1);
        unsigned currennt_b = source->B[fv.first][fv.second] * (radius + 1);

        for (int j = 0; j < radius; j++) {
            pair<int,int> pixel = get_pixel(source, ti + j * w);
            currennt_r += source->R[pixel.first][pixel.second];
            currennt_g += source->G[pixel.first][pixel.second];
            currennt_b += source->B[pixel.first][pixel.second];
        }

        for (int j = 0; j <= radius; j++) {
            pair<int, int> pixel = get_pixel(source, ri);
            currennt_r += (source->R[pixel.first][pixel.second] - source->R[fv.first][fv.second]);
            currennt_g += (source->G[pixel.first][pixel.second] - source->G[fv.first][fv.second]);
            currennt_b += (source->B[pixel.first][pixel.second] - source->B[fv.first][fv.second]);

            set_pixel(target, ti, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);

            ri += w;
            ti += w;
        }

        for (int j = radius + 1; j < h - radius; j++) {
            pair<int, int> first_pixel = get_pixel(source, ri);
            pair<int, int> second_pixel = get_pixel(source, li);

            currennt_r += (source->R[first_pixel.first][first_pixel.second] - source->R[second_pixel.first][second_pixel.second]);
            currennt_g += (source->G[first_pixel.first][first_pixel.second] - source->G[second_pixel.first][second_pixel.second]);
            currennt_b += (source->B[first_pixel.first][first_pixel.second] - source->B[second_pixel.first][second_pixel.second]);

            set_pixel(target, ti, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);

            li += w;
            ri += w;
            ti += w;
        }

        for (int j = h - radius; j < h; j++) {
            pair<int, int> pixel = get_pixel(source, li);

            currennt_r += (source->R[lv.first][lv.second] + source->R[pixel.first][pixel.second]);
            currennt_g += (source->G[lv.first][lv.second] + source->G[pixel.first][pixel.second]);
            currennt_b += (source->B[lv.first][lv.second] + source->B[pixel.first][pixel.second]);

            set_pixel(target, ti, currennt_r * iarr, currennt_g * iarr, currennt_b * iarr);

            li += w;
            ti += w;
        }
    }

    
}

void copy_image(BGR *source, BGR *target) {
    for (int row = 0; row < source->height; row++) {
        for (int coll = 0; coll < source->width; coll++) {
            target->R[row][coll] = source->R[row][coll];
            target->G[row][coll] = source->G[row][coll];
            target->B[row][coll] = source->B[row][coll];
        }
    }
}



void box_blur(BGR *source, BGR *target, int w, int h, int radius) {
    copy_image(source, target);
    box_blur_h(target, source, w, h, radius);
    box_blur_t(source, target, w, h, radius);
}


/**
    Performs a guassian blur operation in O(n) time.

    @param source the source Image
    @param radius the intensity of the blur
    @param savename the filename to save the result into
    @return void

*/
void gaussian_blur(Image &source, double radius ,char *savename) {
    int width = source.width;
    int height = source.height;
    
    BGR *src = new BGR(source);
    BGR *tar = new BGR(source.width, source.height, source.components);
  
    // cout<<"Starting Processing"<<endl;
    int *bxs = boxes_for_gauss(radius, 3);
    box_blur(src, tar, width, height, (bxs[0] - 1) / 2);
    box_blur(tar, src, width, height, (bxs[1] - 1) / 2);
    box_blur(src, tar, width, height, (bxs[2] - 1) / 2);

    //free momory
    free(bxs);
    // cout<<"Returnoing"<<endl;

    tar->save(savename);
    
}





int main(){


    Image im1("./test_images/1.jpg");

    gaussian_blur(im1, 3 , "./test_images/blurred.jpg");
    return 0;
}