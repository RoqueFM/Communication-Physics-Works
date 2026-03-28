#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define IMAX 1000
#define JMAX 120
#define NMAX 2000

#define FRAMESTEP 2

#define MU0 1.256637061e-6
#define EPS0 8.854187817e-12
#define DT 2.358654337e-9
#define DX 1.0

#define M 50.0

#define PI acos(-1.)

#define IDX(i,j) ((i)*JMAX + (j))

#include "../save_frames.h"

#define N_PML 2
#define DELTA_PML 10.0
#define SIGMA_MAX 5e-3 // Se refleja un poco con 2.75e-3

#define X0_EPS 400
#define X_LENGHT 490
#define Y0_EPS 50
#define Y_LENGHT 20

double exponencial_entera(double x, int n){
    if(n==0)
        return 1.;
    else
        return x*exponencial_entera(x,n-1);
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
    FILE *ey_t = fopen("ey_frente_a_t.txt","w");
    int frame = 0;
    system("mkdir -p Frames");
    // forced_scale = 0.005;

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
            rho[k]  = 0.0;
            if(i>=X0_EPS)
            if(j>=Y0_EPS && j<=Y0_EPS+Y_LENGHT)
                eps[k] *= 3;
            if(j>10 && j<110){
                sigma[k] = 0.0;
                if(i<=DELTA_PML)
                    sigma[k] = SIGMA_MAX*exponencial_entera(fabs(i - DELTA_PML)/DELTA_PML, N_PML);
                else if(i>=IMAX-DELTA_PML)
                    sigma[k] = SIGMA_MAX*exponencial_entera(fabs(i - (IMAX - DELTA_PML))/DELTA_PML, N_PML);
                rho[k] = sigma[k]/eps[k]*mu[k]; // Se podría quitar el subíndice para mayor velocidad
            }else
                sigma[k] = 1e9;
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

    // Datos frente al tiempo
    int n_puntos = 2;
    int puntos_frente_a_t[][2] = {
        {600,40},
        {600,60}
    };

    fprintf(ex_t,"#$t$ (s)\t$E_x$ (V/m)\t");
    fprintf(ey_t,"#$t$ (s)\t$E_y$ (V/m)\t");
    for(int i = 0; i<n_puntos; i++){
        fprintf(ex_t,"(%d,%d)\t",puntos_frente_a_t[i][0],puntos_frente_a_t[i][1]);
        fprintf(ey_t,"(%d,%d)\t",puntos_frente_a_t[i][0],puntos_frente_a_t[i][1]);
    }
    fprintf(ex_t,"\n");
    fprintf(ey_t,"\n");

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
            int k = IDX(50,j);
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

        fprintf(ex_t,"%5.5e\t",n*DT);
        fprintf(ey_t,"%5.5e\t",n*DT);
        for(int p = 0; p<n_puntos; p++){
            fprintf(ex_t,"%5.5e\t",ex[IDX(puntos_frente_a_t[p][0],puntos_frente_a_t[p][1])]);
            fprintf(ey_t,"%5.5e\t",ey[IDX(puntos_frente_a_t[p][0],puntos_frente_a_t[p][1])]);
        }
        fprintf(ex_t,"\n");
        fprintf(ey_t,"\n");


        if(n % FRAMESTEP == 0){
            save_frame_with_grid(ey, frame++);
        }
    }

    // =========================
    // SALIDA FINAL
    // =========================
    // Campo E en todo el dominio (para visualización 2D)
    FILE *fx = fopen("field_ex.dat","w");
    FILE *fy = fopen("field_ey.dat","w");

    for(j=JMAX-1;j>=0;j--){
        for(i=0;i<IMAX;i++){
            fprintf(fx,"%lf ",ex[IDX(i,j)]);
            fprintf(fy,"%lf ",ey[IDX(i,j)]);
        }
        fprintf(fx,"\n"); fprintf(fy,"\n");
    }

    fclose(fx); fclose(fy);
    fclose(ex_t); fclose(ey_t);

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