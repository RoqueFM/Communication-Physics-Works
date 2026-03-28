#ifndef SAVE_FRAMES_H
/*
## `save_fames.h`
# Fichero de cabecera que contiene 2 funciones de guardado de frames para utilizar.
# Se requieren 2 valores predefinidos y una macro:
-   IMAX, JMAX
-   IDX(IMAX,JMAX)
*/
#define SAVE_FRAMES_H

double forced_scale = -1.0;

void save_frame(double *field,int frame){
    char name[64];
    sprintf(name,"./Frames/frame_%05d.ppm",frame);

    FILE *f=fopen(name,"wb");

    fprintf(f,"P6\n%d %d\n255\n",IMAX,JMAX);

    double scale = 0;
    if(forced_scale == -1){
        for(int i=0;i<IMAX;i++)
        for(int j=0;j<JMAX;j++)
            if(scale < fabs(field[IDX(i,j)]))
                scale = fabs(field[IDX(i,j)]);
    }else{
        scale = forced_scale;
        // printf("Scale: %lf\n",scale);
    }

    for(int j=JMAX-1;j>=0;j--)
    for(int i=0;i<IMAX;i++){
        double v = field[IDX(i,j)]/scale;

        if(v>1) v=1;
        if(v<-1) v=-1;

        unsigned char r,g,b;

        if(v>=0)
        {
            r=255;
            g=(unsigned char)(255*(1-v));
            b=(unsigned char)(255*(1-v));
        }
        else
        {
            b=255;
            g=(unsigned char)(255*(1+v));
            r=(unsigned char)(255*(1+v));
        }

        fputc(r,f);
        fputc(g,f);
        fputc(b,f);
    }

    fclose(f);
}

void save_frame_with_grid(double *field,int frame){
    char name[64];
    sprintf(name,"./Frames/frame_%05d.ppm",frame);

    FILE *f=fopen(name,"wb");
    fprintf(f,"P6\n%d %d\n255\n",IMAX,JMAX);

    double scale = 0;
    if(forced_scale == -1){
        for(int i=0;i<IMAX;i++)
        for(int j=0;j<JMAX;j++)
            if(scale < fabs(field[IDX(i,j)]))
                scale = fabs(field[IDX(i,j)]);
    }else{
        scale = forced_scale;
        // printf("Scale: %lf\n",scale);
    }

    int step_x = 100; // cada 100 celdas en x
    int step_y = 50;  // cada 20 celdas en y

    for(int j=JMAX-1;j>=0;j--)
    for(int i=0;i<IMAX;i++){
            double v = field[IDX(i,j)]/scale;
            if(v>1) v=1;
            if(v<-1) v=-1;

            unsigned char r,g,b;

            // pintar campo
            if(v>=0){
                r=255;
                g=(unsigned char)(255*(1-v));
                b=(unsigned char)(255*(1-v));
            }else{
                b=255;
                g=(unsigned char)(255*(1+v));
                r=(unsigned char)(255*(1+v));
            }

            // sobreescribir grid
            if(i % step_x == 0 || j % step_y == 0){
                r = g = b = 0;
            }

            fputc(r,f);
            fputc(g,f);
            fputc(b,f);
    }

    fclose(f);
}

#endif