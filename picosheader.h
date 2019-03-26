
#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <string.h>


#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define POSIX_SOURCE 1
#define TRUE 1
#define FALSE 0

struct rets {

        int freqint,mfi1,mff1,mfi2,mff2;
        int *adc;
        int *fdev;
        int *sdev;
        char *message;
};

struct termios options;
int fd;


void openport(void) {

        printf("port will open\n");
        //device is opened with specific parameters.
        fd=open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK );
        //opening is corrected.
        if(fd < 0) perror(MODEMDEVICE);
        else printf("\nport opened successfully\n");
        //input control adjusted
        fcntl(fd,F_SETFL,0/* FNDELAY*/);
        tcgetattr(fd,&options);

        if (fd < 0) printf("error");



        options.c_cflag = BAUDRATE |/* CRTSCTS |*/ CS8 | CLOCAL | CREAD;

        //input options

        options.c_iflag = IGNPAR | ICRNL;

        //output options

        options.c_oflag =0;

        //local options

        options.c_lflag |=ICANON;
        //Control Characters
        options.c_cc[VINTR]    = 0;     /* Ctrl-c */
        options.c_cc[VQUIT]    = 0;     /* Ctrl-z */
        options.c_cc[VERASE]   = 0;     /* BACKSPACE */
        options.c_cc[VKILL]    = 0;     /* Ctrl-U */

        options.c_cc[VEOF]     = 0;     /* Ctrl-d */
        options.c_cc[VTIME]    = 0;     /* inter-character timer unused */
        options.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
        options.c_cc[VSWTC]    = 0;     /* '\0' */
        options.c_cc[VSTART]   = 0;     /* Ctrl-q */
        options.c_cc[VSTOP]    = 0;     /* Ctrl-s */
        options.c_cc[VSUSP]    = 0;     /* Ctrl-z */
        options.c_cc[VEOL]     = 0;     /* '\0' */
        options.c_cc[VREPRINT] = 0;     /* Ctrl-r */
        options.c_cc[VDISCARD] = 0;     /* Ctrl-u */
        options.c_cc[VWERASE]  = 0;     /* Ctrl-w */
        options.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
        options.c_cc[VEOL2]    = 0;     /* '\0' */
}

int * portreadN(void) {
        char msB=0;
        char lsB=0;
        int msI=0;
        int lsI=0;
        static int data[51];
        int *d;
        int i=0;
        d=data;
        read(fd,&msB,1);
        msI=msB;
        printf("\n msb of freq :%d---",msI);
        read(fd,&lsB,1);
        lsI=lsB;
        printf("lsb of freq :%d\n",lsI);
        *d=(msB*256)+lsB;
        d++;
        for (;i<50;i++) {

                read(fd,&msB,1);
                msI=msB;
                printf("\n msb of adc%d :%d--",i,msI);
                read(fd,&lsB,1);
                lsI=lsB;
                if(lsI<0)lsI=lsI+256;
                printf("lsb of adc%d :%d--",i,lsI);

                printf(" adc value of %d is %d\n",i,(msI*256)+lsI);
               *d=(msI*256)+lsI;
               d++;

            }

return data;

}

char * readport(void) {

        char buff=0;
        static char datset[256];
        char *ds;
        int cnt=0;
        ds=datset;
        char ready=0;


        for(cnt=0;cnt<255;cnt++) {

                read(fd,&buff,1);
                *ds=buff;
                ds++;
        }
        printf("\nread datas ::: %s\n",datset);
        return datset;
}


void confport(void) {
        char cnf[]="stty -F /dev/ttyUSB0 min 1 time 5 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke";

        char dispcnf[]="stty -F /dev/ttyUSB0";

        system(cnf);
         printf("port is configured\n");
        system(dispcnf);


}

void writeport(char m) {
        if(m=='q') write(fd,"q\0",2);
        if(m=='t') write(fd,"t\0",2);
        if(m=='s') write(fd,"s\0",2);
}


struct rets* ikea(char *p) {
        struct rets *retvals=(struct rets*)malloc(sizeof(struct rets));
        retvals->adc=(int *)malloc(50*sizeof(int));
        retvals->fdev=(int *)malloc(49*sizeof(int));
        retvals->sdev=(int *)malloc(48*sizeof(int));
        char *dsp=p;
        char freq[5];
        char temp[5];
        int adc[50];
        int fdev[49];
        int sdev[48];
        int freqint;
        int mfi1;
        int mff1;
        int mfi2;
        int mff2;
        int i,j;
        for(i=0;i<5;i++) {
                freq[i]=*dsp;
                dsp++;
        }
        freqint=atoi(freq);
        retvals->freqint=freqint;
        printf("\nFrequency: %d ",freqint);

