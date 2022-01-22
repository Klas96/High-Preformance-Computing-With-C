#include<omp.h>
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
// mäta tid
struct timespec start;
#define CHECKPOINT(fmt, ...) { \
struct timespec now; \
timespec_get(&now, TIME_UTC); \
float elapsed = (now.tv_sec + 1.0e-9 * now.tv_nsec) - (start.tv_sec + 1.0e-9 * start.tv_nsec); \
printf("\n%f: ", elapsed); \
printf(fmt, ##__VA_ARGS__); \
}

//parametrar

//int* Frequens;
//int* dist;

//float y_loc[3];


float data;

int Frequens[3465];

//float distence3D(float* x, float* y) {
int distence3D(float* X, float* Y) {
  //float dist = (x[0]-y[0])*(x[0]-y[0]) + (x[1]-y[1])*(x[1]-y[1]) + (x[2]-y[2])*(x[2]-y[2]);
  float dist_fun = (X[0]-Y[0])*(X[0]-Y[0]) + (X[1]-Y[1])*(X[1]-Y[1]) + (X[2]-Y[2])*(X[2]-Y[2]);
  return ((int)((sqrt(dist_fun)+0.005)*100));

};

void Comp_and_store(float* X, float* Y){

  int dist_loc = distence3D(X,Y);

  Frequens[dist_loc]++;
};


int main(int argc, char *argv[]) {
  // läser in variabler
timespec_get(&start, TIME_UTC);
CHECKPOINT("Start\n")
  char * endpt;
  int tn = strtol(argv[1]+2, &endpt,10);

  //export OMP_NUM_THREADS=tn;
  omp_set_num_threads(tn);
  //FILE * fp1 = fopen("cell_data/cell_e4","r");
  //FILE * fp2 = fopen("cell_data/cell_e4","r");

  FILE * fp1 = fopen("cell_data/cell_web","r");//Testing
  FILE * fp2 = fopen("cell_data/cell_web","r");//Testing

  fseek(fp1, 0, SEEK_END);

  long int end_Of_File = ftell(fp1);

  fseek(fp1, 0, SEEK_SET);

  int tot_row = end_Of_File/24;

  // allokerar minne till data
  //int max_load = 5000000/tn/sizeof(float); //max data memory
  int max_load = 30/tn/sizeof(float); //max data memory

  int max_row = max_load/3;

  float data[max_row][3];

  //int Frequens[3465];

  for(int i = 0; i < 3465; i++){
    Frequens[i] = 0;
  }

  int lenData1 = 0;


  //Ta reda på hur lång filen är och för for ist.(Snabbare)

  //#pragma omp parallel shared(Frequens) {
  //while(ftell(fp1) != EOF){
  for (int v = 0; v < tot_row; v+= max_row){
  //while (fscanf(fp1, "%c",data) != EOF) {
  CHECKPOINT("Calculating\n")

    //fseek(fp1, -1, SEEK_CUR);

    long int poss = ftell(fp1);

    //printf("Poss = %i\n",poss);
    fseek(fp2, poss, SEEK_SET);

    // läser in data 1
    //printf("Data 1:\n");
    for(int i = 0; i<max_row; i++) {

      fseek(fp1, +1, SEEK_CUR);
      if(fscanf(fp1, "%c",data) != EOF){
        fseek(fp1, -2, SEEK_CUR);

        fscanf(fp1, "%f %f %f" ,&data[i][0],&data[i][1],&data[i][2]);

        //printf("(%f, %f ,%f)\n",data[i][0],data[i][1],data[i][2]);

        lenData1 = i+1;
      }else{//CHECKPOINT("Finished loading data 1")
      //printf("Klar data 1\n");
      break;}
    }

    //Lopar över Data 1
    //#pragma omp parallel for
    float y_loc[3];
    for(int k = 0; k < lenData1; k++){
      
      //fseek(fp2, poss+1, SEEK_SET);

      //fseek(fp2, k*24+24, SEEK_SET);
      //fseek(fp2, k*24+24, SEEK_CUR);

      //printf("Data 2: (%f %f %f) k = %i lenData1 = %i\n",data[k][0],data[k][1],data[k][2],k,lenData1);

      //Börja vid första ellemntet i data1 och gå till EOF
      for(int w = v; w < tot_row; w++) {
      //while (fscanf(fp2, "%c",data) != EOF) {
        fseek(fp2, -2, SEEK_CUR);


        fscanf(fp2, "%f %f %f" ,&y_loc[0],&y_loc[1],&y_loc[2]);

        //printf("(%f, %f ,%f)\n",y_loc[0],y_loc[1],y_loc[2]);

        Comp_and_store(data[k], y_loc);

        //fseek(fp2, 1, SEEK_CUR);
      }
      //CHECKPOINT("Finished loading data 2")
    }
  }
  //}
  fclose(fp1);
  fclose(fp2);

  //printar resultat
  for(int i = 0; i < 3464; i++){
    if(Frequens[i] != 0){
      printf("%2.2f ",i*0.01);
      printf("%i \n",Frequens[i]);
    }
  }
CHECKPOINT("End")
  return 0;
}
