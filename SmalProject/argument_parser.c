#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if(argc<=1) {
        printf("No Arguments Entered");
        exit(1);
     }

     int arg1 = atoi(argv[1]);

    printf("You entered: %d \n", arg1);

    return 0;

 }