        for(i=0;i<50;i++) {

                for(j=0;j<5;j++) {
                temp[j]=*dsp;
                dsp++;
                }
        retvals->adc[i]=atoi(temp);

        }



        j=1;
        while(j<50) {
            if((adc[j]<adc[j-1])){
                fdev[j-1]=(adc[j]-adc[j-1])*(-1);
            }
            else    {
                fdev[j-1]=(adc[j]-adc[j-1]);
            }

            j++;

        }

        int predic = 40;

        j=0;
        while(j<49) {
            if(fdev[j]<= predic/2){
                fdev[j]=0;
            }
            else if(fdev[j]>= predic/2)   {
                fdev[j]=1;
            }
            retvals->fdev[j]=fdev[j];
            j++;

        }

        printf("\nfdev calculated\n");
        j=1;
        while(j<48) {
            sdev[j-1]=(abs(fdev[j]-fdev[j-1]));
            if(sdev[j-1]<=20)    {
                sdev[j-1]=0;
            }
            retvals->sdev[j-1]=sdev[j-1];

            j++;

        }
        printf("sdev calculated\n");
        int countd1d = 0, curr_cnt = 1, freq_d1d = 7, key = 0,k=0,m=0;
        for(k = 0; k < 48; k++)    {
            key = fdev[k];
            for(m =k+1; m < 49; m++) {
                if(key == fdev[m] && freq_d1d != key)  {
                    curr_cnt++;
                }
            }
            if(countd1d < curr_cnt)    {
                countd1d = curr_cnt; // frequency
                curr_cnt = 1;
                freq_d1d = key; //Most Frequent index
            }
        }


        int countd2d = 0, curr_cnt2 = 1, freq_d2d = 7, key2 = 0,k2=0,m2=0;
        for(k2 = 0; k2 < 47; k2++)    {
            key2 = sdev[k2];
            for(m2 =k2+1; m2 < 48; m2++) {
                if(key2 == sdev[m2] && freq_d2d != key2)  {
                    curr_cnt2++;
                }
            }
            if(countd2d < curr_cnt2)    {
                countd2d = curr_cnt2; // frequency
                curr_cnt2 = 1;
                freq_d2d = key2; //Most Frequent index
            }
        }
        printf("signal type will be written\n");
        if(freq_d1d == 0 && freq_d2d==0){
        writeport('q');
        printf("signal type sent\n");
        }
        if(freq_d1d == 1 && freq_d2d == 0) {
                writeport('t');
                printf("signal type sent\n");
        }
        if(freq_d2d != 1 && freq_d1d != 0 && freq_d2d != 0 && freq_d1d != 1) {
                writeport('s');
                printf("signal type sent\n");
        }





        return retvals;
}
struct rets* compose(int *p) {
        struct rets *retvals=(struct rets*)malloc(sizeof(struct rets));
        retvals->adc=(int *)malloc(50*sizeof(int));
        retvals->fdev=(int *)malloc(49*sizeof(int));
        retvals->sdev=(int *)malloc(48*sizeof(int));
        int *dsp=(int *)malloc(51*sizeof(int));
        dsp=p;
        int adc[50];
        int fdev[49];
        int sdev[48];
        int freqint;
        int mfi1;
        int mff1;
        int mfi2;
        int mff2;
        int i,j;


        freqint=dsp[0];
        retvals->freqint=freqint;
        printf("\nFrequency: %d \n\n",freqint);

        for(i=1;i<51;i++) {

                adc[i-1]=dsp[i];
                retvals->adc[i-1]=adc[i-1];
               printf("taken value %d\n",retvals->adc[i-1]);
        }


        j=1;
        while(j<50) {
            if((adc[j]<adc[j-1])){
                fdev[j-1]=(adc[j]-adc[j-1])*(-1);
            }
            else    {
                fdev[j-1]=(adc[j]-adc[j-1]);
            }

            j++;
        printf("first derivative -> %d\n",fdev[j-1]);
        }

        int predic = 40;

        j=0;
        while(j<49) {
            if(fdev[j]<= predic/2){
                fdev[j]=0;
            }
            else if(fdev[j]>= predic/2)   {
                fdev[j]=1;
            }
            retvals->fdev[j]=fdev[j];
            j++;

        }

        printf("\nfdev calculated\n");
        j=1;
        while(j<48) {
            sdev[j-1]=(abs(fdev[j]-fdev[j-1]));
            if(sdev[j-1]<=20)    {
                sdev[j-1]=0;
            }
            sdev[j-1]=sdev[j-1];

            j++;

        }
        printf("sdev calculated\n");
        int countd1d = 0, curr_cnt = 1, freq_d1d = 7, key = 0,k=0,m=0;
        for(k = 0; k < 48; k++)    {
            key = fdev[k];
            for(m =k+1; m < 49; m++) {
                if(key == fdev[m] && freq_d1d != key)  {
                    curr_cnt++;
                }
            }
            if(countd1d < curr_cnt)    {
                countd1d = curr_cnt; // frequency
                curr_cnt = 1;
                freq_d1d = key; //Most Frequent index
            }
        }


