#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define IMAX 1000
#define JMAX 120
#define NMAX 1000

#define FRAMESTEP 2

#define MU0 1.256637061e-6
#define EPS0 8.854187817e-12
#define DT 2.358654337e-9
#define DX 1.0

#define M 50.0

#define PI acos(-1.)

#define IDX(i,j) ((i)*JMAX + (j))

void save_frame(double *ex,int frame){
    char name[64];
    sprintf(name,"./Frames/frame_%05d.ppm",frame);

    FILE *f=fopen(name,"wb");

    fprintf(f,"P6\n%d %d\n255\n",IMAX,JMAX);

    double scale = 0;

    for(int i=0;i<IMAX;i++)
    for(int j=0;j<JMAX;j++)
        if(scale < fabs(ex[IDX(i,j)]))
            scale = fabs(ex[IDX(i,j)]);

    for(int j=0;j<JMAX;j++)
    for(int i=0;i<IMAX;i++)
    {
        double v = ex[IDX(i,j)]/scale;

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

int main(){

    int i,j,n;

    int size = IMAX*JMAX;

    double *ex  = calloc(size,sizeof(double));
    double *ey  = calloc(size,sizeof(double));
    double *hz  = calloc(size,sizeof(double));

    double *Cax = malloc(size*sizeof(double));
    double *Cbx = malloc(size*sizeof(double));
    double *Cay = malloc(size*sizeof(double));
    double *Cby = malloc(size*sizeof(double));
    double *Da  = malloc(size*sizeof(double));
    double *Db  = malloc(size*sizeof(double));

    // double eps = EPS0;
    // double mu  = MU0;
    // double sigma = 0.0;
    // double rho   = 0.0;

    double *eps = calloc(size,sizeof(double));
    double *mu = calloc(size,sizeof(double));
    double *sigma = calloc(size,sizeof(double));
    double *rho = calloc(size,sizeof(double));

    FILE *ex_t = fopen("ex_frente_a_t.txt","w");
    int frame = 0;

    // double Ca = (1 - sigma*DT/(2*eps)) / (1 + sigma*DT/(2*eps));
    // double Cb = (DT/(DX*eps)) / (1 + sigma*DT/(2*eps));

    // double Da0 = (1 - rho*DT/(2*mu)) / (1 + rho*DT/(2*mu));
    // double Db0 = (DT/(DX*mu)) / (1 + rho*DT/(2*mu));

    #pragma omp parallel for collapse(2)
    for(i=0;i<IMAX;i++)
        for(j=0;j<JMAX;j++){
            int k = IDX(i,j);
            eps[k] = EPS0;
            mu[k]  = MU0;
            if(j>10 && j<110)
                sigma[k] = 0.0;
            else
                sigma[k] = 1e9;
            rho[k]  = 0.0;
        }

    #pragma omp parallel for collapse(2)
    for(i=0;i<IMAX;i++){
        for(j=0;j<JMAX;j++){
            int k = IDX(i,j);
            double a = sigma[k]*DT/(2*eps[k]);
            Cax[k] = (1 - a) / (1 + a);
            Cay[k] = (1 - a) / (1 + a);
            
            double b = DT/(DX*eps[k]);
            Cbx[k] = b / (1 + a);
            Cby[k] = b / (1 + a);

            double c = rho[k]*DT/(2*mu[k]);
            Da[k] = (1 - c) / (1 + c);
            Db[k] = (DT/(DX*mu[k])) / (1 + c);
            // printf("Cax=%4.2e\tCbx=%4.2e\tDa=%4.2e\tDb=%4.2e\n",Cax[k],Cbx[k],Da[k],Db[k]);
        }
    }

    // Casos frontera
    for(i=0;i<IMAX;i++){
        // Arriba: el Ey de j=109 es el de j=110
        int k = IDX(i,110);
        double a = sigma[k]*DT/(2*eps[k]);
        Cay[IDX(i,109)] = (1 - a) / (1 + a);
        double b = DT/(DX*eps[k]);
        Cby[IDX(i,109)] = b / (1 + a);
    }
    for(i=0;i<IMAX;i++){
        // Abajo: el Ey de j=9 es el de j=10
        int k = IDX(i,10);
        double a = sigma[k]*DT/(2*eps[k]);
        Cay[IDX(i,9)] = (1 - a) / (1 + a);
        double b = DT/(DX*eps[k]);
        Cby[IDX(i,9)] = b / (1 + a);
    }

    for(n=0;n<NMAX;n++){

        #pragma omp parallel for collapse(2)
        for(i=1;i<IMAX-1;i++){
            for(j=1;j<JMAX-1;j++){

                int k = IDX(i,j);

                ex[k] = Cax[k]*ex[k]
                      + Cbx[k]*( hz[IDX(i,j)]
                               - hz[IDX(i,j-1)] );

                ey[k] = Cay[k]*ey[k]
                      + Cby[k]*( hz[IDX(i-1,j)]
                               - hz[IDX(i,j)] );
            }
        }

        for(j=11;j<109;j++){
            int k = IDX(500,j);
            ey[k] = sin(2*PI*n/M);
            // ex[k] = 0;
        }

        #pragma omp parallel for collapse(2)
        for(i=1;i<IMAX-1;i++){
            for(j=1;j<JMAX-1;j++){

                int k = IDX(i,j);

                hz[k] = Da[k]*hz[k]
                        + Db[k]*( (ex[IDX(i,j+1)] - ex[k])
                                - (ey[IDX(i+1,j)] - ey[k]) );
            }
        }

        if(n%(NMAX/10)==0)
            printf("Step %d\n",n);
        printf("ey(600,55)=%5.3e\tex(500,740)=%5.3e\n",ey[IDX(600,55)],ex[IDX(500,740)]);
        fprintf(ex_t,"%5.5e\t%5.5e\t%5.5e\n",n*DT,ey[IDX(600,55)],ex[IDX(500,740)]);
        if(n%FRAMESTEP==0){
            save_frame(ey,frame++);
            printf("frame %d\n",frame);
        }
    }

    FILE *f = fopen("field.dat","w");

    for(j=0;j<JMAX;j++){
        for(i=0;i<IMAX;i++){
            fprintf(f,"%lf ",ey[IDX(i,j)]);
        }
        fprintf(f,"\n");
    }

    fclose(f); fclose(ex_t);

    free(ex);
    free(ey);
    free(hz);

    free(Cax);
    free(Cbx);
    free(Cay);
    free(Cby);
    free(Da);
    free(Db);

    printf("Simulation finished\n");

    return 0;
}