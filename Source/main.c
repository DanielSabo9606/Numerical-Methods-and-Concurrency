#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Include/stb_image_write.h"

#include "Include/clparse.h"

float* find_brightest_ave(float* image, int image_size) {
  //initialise brightest list
  float* list = malloc(3000*sizeof(float));
  float* brightest = malloc(3*sizeof(float));
  int i,j;
  for (i=0; i<3000; i++) list[i] = 0.0;
  //read through the image looking for 1000 brightest pixels
  float tp[3];
  for (i=0; i<image_size; i+=3) {
    list[0] = *(image+i);
    list[1] = *(image+(i+1));
    list[2] = *(image+(i+2));
    // don't process saturated pixels
    if ((list[0]<0.99) && (list[1]<0.99) && (list[2]<0.99)) {
      //do sort
      j = 0;
      while ((j<2994) && ((list[j]+list[j+1]+list[j+2])>(list[j+3]+list[j+4]+list[j+5]))) {
          tp[0] = list[j]; tp[1] = list[j+1]; tp[2] = list[j+2];
          list[j] = list[j+3]; list[j+1] = list[j+4]; list[j+2]=list[j+5];
          list[j+3] = tp[0]; list[j+4] = tp[1]; list[j+5] = tp[2];
          j+=3;
      }
    }
  }
  brightest[0] = 0.0; brightest[2] = 0.0; brightest[3] = 0.0;
  for (i=3; i<3000; i+=3) {
    brightest[0] += list[i]; brightest[1] += list[i+1]; brightest[2] += list[i+2];
  }
  brightest[0]=brightest[0]/999.0; brightest[1]=brightest[1]/999.0; brightest[2]=brightest[2]/999.0;
  free(list);
  return brightest;
}

int cinstr(char c, char* str) {
  char* p=str;
  while (*p!='\0') {
    if (*p==c) return !(0);
    p++;
  }
  return 0;
}

int main(int argc, char* argv[]) {

  char cl[1000];
  int ok = args2str(argc, argv, sizeof(cl), cl);
  char* inptr = cl;

   long long threadCount = atoll(argv[3]); //creates an array
   long long sliceList[threadCount]; //  this will hold the number of computations based on number of slices
   long long startList[threadCount]; // and this holds  the value of the start of the array
   long long endList[threadCount];//  this holds the last value of the array
   long long *result;// holds the result

  char filestr[100];
  char extstr[100];
  char switchstr[100];

  // check for valid file name
  if (!cl_parameters(&inptr, filestr, extstr, switchstr)) {
    printf("Invalid command line\n");
    return 1;
  }
  // construct file name
  char filename[100];
  strcpy(filename, filestr);
  if (strlen(extstr)>0) {
    strcat(filename,".");
    strcat(filename, extstr);
  }
  // open image file
  int x,y,n;
  unsigned char* image = stbi_load(filename, &x, &y, &n, 0);
  printf("%s\n", filename);//prints file name as a char
  printf("%d\n", x);// prints the value x as int
  printf("%d\n", y);// prints the value y as int
  printf("%d\n", n);// prints the value n as int
  if (image==NULL) { printf("nothing to load\n"); return 1; }
  // translate image to floating point
  float* flimage = malloc(x*y*n*sizeof(float));
  int i;
  for (i=0; i<(x*y*n); i++) {
    *(flimage+i) = ((float) *(image+i))/255.0;
  }
  long long remainder = (x*y*n) % threadCount; // this remainder will hold the left over threads
  long long generalSlice = (x*y*n) / threadCount; // this will  slice the number of threads
  for (long long i = 0; i < threadCount; i++){ // its a  for loop that increments i by 1 until its no longer less then
	sliceList[i] = generalSlice;
  }


  for(long long i = 0; i < threadCount; i++){
                sliceList[i] = generalSlice;
        }

        for(long long i = 0; i < remainder; i++){
                sliceList[i] = sliceList[i] + 1;
        }

        for(long long i = 0; i < threadCount; i++){
                //printf("Slice List = %lld\n", sliceList[i]);
        }

        for(long long i = 0; i < threadCount; i++){ //creates start and end list
                if(i == 0){
                        startList[i] = 1;
                        endList[i] = sliceList[i];
                }else{
                        startList[i] = endList[i-1] + 1;
                        endList[i] = endList[i-1] + sliceList[i];
                }
        }

  // find brightest pixels
  float* brightest = find_brightest_ave(flimage, x*y*n);
  // work out correction needed



  float ave = (brightest[0] + brightest[1] + brightest[2])/3.0;
  float rcorr = ave/brightest[0];
  float gcorr = ave/brightest[1];
  float bcorr = ave/brightest[2];
  float iRed = -1.0 - rcorr;// takes the red colour and makes it -1
  float iGreen = -1.0 - gcorr;// takes the red green and makes it -1
  float iBlue = -1.0 - bcorr;// takes the red blue and makes it -1`
  printf("%f\n", rcorr);// prints the value iRed as a double
  printf("%f\n", gcorr);// prints the value iGreen as a double
  printf("%f\n", bcorr);// prints the value iBlue as a double
  // correct the image
  /*for (i=0; i<(x*y*n); i+=3) {
    *(flimage+i) = *(flimage+i)*invertRed;
    *(flimage+i+1) = *(flimage+i+1)*invertGreen;
    *(flimage+i+2) = *(flimage+i+2)*invertBlue;
  }*/

  for (int u = 0; u<threadCount; u++) { // this is a  for loop and it will loop until u is no longer less then threadcount
	for (i=startList[u]; i<endList[u]; i+=3) { // for loop that will  sets i as a value of startList and increments it until it is no longer less then end List.
    *(flimage+i) = *(flimage+i)*iRed;
    *(flimage+i+1) = *(flimage+i+1)*iGreen;
    *(flimage+i+2) = *(flimage+i+2)*iBlue;
	}
  }
  // now copy back to bytes
  for (i=0; i<(x*y*n); i++) *(image+i) = (unsigned char) (*(flimage+i)*255.0);
  // check output format
  // make root of output filename
  strcpy(filename,filestr);
  strcat(filename,"[out]");
  if (cinstr('p', switchstr)||cinstr('P', switchstr)) {
    strcat(filename,".png");
    stbi_write_png(filename, x, y, n, image, x*n);
  } else if (cinstr('b', switchstr)||cinstr('B', switchstr)) {
    strcat(filename,".bmp");
    stbi_write_bmp(filename, x, y, n, image);
  } else { //default to JPEG
    strcat(filename,".jpeg");
    stbi_write_jpg(filename, x, y, n, image, 75);
  }



  free(image);
  free(flimage);
  free(brightest);
}

// end of main function definition



