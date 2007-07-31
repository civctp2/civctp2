























#ifndef FREETYPE_H
#define FREETYPE_H


#define TT_FREETYPE_MAJOR  1
#define TT_FREETYPE_MINOR  1


#include "fterrid.h"
#include "ftnameid.h"




#ifndef EXPORT_DEF
#define EXPORT_DEF extern
#endif





#ifndef HAVE_TT_TEXT
#define HAVE_TT_TEXT
  typedef char  TT_Text;              
                                      
#endif

#ifdef __cplusplus
  extern "C" {
#endif


  
  
  
  
  
  
  

  typedef unsigned short  TT_Bool;

  typedef signed long     TT_Fixed;   

  typedef signed short    TT_FWord;   
  typedef unsigned short  TT_UFWord;  

  typedef char            TT_String;
  typedef signed char     TT_Char;
  typedef unsigned char   TT_Byte;
  typedef signed short    TT_Short;
  typedef unsigned short  TT_UShort;
  typedef signed long     TT_Long;
  typedef unsigned long   TT_ULong;

  typedef signed short    TT_F2Dot14; 
                                      
                                      
                                      
                                      
                                      
                                      
                                      
                                      
                                      
                                      
                                      

  typedef signed long     TT_F26Dot6; 
                                      

  typedef signed long     TT_Pos;     
                                      
                                      
                                      
                                      
                                      
                                      
                                      
                                      


  struct  TT_UnitVector_      
  {
    TT_F2Dot14  x;
    TT_F2Dot14  y;
  };

  typedef struct TT_UnitVector_  TT_UnitVector;


  struct  TT_Vector_          
  {
    TT_F26Dot6  x;
    TT_F26Dot6  y;
  };

  typedef struct TT_Vector_  TT_Vector;


  
  
  
  
  
  

  struct  TT_Matrix_
  {
    TT_Fixed  xx, xy;
    TT_Fixed  yx, yy;
  };

  typedef struct TT_Matrix_  TT_Matrix;


  

  struct  TT_Outline_
  {
    TT_Short         n_contours;   
    TT_UShort        n_points;     

    TT_Vector*       points;       
    TT_Byte*         flags;        
    TT_UShort*       contours;     

    
    
    
    

    TT_Bool          owner;      
                                 

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    TT_Bool          high_precision;  
    TT_Bool          second_pass;     
    TT_Char          dropout_mode;    
  };

  typedef struct TT_Outline_  TT_Outline;


  

  struct TT_BBox_
  {
    TT_Pos  xMin;
    TT_Pos  yMin;
    TT_Pos  xMax;
    TT_Pos  yMax;
  };

  typedef struct TT_BBox_  TT_BBox;


  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  struct  TT_Glyph_Metrics_
  {
    TT_BBox  bbox;      

    TT_Pos   bearingX;  
    TT_Pos   bearingY;  

    TT_Pos   advance;   
  };


  
  
  
  
  
  
  
  

  struct TT_Big_Glyph_Metrics_
  {
    TT_BBox  bbox;          

    TT_Pos   horiBearingX;  
    TT_Pos   horiBearingY;  

    TT_Pos   vertBearingX;  
    TT_Pos   vertBearingY;  

    TT_Pos   horiAdvance;   
    TT_Pos   vertAdvance;   

    
    
    
    
    
    
    
    

    TT_Pos   linearHoriBearingX;  
    TT_Pos   linearHoriAdvance;   

    TT_Pos   linearVertBearingY;  
    TT_Pos   linearVertAdvance;   
  };

  typedef struct TT_Glyph_Metrics_      TT_Glyph_Metrics;
  typedef struct TT_Big_Glyph_Metrics_  TT_Big_Glyph_Metrics;


  

  struct  TT_Instance_Metrics_
  {
    TT_F26Dot6  pointSize;     

    TT_UShort   x_ppem;        
    TT_UShort   y_ppem;        

    TT_Fixed    x_scale;     
    TT_Fixed    y_scale;     

    TT_UShort   x_resolution;  
    TT_UShort   y_resolution;  
  };

  typedef struct TT_Instance_Metrics_  TT_Instance_Metrics;


  
  
  
  
  

  
  
  

#define TT_Flow_Down  -1  
#define TT_Flow_Up     1  
#define TT_Flow_Error  0  


  
  
  

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  struct  TT_Raster_Map_
  {
    int    rows;    
    int    cols;    
    int    width;   
    int    flow;    

    void*  bitmap;  
    long   size;    
  };

  typedef struct TT_Raster_Map_  TT_Raster_Map;


  

  struct  TT_Header_
  {
    TT_Fixed   Table_Version;
    TT_Fixed   Font_Revision;

    TT_Long    CheckSum_Adjust;
    TT_Long    Magic_Number;

    TT_UShort  Flags;
    TT_UShort  Units_Per_EM;

    TT_Long    Created [2];
    TT_Long    Modified[2];

    TT_FWord   xMin;
    TT_FWord   yMin;
    TT_FWord   xMax;
    TT_FWord   yMax;

    TT_UShort  Mac_Style;
    TT_UShort  Lowest_Rec_PPEM;

    TT_Short   Font_Direction;
    TT_Short   Index_To_Loc_Format;
    TT_Short   Glyph_Data_Format;
  };

  typedef struct TT_Header_  TT_Header;


  

  
  
  
  

  struct  TT_Horizontal_Header_
  {
    TT_Fixed   Version;
    TT_FWord   Ascender;
    TT_FWord   Descender;
    TT_FWord   Line_Gap;

    TT_UFWord  advance_Width_Max;      

    TT_FWord   min_Left_Side_Bearing;  
    TT_FWord   min_Right_Side_Bearing; 
    TT_FWord   xMax_Extent;            
    TT_FWord   caret_Slope_Rise;
    TT_FWord   caret_Slope_Run;

    TT_Short   Reserved[5];

    TT_Short   metric_Data_Format;
    TT_UShort  number_Of_HMetrics;

    
    
    

    void*      long_metrics;
    void*      short_metrics;
  };


  
  
  
  
  

  struct  TT_Vertical_Header_
  {
    TT_Fixed   Version;
    TT_FWord   Ascender;
    TT_FWord   Descender;
    TT_FWord   Line_Gap;

    TT_UFWord  advance_Height_Max;      

    TT_FWord   min_Top_Side_Bearing;    
    TT_FWord   min_Bottom_Side_Bearing; 
    TT_FWord   yMax_Extent;             
    TT_FWord   caret_Slope_Rise;
    TT_FWord   caret_Slope_Run;
    TT_FWord   caret_Offset;

    TT_Short   Reserved[4];

    TT_Short   metric_Data_Format;
    TT_UShort  number_Of_VMetrics;

    
    
    

    void*      long_metrics;
    void*      short_metrics;
  };


  typedef struct TT_Horizontal_Header_  TT_Horizontal_Header;
  typedef struct TT_Vertical_Header_    TT_Vertical_Header;


  

  struct  TT_OS2_
  {
    TT_UShort  version;                
    TT_FWord   xAvgCharWidth;
    TT_UShort  usWeightClass;
    TT_UShort  usWidthClass;
    TT_Short   fsType;
    TT_FWord   ySubscriptXSize;
    TT_FWord   ySubscriptYSize;
    TT_FWord   ySubscriptXOffset;
    TT_FWord   ySubscriptYOffset;
    TT_FWord   ySuperscriptXSize;
    TT_FWord   ySuperscriptYSize;
    TT_FWord   ySuperscriptXOffset;
    TT_FWord   ySuperscriptYOffset;
    TT_FWord   yStrikeoutSize;
    TT_FWord   yStrikeoutPosition;
    TT_Short   sFamilyClass;

    TT_Byte    panose[10];

    TT_ULong   ulUnicodeRange1;        
    TT_ULong   ulUnicodeRange2;        
    TT_ULong   ulUnicodeRange3;        
    TT_ULong   ulUnicodeRange4;        

    TT_Char    achVendID[4];

    TT_UShort  fsSelection;
    TT_UShort  usFirstCharIndex;
    TT_UShort  usLastCharIndex;
    TT_Short   sTypoAscender;
    TT_Short   sTypoDescender;
    TT_Short   sTypoLineGap;
    TT_UShort  usWinAscent;
    TT_UShort  usWinDescent;

    

    TT_ULong   ulCodePageRange1;       
    TT_ULong   ulCodePageRange2;       
  };

  typedef struct TT_OS2_  TT_OS2;


  

  struct  TT_Postscript_
  {
    TT_Fixed  FormatType;
    TT_Fixed  italicAngle;
    TT_FWord  underlinePosition;
    TT_FWord  underlineThickness;
    TT_ULong  isFixedPitch;
    TT_ULong  minMemType42;
    TT_ULong  maxMemType42;
    TT_ULong  minMemType1;
    TT_ULong  maxMemType1;

    
    
  };

  typedef struct TT_Postscript_  TT_Postscript;


  

  struct  TT_Hdmx_Record_
  {
    TT_Byte   ppem;
    TT_Byte   max_width;
    TT_Byte*  widths;
  };

  typedef struct TT_Hdmx_Record_  TT_Hdmx_Record;


  struct  TT_Hdmx_
  {
    TT_UShort        version;
    TT_Short         num_records;
    TT_Hdmx_Record*  records;
  };

  typedef struct TT_Hdmx_  TT_Hdmx;


  

  struct  TT_Face_Properties_
  {
    TT_UShort  num_Glyphs;      
    TT_UShort  max_Points;      
    TT_UShort  max_Contours;    

    TT_UShort  num_CharMaps;    
    TT_UShort  num_Names;       

    TT_ULong   num_Faces;  
                           
                           

    TT_Header*             header;        
    TT_Horizontal_Header*  horizontal;    
    TT_OS2*                os2;           
    TT_Postscript*         postscript;    
    TT_Hdmx*               hdmx;
    TT_Vertical_Header*    vertical;      
  };

  typedef struct TT_Face_Properties_  TT_Face_Properties;



  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  struct TT_Engine_   { void*  z; };
  struct TT_Stream_   { void*  z; };
  struct TT_Face_     { void*  z; };
  struct TT_Instance_ { void*  z; };
  struct TT_Glyph_    { void*  z; };
  struct TT_CharMap_  { void*  z; };

  typedef struct TT_Engine_    TT_Engine;    
  typedef struct TT_Stream_    TT_Stream;    
  typedef struct TT_Face_      TT_Face;      
  typedef struct TT_Instance_  TT_Instance;  
  typedef struct TT_Glyph_     TT_Glyph;     
  typedef struct TT_CharMap_   TT_CharMap;   

  typedef long  TT_Error;



  
  
  
  
  
  
  
  
  

  

  EXPORT_DEF
  TT_Error  TT_Init_FreeType( TT_Engine*  engine );


  

  EXPORT_DEF
  TT_Error  TT_Done_FreeType( TT_Engine  engine );


  
  
  
  
  
  
  
  
  

  EXPORT_DEF
  TT_Error  TT_Set_Raster_Gray_Palette( TT_Engine  engine,
                                        TT_Byte*   palette );


  

  
  

  
  
  
  
  
  

  EXPORT_DEF
  TT_Error  TT_Open_Face( TT_Engine       engine,
                          const TT_Text*  fontPathName,
                          TT_Face*        face );


  
  

  EXPORT_DEF
  TT_Error  TT_Open_Collection( TT_Engine       engine,
                                const TT_Text*  collectionPathName,
                                TT_ULong        fontIndex,
                                TT_Face*        face );


  

  EXPORT_DEF
  TT_Error  TT_Get_Face_Properties( TT_Face              face,
                                    TT_Face_Properties*  properties );


  

  EXPORT_DEF
  TT_Error  TT_Set_Face_Pointer( TT_Face  face,
                                 void*    data );


  

  EXPORT_DEF
  void*     TT_Get_Face_Pointer( TT_Face  face );


  
  

  EXPORT_DEF
  TT_Error  TT_Flush_Face( TT_Face  face );

  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_Face_Metrics( TT_Face     face,
                                 TT_UShort   firstGlyph,
                                 TT_UShort   lastGlyph,
                                 TT_Short*   leftBearings,
                                 TT_UShort*  widths,
                                 TT_Short*   topBearings,
                                 TT_UShort*  heights );

  
  

  EXPORT_DEF
  TT_Error  TT_Close_Face( TT_Face  face );


  

  EXPORT_DEF
  TT_Error  TT_Get_Font_Data( TT_Face   face,
                              TT_ULong  tag,
                              TT_Long   offset,
                              void*     buffer,
                              TT_Long*  length );




#define MAKE_TT_TAG( _x1, _x2, _x3, _x4 ) \
          (((TT_ULong)_x1 << 24) |        \
           ((TT_ULong)_x2 << 16) |        \
           ((TT_ULong)_x3 << 8)  |        \
            (TT_ULong)_x4)



  

  
  

  EXPORT_DEF
  TT_Error  TT_New_Instance( TT_Face       face,
                             TT_Instance*  instance );


  
  

  EXPORT_DEF
  TT_Error  TT_Set_Instance_Resolutions( TT_Instance  instance,
                                         TT_UShort    xResolution,
                                         TT_UShort    yResolution );


  

  EXPORT_DEF
  TT_Error  TT_Set_Instance_CharSize( TT_Instance  instance,
                                      TT_F26Dot6   charSize );

  EXPORT_DEF
  TT_Error  TT_Set_Instance_CharSizes( TT_Instance  instance,
                                       TT_F26Dot6   charWidth,
                                       TT_F26Dot6   charHeight );

#define TT_Set_Instance_PointSize( ins, ptsize )   \
            TT_Set_Instance_CharSize( ins, ptsize*64 )

  EXPORT_DEF
  TT_Error  TT_Set_Instance_PixelSizes( TT_Instance  instance,
                                        TT_UShort    pixelWidth,
                                        TT_UShort    pixelHeight,
                                        TT_F26Dot6   pointSize );


  
  
  

  EXPORT_DEF
  TT_Error  TT_Set_Instance_Transform_Flags( TT_Instance  instance,
                                             TT_Bool      rotated,
                                             TT_Bool      stretched );


  

  EXPORT_DEF
  TT_Error  TT_Get_Instance_Metrics( TT_Instance           instance,
                                     TT_Instance_Metrics*  metrics );


  

  EXPORT_DEF
  TT_Error  TT_Set_Instance_Pointer( TT_Instance  instance,
                                     void*        data );


  

  EXPORT_DEF
  void*     TT_Get_Instance_Pointer( TT_Instance  instance );


  

  EXPORT_DEF
  TT_Error  TT_Done_Instance( TT_Instance  instance );



  

  

  EXPORT_DEF
  TT_Error  TT_New_Glyph( TT_Face    face,
                          TT_Glyph*  glyph );


  

  EXPORT_DEF
  TT_Error  TT_Done_Glyph( TT_Glyph  glyph );


#define TTLOAD_SCALE_GLYPH  1
#define TTLOAD_HINT_GLYPH   2

#define TTLOAD_DEFAULT  (TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH)


  
  
  
  

  
  
  

  
  
  
  

  
  

  
  
  

  EXPORT_DEF
  TT_Error  TT_Load_Glyph( TT_Instance  instance,
                           TT_Glyph     glyph,
                           TT_UShort    glyphIndex,
                           TT_UShort    loadFlags );


  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_Glyph_Outline( TT_Glyph     glyph,
                                  TT_Outline*  outline );


  

  EXPORT_DEF
  TT_Error  TT_Get_Glyph_Metrics( TT_Glyph           glyph,
                                  TT_Glyph_Metrics*  metrics );


  
  

  EXPORT_DEF
  TT_Error  TT_Get_Glyph_Big_Metrics( TT_Glyph               glyph,
                                      TT_Big_Glyph_Metrics*  metrics );


  

  
  
  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_Glyph_Bitmap( TT_Glyph        glyph,
                                 TT_Raster_Map*  map,
                                 TT_F26Dot6      xOffset,
                                 TT_F26Dot6      yOffset );


  

  
  
  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_Glyph_Pixmap( TT_Glyph        glyph,
                                 TT_Raster_Map*  map,
                                 TT_F26Dot6      xOffset,
                                 TT_F26Dot6      yOffset );



  

  
  

  EXPORT_DEF
  TT_Error  TT_New_Outline( TT_UShort    numPoints,
                            TT_Short     numContours,
                            TT_Outline*  outline );


  

  EXPORT_DEF
  TT_Error  TT_Done_Outline( TT_Outline*  outline );


  

  EXPORT_DEF
  TT_Error  TT_Copy_Outline( TT_Outline*  source,
                             TT_Outline*  target );


  

  EXPORT_DEF
  TT_Error  TT_Get_Outline_Bitmap( TT_Engine       engine,
                                   TT_Outline*     outline,
                                   TT_Raster_Map*  map );


  
  

  EXPORT_DEF
  TT_Error  TT_Get_Outline_Pixmap( TT_Engine       engine,
                                   TT_Outline*     outline,
                                   TT_Raster_Map*  map );


  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_Outline_BBox( TT_Outline*  outline,
                                 TT_BBox*     bbox );


  

  EXPORT_DEF
  void      TT_Transform_Outline( TT_Outline*  outline,
                                  TT_Matrix*   matrix );


  

  EXPORT_DEF
  void      TT_Translate_Outline( TT_Outline*  outline,
                                  TT_F26Dot6   xOffset,
                                  TT_F26Dot6   yOffset );


  

  EXPORT_DEF
  void      TT_Transform_Vector( TT_F26Dot6*  x,
                                 TT_F26Dot6*  y,
                                 TT_Matrix*   matrix );


  

  EXPORT_DEF
  void      TT_Matrix_Multiply( TT_Matrix*  a,
                                TT_Matrix*  b );


  

  EXPORT_DEF
  TT_Error  TT_Matrix_Invert( TT_Matrix*  matrix );


  

  EXPORT_DEF
  TT_Long   TT_MulDiv( TT_Long A, TT_Long B, TT_Long C );


  
  

  EXPORT_DEF
  TT_Long   TT_MulFix( TT_Long A, TT_Long B );



  

  
  
  
  
  
  
  
  
  
  
  

  EXPORT_DEF
  int  TT_Get_CharMap_Count( TT_Face  face );


  
  

  EXPORT_DEF
  TT_Error  TT_Get_CharMap_ID( TT_Face     face,
                               TT_UShort   charmapIndex,
                               TT_UShort*  platformID,
                               TT_UShort*  encodingID );


  
  
  
  
  

  EXPORT_DEF
  TT_Error  TT_Get_CharMap( TT_Face      face,
                            TT_UShort    charmapIndex,
                            TT_CharMap*  charMap );


  
  
  
  

  EXPORT_DEF
  TT_UShort  TT_Char_Index( TT_CharMap  charMap,
                            TT_UShort   charCode );



  

  
  
  
  
  
  
  
  
  
  
  

  EXPORT_DEF
  int  TT_Get_Name_Count( TT_Face  face );


  
  

  EXPORT_DEF
  TT_Error  TT_Get_Name_ID( TT_Face     face,
                            TT_UShort   nameIndex,
                            TT_UShort*  platformID,
                            TT_UShort*  encodingID,
                            TT_UShort*  languageID,
                            TT_UShort*  nameID );


  
  
  

  
  

  EXPORT_DEF
  TT_Error  TT_Get_Name_String( TT_Face      face,
                                TT_UShort    nameIndex,
                                TT_String**  stringPtr, 
                                TT_UShort*   length );  



#ifdef __cplusplus
  }
#endif

#endif 



