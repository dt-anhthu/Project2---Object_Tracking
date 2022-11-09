#include <stdio.h>
#include <stdlib.h>
# define STB_IMAGE_IMPLEMENTATION
# include "./headers/stb_image.h"
# define STB_IMAGE_WRITE_IMPLEMENTATION
# include "./headers/stb_image_write.h"


unsigned char *tracking_object(unsigned char *image_tp, int width_tp , int height_tp , int channel_tp, unsigned char *image_im, int width_im , int height_im , int channel_im)
{
    double result[(width_im-width_tp+1)*(height_im - height_tp +1)];
    double m, x, y;
    // Consine similarity
    for(int i=0; i<height_im-height_tp+1; i++)
    {
        for(int j=0; j<width_im-width_tp+1;j++)
        {
            m=0.0;
            x=0.0;
            y=0.0;
            for(int k=0; k<height_tp; k++)
            {
                for(int h=0; h<width_tp; h++)
                {
                    for(int p=0; p<channel_im; p++)
                    {
                        int index_im = (k+i)*channel_im*width_im+(j+h)*channel_im+p;
                        int index_tp = k*channel_im*width_tp + h*channel_im +p;
                        double value_im = image_im[index_im];
                        double value_tp = image_tp[index_tp];
                        m = m + (double)value_im * (double)value_tp;
                        x = x + value_im*value_im;
                        y = y + value_tp*value_tp;
                    }
                }
            }
            result[i*(width_im-width_tp+1)+j] = (double)m / (sqrt(x)*sqrt(y));
        }
    }
    // Find the first position
    double m= 0.0;
    int x_axis = 0, y_axis=0;
    for(int i=0; i<height_im-height_tp+1; i++)
    {
        for (int j=0; j<width_im-width_tp+1; j++)
        {
            if(result[i*(width_im-width_tp+1)+j]>m)
            {
                m = result[i*(width_im-width_tp+1)+j];
                y_axis = i;
                x_axis = j;
            }
        }
    }

    // Draw border
    for(int i=0; i<width_tp; i++)
    {
        for(int j=0; j<channel_tp; j++)
        {
            image_im[y_axis*width_im*channel_im+(x_axis+i)*channel_im+j] = 0;
            image_im [(y_axis+height_tp)*width_im*channel_im+(x_axis+i)*channel_im+j] = 0;
        }
    }
    for(int i=0; i<height_tp; i++)
    {
        for(int j=0; j<channel_im; j++)
        {
            image_im[(y_axis+i)*width_im*channel_im+ x_axis*channel_im+j]= 0;
            image_im[(y_axis+i)*width_im*channel_im+(x_axis+width_tp)*channel_im+j] = 0;
        }
    }

    // Update template
    for(int i=0; i<height_tp; i++)
    {
        for(int j=0; j<width_tp; j++)
        {
            for(int k=0; k<channel_tp; k++)
            {
                image_tp[i*width_tp*channel_tp+j*channel_tp+k] = image_im[(y_axis+i)*width_im*channel_im+(x_axis+j)*channel_im+k];
            }
        }
    }

}

int main()
{
     // declare variables
    int width_tp , height_tp , channel_tp ;
    char path_img_tp [50];
    char save_path_tp [50];


    int width_im , height_im , channel_im ;
    char path_img_im [50];
    char save_path_im [50];

    unsigned char *image_im;
    unsigned char *image_tp;

    for(int i=0; i<63; i++)
    {
        sprintf(path_img_im,"./images/img%d.jpg",i);
        sprintf(save_path_im,"./results/img_%d-res.jpg",i);

        sprintf(path_img_tp,"./templates/tp_%d.jpg",i);
        sprintf(save_path_tp,"./templates/tp_%d.jpg",i+1);

        // read image data
        image_im = stbi_load ( path_img_im, &width_im , &height_im , &channel_im , 0) ;
        if (image_tp == NULL)
        {
            printf ("\nError in loading the image \n") ;
            exit (1) ;
        }
        printf("Image %d:\n",i);
        printf ("width_im = %d height_tim = %d channel_im = %d\n", width_im , height_im , channel_im ) ;

        image_tp = stbi_load ( path_img_tp, &width_tp , &height_tp , &channel_tp , 0) ;
        if (image_tp == NULL)
        {
            printf ("\nError in loading the template image \n") ;
            exit (1) ;
        }
        printf("Template %d:\n",i);
        printf ("width_tp = %d height_tp = %d channel_tp = %d\n", width_tp , height_tp , channel_tp ) ;

        // Tracking Object
        tracking_object(image_tp, width_tp , height_tp , channel_tp, image_im, width_im , height_im , channel_im);

        // save image
        stbi_write_jpg ( save_path_im , width_im, height_im, channel_im , image_im , width_im*channel_im) ;
        stbi_write_jpg ( save_path_tp , width_tp, height_tp, channel_tp , image_tp , width_tp*channel_tp) ;

        printf ("New image saved to %s\n", save_path_im ) ;
    }

    return 0;
}
