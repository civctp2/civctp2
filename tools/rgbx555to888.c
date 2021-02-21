#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char* argv[]){

    unsigned char buf[64];

    FILE* fp=fopen(argv[1],"rb");    
    if(fp==NULL){
       fprintf(stderr,"ERROR: Unable to open %s\n",argv[1]);
       exit(1);
    }

    // Read TGA header of 18 bytes, extract width and height
    fread(buf,1,18,fp);  // 12 bytes junk, 2 bytes width, 2 bytes height, 2 bytes junk
    unsigned short w=buf[12]|(buf[13]<<8);
    unsigned short h=buf[14]|(buf[15]<<8);

    // Write PPM header
    fprintf(stdout,"P6\n");
    fprintf(stdout,"%d %d\n",w,h);
    fprintf(stdout,"255\n");

    // Read 2 bytes at a time RGB0555
    while(fread(buf,2,1,fp)==1){
       unsigned char out[3];
       out[0]=buf[1]&0x7c; // 0111 1100 => x5r
       out[1]=((buf[1]&3)<<6) | ((buf[0]&0xe0)>>2); // 0000 0011 1110 0000 => 5g
       out[2]=(buf[0]&0x1f)<<3; // 0001 1111 => 5b
       // Write the 3 modified bytes out RGB888
       fwrite(out,3,1,stdout);
    }
    fclose(fp);
    return 0;
}
