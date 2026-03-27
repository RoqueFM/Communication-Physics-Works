#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Dimensiones de la malla y número de pasos temporales
#define IMAX 1000
#define JMAX 1000
#define NMAX 700

// Constantes físicas (SI)
#define MU0 1.256637061e-6
#define EPS0 8.854187817e-12

// Parámetros de discretización
#define DT 2.358654337e-9   // Paso temporal (cumple condición CFL)
#define DX 1.0              // Tamaño de celda espacial (en metros)

#define PI acos(-1.)

// Macro para indexar la matriz 2D como vector 1D (row-major)
#define IDX(i,j) ((i)*JMAX + (j))

int main(){

    int i,j,n;

    int size = IMAX*JMAX;

    // =========================
    // CAMPOS
    // =========================
    // Componentes del modo TE (2D):
    // ex(i,j) ~ Ex(i+1/2,j)
    // ey(i,j) ~ Ey(i,j+1/2)
    // hz(i,j) ~ Hz(i+1/2,j+1/2)
    double *ex  = calloc(size,sizeof(double));
    double *ey  = calloc(size,sizeof(double));
    double *hz  = calloc(size,sizeof(double));

    // =========================
    // COEFICIENTES FDTD
    // =========================
    double *Cax = malloc(size*sizeof(double));
    double *Cbx = malloc(size*sizeof(double));
    double *Cay = malloc(size*sizeof(double));
    double *Cby = malloc(size*sizeof(double));
    double *Da  = malloc(size*sizeof(double));
    double *Db  = malloc(size*sizeof(double));

    // =========================
    // PARÁMETROS DEL MEDIO
    // =========================
    // Se definen como arrays para permitir heterogeneidad espacial en ejercicios posteriores
    double *eps = calloc(size,sizeof(double));
    double *mu = calloc(size,sizeof(double));
    double *sigma = calloc(size,sizeof(double));
    double *rho = calloc(size,sizeof(double));

    // Archivo para monitorizar la propagación temporal
    FILE *ex_t = fopen("ex_frente_a_t.txt","w");
    FILE *ey_t = fopen("ey_frente_a_t.txt","w");

    // Datos frente al tiempo
    int n_puntos = 2;
    int puntos_frente_a_t[][2] = {
        {500,560},
        {500,740}
    };

    // =========================
    // INICIALIZACIÓN DEL MEDIO
    // =========================
    // Vacío: ε = ε0, μ = μ0, sin pérdidas
    #pragma omp parallel for collapse(2)
    for(i=0;i<IMAX;i++)
        for(j=0;j<JMAX;j++){
            int k = IDX(i,j);
            eps[k] = EPS0;
            mu[k]  = MU0;
            sigma[k] = 0.0;
            rho[k]  = 0.0;
        }

    // =========================
    // CÁLCULO DE COEFICIENTES FDTD
    // =========================
    // Se calculan los coeficientes locales para cada celda
    #pragma omp parallel for collapse(2)
    for(i=0;i<IMAX;i++){
        for(j=0;j<JMAX;j++){
            int k = IDX(i,j);

            // Coeficientes eléctricos (Ex, Ey)
            double a = sigma[k]*DT/(2*eps[k]);
            Cax[k] = (1 - a) / (1 + a);
            Cay[k] = (1 - a) / (1 + a);
            
            double b = DT/(DX*eps[k]);
            Cbx[k] = b / (1 + a);
            Cby[k] = b / (1 + a);

            // Coeficientes magnéticos (Hz)
            double c = rho[k]*DT/(2*mu[k]);
            Da[k] = (1 - c) / (1 + c);
            Db[k] = (DT/(DX*mu[k])) / (1 + c);
        }
    }

    // =========================
    // BUCLE TEMPORAL
    // =========================
    for(n=0;n<NMAX;n++){

        // -------------------------
        // ACTUALIZACIÓN DE E (Ex, Ey)
        // -------------------------
        // Se excluye el marco (i=0, i=IMAX-1, etc.)
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

        // -------------------------
        // FUENTE DURA
        // -------------------------
        // Fuente puntual en el centro del dominio
        // Introduce una onda sinusoidal
        ex[IDX(IMAX/2,JMAX/2)] = sin(2*PI*n/50.0);

        // -------------------------
        // ACTUALIZACIÓN DE H (Hz)
        // -------------------------
        #pragma omp parallel for collapse(2)
        for(i=1;i<IMAX-1;i++){
            for(j=1;j<JMAX-1;j++){

                int k = IDX(i,j);

                hz[k] = Da[k]*hz[k]
                      + Db[k]*( ex[IDX(i,j+1)] - ex[k]
                              - ey[IDX(i+1,j)] + ey[k] );
            }
        }

        // -------------------------
        // MONITORIZACIÓN
        // -------------------------
        // Se comprueba la propagación en dos puntos
        if(n%(NMAX/10)==0)
            printf("Step %d\n",n);

        // printf("ex(500,560)=%5.3e\tex(500,740)=%5.3e\n",
        //        ex[IDX(500,560)],ex[IDX(500,740)]);

        // fprintf(ex_t,"%5.5e\t%5.5e\t%5.5e\n",
        //         n*DT,
        //         ex[IDX(500,560)],
        //         ex[IDX(500,740)]);
        // fprintf(ey_t,"%5.5e\t%5.5e\t%5.5e\n",
        //         n*DT,
        //         ey[IDX(500,560)],
        //         ey[IDX(500,740)]);

        fprintf(ex_t,"%5.5e\t",n*DT);
        fprintf(ey_t,"%5.5e\t",n*DT);
        for(int p = 0; p<n_puntos; p++){
            fprintf(ex_t,"%5.5e\t",ex[IDX(puntos_frente_a_t[p][0],puntos_frente_a_t[p][1])]);
            fprintf(ey_t,"%5.5e\t",ey[IDX(puntos_frente_a_t[p][0],puntos_frente_a_t[p][1])]);
        }
        fprintf(ex_t,"\n");
        fprintf(ey_t,"\n");
    }

    // =========================
    // SALIDA FINAL
    // =========================
    // Campo E en todo el dominio (para visualización 2D)
    FILE *fx = fopen("field_ex.dat","w");
    FILE *fy = fopen("field_ey.dat","w");

    for(i=0;i<IMAX;i++){
        for(j=0;j<JMAX;j++){
            fprintf(fx,"%lf ",ex[IDX(i,j)]);
            fprintf(fy,"%lf ",ey[IDX(i,j)]);
        }
        fprintf(fx,"\n"); fprintf(fy,"\n");
    }

    fclose(fx); fclose(fy);
    fclose(ex_t); fclose(ey_t);

    // Liberación de memoria
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