        int countd2d = 0, curr_cnt2 = 1, freq_d2d = 7, key2 = 0,k2=0,m2=0;
        for(k2 = 0; k2 < 47; k2++)    {
            key2 = sdev[k2];
            for(m2 =k2+1; m2 < 48; m2++) {
                if(key2 == sdev[m2] && freq_d2d != key2)  {
                    curr_cnt2++;
                }
            }
            if(countd2d < curr_cnt2)    {
                countd2d = curr_cnt2; // frequency
                curr_cnt2 = 1;
                freq_d2d = key2; //Most Frequent index
            }
        }
        printf("signal type will be written\n");
        if(freq_d1d == 0 && freq_d2d==0){
        writeport('q');
        printf("signal type sent\n");
        }
        if(freq_d1d == 1 && freq_d2d == 0) {
                writeport('t');
                printf("signal type sent\n");
        }
        if(freq_d2d != 1 && freq_d1d != 0 && freq_d2d != 0 && freq_d1d != 1) {
                writeport('s');
                printf("signal type sent\n");
        }





        return retvals;
}
/*struct rets* ikea(char *p) {
        struct rets *retvals=(struct rets*)malloc(sizeof(struct rets));
        retvals->adc=(int *)malloc(50*sizeof(int));
        retvals->fdev=(int *)malloc(49*sizeof(int));
        retvals->sdev=(int *)malloc(48*sizeof(int));
        char *dsp=p;
        char freq[5];
        char temp[5];
        int adc[50];
        int fdev[49];
        int sdev[48];
        int freqint;
        int mfi1;
        int mff1;
        int mfi2;
        int mff2;
        int i,j;
        for(i=0;i<5;i++) {
                freq[i]=*dsp;
                dsp++;
        }
        freqint=atoi(freq);
        retvals->freqint=freqint;
        printf("\nFrequency: %d ",freqint);

        for(i=0;i<50;i++) {

                for(j=0;j<5;j++) {
                temp[j]=*dsp;
                dsp++;
                }
        adc[i]=atoi(temp);

        retvals->adc[i]=adc[i];
        }



        j=1;
        while(j<50) {
            if((adc[j]<adc[j-1])){
                fdev[j-1]=(adc[j]-adc[j-1])*(-1);
            }
            else    {
                fdev[j-1]=(adc[j]-adc[j-1]);
            }

            j++;

        }

        int predic = 40;

        j=0;
        while(j<49) {
            if(fdev[j]<= predic/2){
                fdev[j]=0;
            }
            else if(fdev[j]>= predic/2)   {
                fdev[j]=1;
            }
            retvals->fdev[j]=fdev[j];
            j++;

        }

        printf("\nfdev calculated\n");
        j=1;
        while(j<48) {
            sdev[j-1]=(abs(fdev[j]-fdev[j-1]));
            if(sdev[j-1]<=20)    {
                sdev[j-1]=0;
            }
            retvals->sdev[j-1]=sdev[j-1];

            j++;

        }
        printf("sdev calculated\n");
        int countd1d = 0, curr_cnt = 1, freq_d1d = 7, key = 0,k=0,m=0;
        for(k = 0; k < 48; k++)    {
            key = fdev[k];
            for(m =k+1; m < 49; m++) {
                if(key == fdev[m] && freq_d1d != key)  {
                    curr_cnt++;
                }
            }
            if(countd1d < curr_cnt)    {
                countd1d = curr_cnt; // frequency
                curr_cnt = 1;
                freq_d1d = key; //Most Frequent index
            }
        }


        int countd2d = 0, curr_cnt2 = 1, freq_d2d = 7, key2 = 0,k2=0,m2=0;
        for(k2 = 0; k2 < 47; k2++)    {
            key2 = sdev[k2];
            for(m2 =k2+1; m2 < 48; m2++) {
                if(key2 == sdev[m2] && freq_d2d != key2)  {
                    curr_cnt2++;
                }
            }
            if(countd2d < curr_cnt2)    {
                countd2d = curr_cnt2; // frequency
                curr_cnt2 = 1;
                freq_d2d = key2; //Most Frequent index
            }
        }
        printf("signal type will be written\n");
        if(freq_d1d == 0 && freq_d2d==0){
        writeport('q');
        printf("signal type sent\n");
        }
        if(freq_d1d == 1 && freq_d2d == 0) {
                writeport('t');
                printf("signal type sent\n");
        }
        if(freq_d2d != 1 && freq_d1d != 0 && freq_d2d != 0 && freq_d1d != 1) {
                writeport('s');
                printf("signal type sent\n");
        }





        return retvals;
}*/
