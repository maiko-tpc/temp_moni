#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

int main(int argc, char * argv[]){

  int sockfd ;
  int len ;
  int i;
  struct sockaddr_in address ;
  char command[256];
  int result ;
  int result2;
  int data_length;
  char data[256];
  char data2[256];
  char header[32];
  char buff[256];
  long buff2;
  char *endptr;
  long ADC_val[16];

  float temp[16];

  int rec_int;

  char *outfile;

  double p_offset;
  p_offset=1.285; // measured on 2014/12/04 at new_SUBARU

  if(argc == 2) rec_int=60;
  if(argc == 3) rec_int=atoi(argv[2]);

  //  printf("%d \n", rec_int);

  while(1){

    bzero((char*)&address, sizeof(address));
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //  printf("id= %d \n", sockfd);
    address.sin_family = AF_INET ;
    address.sin_addr.s_addr =inet_addr("172.16.209.229"); // RCNP from 2020
    //    address.sin_addr.s_addr =inet_addr("192.168.253.59"); // Cyric fam3-gagg
    address.sin_port =htons(12289);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr*)&address, len);
    //  printf("result= %d\n", result);
    
    if ( result == -1 ) {
      perror("oops: client");
    return 0;
    }
    
    //  send(sockfd, "01TSThogehoge\r\n", 256, 0); /* for test */
    
    /* Read ADC data (D00001 ~ D00008)*/ 
    send(sockfd, "01WRDD00101,17\r\n", 256, 0);
    data_length=recv(sockfd, &data, 256, 0);
    //    close(sockfd);
    strncpy(header, data, 4);

    /* Check if the header data is correct */
    if(strstr(header, "11OK") == NULL) exit(0);

    /* Read ADC data (D00001 ~ D00008)*/ 
    send(sockfd, "01WRDD00301,17\r\n", 256, 0);
    data_length=recv(sockfd, &data2, 256, 0);
    close(sockfd);
    strncpy(header, data2, 4);
    
    /* Check if the header data is correct */
    if(strstr(header, "11OK") == NULL) exit(0);
    
    /* Convert hex string to dec integer */
    ADC_val[0]= -10000;
    
    sprintf(buff, "0x%c%c%c%c", data[4], data[5],
	    data[6], data[7]);
    
    buff2= strtol(buff, &endptr, 16);
    ADC_val[0]= buff2;
    //    temp[0]=ADC_val[0]*0.1;
    temp[0]=ADC_val[0]*0.01;

    /* Convert hex string to dec integer */
    ADC_val[1]= -10000;
    
    sprintf(buff, "0x%c%c%c%c", data2[4], data2[5],
	    data2[6], data2[7]);
    
    buff2= strtol(buff, &endptr, 16);
    ADC_val[1]= buff2;
    //    temp[1]=ADC_val[1]*0.1;
    temp[1]=ADC_val[1]*0.01;
      
    
    
    
    /* File output */
    outfile = argv[1];
    
    FILE *output;
    output = fopen(outfile, "a");
    if (output == NULL){
      printf("output file open error! \n");
      exit(1);
    }
    
    time_t timer;
    struct tm *local;
    timer = time(NULL);
    local = localtime(&timer);
    
    //  fprintf(output, "%4d/", local->tm_year + 1900);
    //  fprintf(output, "%2d/", local->tm_mon + 1);
    //  fprintf(output, "%2d ", local->tm_mday);
    //  fprintf(output, "%2d:", local->tm_hour);
    //  fprintf(output, "%2d:", local->tm_min);
    //  fprintf(output, "%2d   ", local->tm_sec);
    
    fprintf(output, "%d ", timer);
    
    fprintf(output, "%f %f \n", temp[0], temp[1]);
    //    fprintf(output, "%f ", temp);
    //    fprintf(output, "%f ", density);
    //    fprintf(output, "%f ", dew);
    //    fprintf(output, "%f \n", dew_ppm);
    fclose(output);

    sleep(rec_int);
  }
  return 0;
}
