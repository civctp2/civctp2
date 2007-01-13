









#ifndef _INFUTIL_H
#define _INFUTIL_H

typedef enum {
      TYPE,     
      LENS,     
      STORED,   
      TABLE,    
      BTREE,    
      DTREE,    
      CODES,    
      DRY,      
      DONE,     
      BAD}      
inflate_block_mode;


struct inflate_blocks_state {

  
  inflate_block_mode  mode;     

  
  union {
    uInt left;          
    struct {
      uInt table;               
      uInt index;               
      uIntf *blens;             
      uInt bb;                  
      inflate_huft *tb;         
    } trees;            
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           
  } sub;                
  uInt last;            

  
  uInt bitk;            
  uLong bitb;           
  inflate_huft *hufts;  
  Bytef *window;        
  Bytef *end;           
  Bytef *read;          
  Bytef *write;         
  check_func checkfn;   
  uLong check;          

};




#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}

#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}

#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}

#define LOAD {LOADIN LOADOUT}


extern uInt inflate_mask[17];


extern int inflate_flush OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));

struct internal_state      {int dummy;}; 

#endif
