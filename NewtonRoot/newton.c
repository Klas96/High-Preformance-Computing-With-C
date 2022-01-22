#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<complex.h>
#include<math.h>

//Global Variabales

float complex  Roots[d];
FILE * fproots = fopen("Roots_tQ_lQ_dQ.ppm","wb+");
FILE * Iter = fopen("Iter_tQ_lQ_dQ.ppm","wb+");

typedef struct Newton_arg {
  float complex x;
  int d;
  int* it;
  int l;
} Newton_arg;

typedef struct Bild_arg {
  float complex;
} Bild_arg;

float complex f(float complex x, int d){
  float complex x_orginal = x;
  
  for(size_t i = 1; i<d; i++){
    //x^d
    x  *= x_orginal; 
  }
  //printf("f(x) %f+%fi  ",creal(x-1), cimag(x-1));
  return  x-1;
}

float complex df(float complex x, int d){
  float complex x_orginal = x;
  for(size_t i = 1; i<(d-1); i++){
    //x^(d-1)
    x *= x_orginal;
  }
  //printf("df(x) %f+%fi  ",creal(x*d),cimag(x*d));
  return d*x;
}

void* skapaBilder(void* arg){
  struct Bild_arg* args = (struct Bild_arg*)arg;

  pthread_mutex_lock(xp[]);
  
  int Color[3];
  
  for(size_t j = 0 ; j<l; j++){
    for(size_t k = 0; k<l; k++){

      int curentRoot = 0;

      arg->x = xp[j][k];
      
      newton(arg);
      arg->it = 0;
      
      Color[0] = (int)(curentRoot)*20%255;
      Color[1] = (int)abs(curentRoot-1)*30%255;
      Color[2] = (int)abs(curentRoot-2)*40%255;
      
      (void) fprintf(fproots, "%d %d %d  ",Color[0],Color[1],Color[2]);
      
      Color[0] = &arg->it;
      Color[1] = &arg->it;
      Color[2] = &arg->it;
      
      (void) fprintf(Iter, "%d %d %d  ",Color[0],Color[1],Color[2]);

      for(int l = 0; l < d; l++){
	if((cimag(xp[j][k])-cimag(Roots[l]))<0.1 && (creal(xp[j][k])-creal(Roots[l]))<0.1){
	  curentRoot = l;
	  //printf("%f + %fi\n",cimag(Roots[l]),cimag(Roots[l]));
       }
    }
}


void* newton(void* arg){
  //Slutar när steglängd mindre änn tol (ändra till ratio av f)
  struct Newton_arg* args = (struct Newton_arg*)arg;
  float complex x_new = args->x;
  int d_new = args->d;
  int* it_new = args->it;
  int l_new = args->l;
  //free(arg);
  
  float tol = 0.01;
  
  //Steg
  float complex p = 1 + 1*I;
  
  //Iteration
  it_new = 0;
  int n = 0;
  
  while(cabsf(p) > tol){
    p = f(x_new,d_new)/df(x_new,d_new);
    x_new = x_new - p;//kan det hända något här???
    it_new += 1;
    
    n = isnormal(creal(x_new));
    
    if(n == 0 ||cabsf(x_new)<0.01){
      x_new = 0+0I;
      it_new = 100000000;
      break;
    }
    //printf("x %f+%fi  p %f+%fi  it %i\n",creal(x),cimag(x),creal(p),cimag(p),it);
  }
  //printf("x %f+%fi  p %f+%fi  it %i\n",creal(x),cimag(x),creal(p),cimag(p),it);
  //return it;
}

int main(int argc, char*argv[]){
  //input argument
  
  char * endpt;
  int t = strtol(argv[1]+2, &endpt,10) + 2;
  int l = strtol(argv[2]+2, &endpt,10);
  int d = strtol(argv[3], &endpt,10);

  printf("Roots:\n");
  for(int rootNr = 0; rootNr<d; rootNr++){
    Roots[rootNr] = cos(2*3.14*rootNr/d)+I*sin(2*3.14*rootNr/d);
    printf("%f + %fi\n",creal(Roots[rootNr]),cimag(Roots[rootNr]));
  }
  
  pthread_t threads[t];
  
  //Aloockera matris
  printf("Skapar Matris\n");
  float complex* xpp =(float complex*)malloc(l*l*sizeof(float complex));
  float complex** xp =(float complex**)malloc(l*sizeof(float complex*));
  
  for ( size_t ix = 0, jx = 0; ix < l; ++ix, jx+=l ){
    xp[ix] = xpp + jx;
  }
  
  //Lopar igenom matrisen och sätter imaginera värden som kordinater -2 till 2
  
  float re, im;
  
  //Steglängd
  float sz = 4.0f/(l-1);
  for(int j = 0; j<l; j++){
    for(int k = 0; k<l; k++){
      re = -2+sz*k;
      im = 2-sz*j;
      xp[j][k] = re + im*I;
      //printf("%f+%fi ",creal(xp[j][k]),cimag(xp[j][k]));
    }
  }
  
  //Skapa PPM fil.
  printf("Skapar filer\n");
  
  (void) fprintf(fproots, "P3\n%d %d\n255\n",l,l);
  (void) fprintf(Iter, "P3\n%d %d\n10\n",l,l);

  //printf("Svaren: \n");


  //initi struct ska ha xp d *it l
  struct Newton_arg *arg = (struct Newton_arg*)malloc(sizeof(struct Newton_arg));
  arg->x = 0;
  arg->d = d;
  arg->it = 0;
  arg->l = l;
  
  //Newton_arg arg = { float complex .x = 0 , int* .it = 0, int l = 0};
  
  //  struct arg;
        
      //printf("%f+%fi ",creal(xp[j][k]),cimag(xp[j][k]));
      //Skriver en färg till ppm filen
      
      //Color[0] = (int)abs(creal(xp[j][k]))*10;
      //Color[1] = (int)abs(cimag(xp[j][k]))*10;
      //Color[2] = (int)abs(cimag(xp[j][k])*creal(xp[j][k]))*10;
      
      
     
    }
    (void) fprintf(fproots, "\n");
    (void) fprintf(Iter, "\n");
  }
  
  (void) fclose(fproots);
  (void) fclose(Iter);
  
  return 0;
}
