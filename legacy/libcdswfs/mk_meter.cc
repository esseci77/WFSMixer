#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cairo.h>


#define W 8
#define L 61


static cairo_pattern_t *pattern (int t)
{
    cairo_pattern_t *P;

    P = 0;
    switch (t)
    {
    case 0:
        P = cairo_pattern_create_linear (4, L - 0.5, 4, 0.5);
	break;
    case 1:
        P = cairo_pattern_create_linear (0.5, 4, L - 0.5, 4);
	break;
    case 2:
        P = cairo_pattern_create_linear (4, 0.5, 4, L - 0.5);
	break;
    case 3:
        P = cairo_pattern_create_linear (L - 0.5, 4, 0.5, 4);
	break;
    }
    cairo_pattern_add_color_stop_rgb (P, 0.00, 0.00, 0.00, 1.00);
    cairo_pattern_add_color_stop_rgb (P, 0.33, 0.00, 0.75, 0.75);
    cairo_pattern_add_color_stop_rgb (P, 0.67, 0.00, 1.00, 0.00);
    cairo_pattern_add_color_stop_rgb (P, 0.78, 1.00, 1.00, 0.00);
    cairo_pattern_add_color_stop_rgb (P, 0.89, 1.00, 0.50, 0.20);
    cairo_pattern_add_color_stop_rgb (P, 1.00, 1.00, 0.20, 0.20);
    return P;
}



int main (int ac, char *av [])
{
    cairo_surface_t      *S;
    cairo_pattern_t      *P;
    cairo_t              *T;
     
    S = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 8, 61);
    T = cairo_create (S);
    P = pattern (0);    
    cairo_set_line_width (T, W);
    cairo_set_source (T, P);
    cairo_new_path (T);
    cairo_move_to (T, W / 2, 0);
    cairo_line_to (T, W / 2, L);
    cairo_stroke (T);
    cairo_pattern_destroy (P);
    cairo_surface_write_to_png (S, "meter0.png");
    cairo_surface_destroy (S);

    S = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 61, 8);
    T = cairo_create (S);
    P = pattern (1);    
    cairo_set_line_width (T, W);
    cairo_set_source (T, P);
    cairo_new_path (T);
    cairo_move_to (T, 0, W / 2);
    cairo_line_to (T, L, W / 2);
    cairo_stroke (T);
    cairo_pattern_destroy (P);
    cairo_surface_write_to_png (S, "meter1.png");
    cairo_surface_destroy (S);

    S = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 8, 61);
    T = cairo_create (S);
    P = pattern (2);    
    cairo_set_line_width (T, W);
    cairo_set_source (T, P);
    cairo_new_path (T);
    cairo_move_to (T, W / 2, 0);
    cairo_line_to (T, W / 2, L);
    cairo_stroke (T);
    cairo_pattern_destroy (P);
    cairo_surface_write_to_png (S, "meter2.png");
    cairo_surface_destroy (S);

    S = cairo_image_surface_create (CAIRO_FORMAT_RGB24, 61, 8);
    T = cairo_create (S);
    P = pattern (3);    
    cairo_set_line_width (T, W);
    cairo_set_source (T, P);
    cairo_new_path (T);
    cairo_move_to (T, 0, W / 2);
    cairo_line_to (T, L, W / 2);
    cairo_stroke (T);
    cairo_pattern_destroy (P);
    cairo_surface_write_to_png (S, "meter3.png");
    cairo_surface_destroy (S);

    return 0;
}

