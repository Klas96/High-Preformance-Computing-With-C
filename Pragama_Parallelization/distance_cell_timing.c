
#include<omp.h>
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
// mäta tid
float t_initi;
float t_read;
float t_calc;

struct timespec start_initi;
struct timespec end_initi;

struct timespec start_read;
struct timespec end_read;

struct timespec start_calc;
struct timespec end_calc;

//float elapsed = (now.tv_sec + 1.0e-9 * now.tv_nsec) - (start.tv_sec + 1.0e-9 * start.tv_nsec); \

#define CHECKPOINT(fmt, ...) { \
struct timespec now; \
timespec_get(&now, TIME_UTC); \
float elapsed = (now.tv_sec + 1.0e-9 * now.tv_nsec) - (start.tv_sec + 1.0e-9 * start.tv_nsec); \
printf("\n%f: ", elapsed); \
printf(fmt, ##__VA_ARGS__); \
}

int freq[3465];

void Comp_and_store(float* X, float* Y) {

  float dist_fun = (X[0]-Y[0])*(X[0]-Y[0]) + (X[1]-Y[1])*(X[1]-Y[1]) + (X[2]-Y[2])*(X[2]-Y[2]);
  int dist_loc = (int)(((sqrt(dist_fun))*100)+0.5);

  freq[dist_loc]++;
};


