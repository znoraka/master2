#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image_ppm.h"

typedef struct { double x, y, z; int group; } point_t, *point;
 
double randf(double m);
point gen_xyz(int count, double radius);
point colorToPoint(OCTET *in, int width, int height);
double dist2(point a, point b);
int nearest(point pt, point cent, int n_cluster, double *d2);
void kpp(point pts, int len, point cent, int n_cent);
point lloyd(point pts, int len, int n_cluster);
void print_eps(point pts, int len, point cent, int n_cluster);
