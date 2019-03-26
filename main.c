#include <stdio.h>
#include "picosheader.h"




int readADC[50];
int readFDEV[49];
int readSDEV[48];
char frequency[32];


int main(int argc, char **argv) {
        int x=0;
        struct rets *returnvalues=(struct rets *)malloc(sizeof(struct rets));
        confport();
        openport();
        while(1) {
                printf("\nready to read the portt\n");
                int *m=portreadN();

                returnvalues=compose(m);



                x++;
                printf("\n%d kez girdi\n",x);
                sleep(2);


                for(int i=0;i<50;i++){
                        readADC[i]=returnvalues->adc[i];
                        printf("\n%d --> adc: %d",i,readADC[i]);
                        if(i<49) {
                        readFDEV[i]=returnvalues->fdev[i];
                        printf("\t\t%d --> fdev: %d",i,readFDEV[i]);
                        }
                        if(i<48){
                        readSDEV[i]=returnvalues->sdev[i];
                        printf("\t\t\t\t%d --> sdev: %d",i,readSDEV[i]);
                        }
                }

        }
        int c=close(fd);

        if (c==-1) {
               fprintf(stderr,"error = %s\n",strerror(errno));
               exit(-1);
              }

    return 0;
}