int main(int argc, char *argv[]) {

  timespec_get(&start_initi, TIME_UTC);

  // läser in variabler

  char * endpt;
  int tn = strtol(argv[1]+2, &endpt,10);

  //export OMP_NUM_THREADS = tn;
  omp_set_num_threads(tn);

  //FILE * fp1 = fopen("cells","r");
  //FILE * fp2 = fopen("cells","r");

  FILE * fp1 = fopen("cell_data/cell_e4","r");
  FILE * fp2 = fopen("cell_data/cell_e4","r");

  //FILE * fp1 = fopen("cell_data/cell_e5","r");
  //FILE * fp2 = fopen("cell_data/cell_e5","r");

  //FILE * fp1 = fopen("cell_data/cell_web","r");//Testing
  //FILE * fp2 = fopen("cell_data/cell_web","r");//Testing

  fseek(fp1, 0, SEEK_END);
  long int end_Of_File = ftell(fp1);
  int tot_row = end_Of_File/24;

  fseek(fp1, 0, SEEK_SET);

  int max_load = 3000/sizeof(float); //max data memory
  //int max_load = 80/sizeof(float); //TESTING
  //int lenData1 = 0;

  int chunkSize = max_load/6;

  //int chunkSize_ind = chunkSize/2;

  float data1[chunkSize][3];
  float data2[chunkSize][3];

  //float data1[chunkSize_ind][3];
  //float data2[chunkSize_ind][3];

  for(int i = 0; i < 3465; i++){
    freq[i] = 0;
  }

  int chunkNr = (int)tot_row/chunkSize;
  int restRows = (tot_row % chunkSize);

  int rows1;
  int rows2;

  printf("chunkNr = %i,restRows = %i, tot_row = %i, chunkSize = %i \n",chunkNr,restRows, tot_row, chunkSize );

  //Lopar över chunks
  //CHECKPOINT("Börjar chunk loop\n");

  timespec_get(&end_initi, TIME_UTC);

  float t_initi = (end_initi.tv_sec + 1.0e-9 * end_initi.tv_nsec) - (start_initi.tv_sec + 1.0e-9 * start_initi.tv_nsec);

  printf("T_init = %f ",t_initi);

  for(int v = 0; v < chunkNr+1; v++){

    //int nx = omp_get_thread_num();
    //printf("Thread %i: I take chunk %i\n",nx,v);

    if(v<chunkNr){
      rows1 = chunkSize;
    }else{
      rows1 = restRows;
    }

    timespec_get(&start_read, TIME_UTC);
    //Läser Chunk 1
    //printf("Läser in Chunk 1:\n");
    for(int i = 0; i<rows1; i++){

      fscanf(fp1, "%f %f %f\n" ,&data1[i][0],&data1[i][1],&data1[i][2]);
      //printf("%f %f %f\n", data1[i][0],data1[i][1],data1[i][2]);

    }
    timespec_get(&end_read, TIME_UTC);

    t_read += (end_read.tv_sec + 1.0e-9 * end_read.tv_nsec) - (start_read.tv_sec + 1.0e-9 * start_read.tv_nsec);

    timespec_get(&start_calc, TIME_UTC);
    //Jämför med sig själv
    #pragma omp parallel for reduction(+:freq[:3465])
    for(int j = 0; j < rows1; j++){
      for(int k = j+1; k <rows1; k++){
        //Comp_and_store(data1[j],data1[k]);

        float dist_fun = (data1[j][0]-data1[k][0])*(data1[j][0]-data1[k][0]) + (data1[j][1]-data1[k][1])*(data1[j][1]-data1[k][1]) + (data1[j][2]-data1[k][2])*(data1[j][2]-data1[k][2]);
        int dist_loc = (int)(((sqrt(dist_fun))*100)+0.5);

        freq[dist_loc]++;

      }
    }
    timespec_get(&end_calc, TIME_UTC);

    t_calc += (end_calc.tv_sec + 1.0e-9 * end_calc.tv_nsec) - (start_calc.tv_sec + 1.0e-9 * start_calc.tv_nsec);

    fseek(fp2, (v+1)*24*chunkSize, SEEK_SET);

    //Jmf Chunk 1 med alla chunks

    for(int j = v+1; j <  chunkNr+1; j++){
      if(j<chunkNr){
        rows2 = chunkSize;
      }else{
        rows2 = restRows;

      }

      timespec_get(&start_read, TIME_UTC);
      //Läser Chunk 2
      //printf("Läser in Cunk 2: \n ");
      for(int i = 0; i<rows2; i++){
        fscanf(fp2, "%f %f %f" ,&data2[i][0],&data2[i][1],&data2[i][2]);
        //printf("%f %f %f\n", data2[i][0],data2[i][1],data2[i][2]);
      }
      timespec_get(&end_read, TIME_UTC);

      t_read += (end_read.tv_sec + 1.0e-9 * end_read.tv_nsec) - (start_read.tv_sec + 1.0e-9 * start_read.tv_nsec);


      //compare Chunk1 och Chunk2
      //printf("rows1 = %i, rows2 = %i\n", rows1, rows2);
      timespec_get(&start_calc, TIME_UTC);
      #pragma omp parallel for reduction(+:freq[:3465])
      for(int k = 0; k<rows1; k++){
        for(int l = 0; l < rows2; l++){
          //printf("Jämför: ");
          //printf("D1: %f %f %f och ", data1[k][0],data1[k][1],data1[k][2]);
          //printf("D2: %f %f %f\n", data2[l][0],data2[l][1],data2[l][2]);
          //Comp_and_store(data1[k],data2[l]);

          float dist_fun = (data1[k][0]-data2[l][0])*(data1[k][0]-data2[l][0]) + (data1[k][1]-data2[l][1])*(data1[k][1]-data2[l][1]) + (data1[k][2]-data2[l][2])*(data1[k][2]-data2[l][2]);
          int dist_loc = (int)(((sqrt(dist_fun))*100)+0.5);

          freq[dist_loc]++;


          //float dist_fun = (data1[0]-data2[0])*(data1[0]-Y[0]) + (data1[1]-Y[1])*(data1[1]-Y[1]) + (data1[2]-Y[2])*(data1[2]-Y[2]);
          //int dist_loc = (int)(((sqrt(dist_fun)+0.005)*100));

          //freq[dist_loc]++;

        }
      }

      timespec_get(&end_calc, TIME_UTC);

      t_calc += (end_calc.tv_sec + 1.0e-9 * end_calc.tv_nsec) - (start_calc.tv_sec + 1.0e-9 * start_calc.tv_nsec);
    }
  }

  fclose(fp1);
  fclose(fp2);

  //CHECKPOINT("Printar resultat \n");
  for(int i = 0; i < 3464; i++){
    if(freq[i] != 0){
      //printf("%2.2f ",i*0.01);
      //printf("%i \n",freq[i]);
    }
  }

  printf("T_read = %f T_calc = %f\n", t_read, t_calc);
//CHECKPOINT("Slut\n");
  return 0;

}
