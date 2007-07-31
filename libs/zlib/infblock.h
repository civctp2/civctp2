









struct inflate_blocks_state;
typedef struct inflate_blocks_state FAR inflate_blocks_statef;

extern inflate_blocks_statef * inflate_blocks_new OF((
    z_streamp z,
    check_func c,               
    uInt w));                   

extern int inflate_blocks OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));                      

extern void inflate_blocks_reset OF((
    inflate_blocks_statef *,
    z_streamp ,
    uLongf *));                  

extern int inflate_blocks_free OF((
    inflate_blocks_statef *,
    z_streamp));

extern void inflate_set_dictionary OF((
    inflate_blocks_statef *s,
    const Bytef *d,  
    uInt  n));       

extern int inflate_blocks_sync_point OF((
    inflate_blocks_statef *s));
