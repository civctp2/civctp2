/*******************************************************************
 *
 *  ftxgsub.c
 *
 *    TrueType Open GSUB table support.
 *
 *  Copyright 1996-1999 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

/* XXX There is *a lot* of duplicated code (cf. formats 5 and 6), but
       I don't care currently.  I believe that it would be possible to
       save about 50% of TTO code by carefully designing the structures,
       sharing as much as possible with extensive use of macros.  This
       is something for a volunteer :-)                                  */

#include "tttypes.h"
#include "tttags.h"
#include "ttload.h"
#include "ttextend.h"
#include "ttmemory.h"
#include "ttfile.h"

#include "ftxopen.h"
#include "ftxopenf.h"


#define GSUB_ID  Build_Extension_ID( 'G', 'S', 'U', 'B' )


#define ADD_String( in, num_in, out, num_out, data )                \
          ( ( error = TT_GSUB_Add_String( (in), (num_in),           \
                                          (out), (num_out),         \
                                          (data) ) ) != TT_Err_Ok )

#define CHECK_Property( gdef, index, flags, property )              \
          ( ( error = Check_Property( (gdef), (index), (flags),     \
                                      (property) ) ) != TT_Err_Ok )


  static TT_Error  Do_Glyph_Lookup( TTO_GSUBHeader*   gsub,
                                    UShort            lookup_index,
                                    TTO_GSUB_String*  in,
                                    TTO_GSUB_String*  out,
                                    UShort            context_length,
                                    int               nesting_level );



  /**********************
   * Auxiliary functions
   **********************/


  /* The following function copies `num_out' elements from `data' to
     `out', advancing the array pointer in the `in' structure by `num_in'
     elements and in `out' by `num_out' elements.  If the string (resp.
     the properties) array in `out' is empty or too small, it allocates
     resp. reallocates the string (and properties) array.  Finally, it
     sets the `length' field of `out' equal to `pos' of the `out'
     structure.

     The properties (if defined) for all replaced glyphs are taken from
     the glyph at position `in->pos'.                                     */

  EXPORT_FUNC
  TT_Error  TT_GSUB_Add_String( TTO_GSUB_String*  in,
                                UShort            num_in,
                                TTO_GSUB_String*  out,
                                UShort            num_out,
                                UShort*           data )
  {
    TT_Error  error;
    UShort    i;
    UShort    p_in;
    UShort*   p_out;


    /* sanity check */

    if ( !in || !out ||
         in->length == 0 || in->pos >= in->length ||
         in->length < in->pos + num_in )
      return TT_Err_Invalid_Argument;

    if ( out->pos + num_out >= out->allocated )
    {
      ULong  size = out->pos + num_out + 256L;


      /* The following works because all fields in `out' must be
         initialized to zero (including the `string' field) for the
         first use.                                               */

      if ( REALLOC( out->string, size * sizeof ( UShort ) ) )
        return error;
      if ( in->properties )
        if ( REALLOC( out->properties, size * sizeof ( UShort ) ) )
          return error;
      out->allocated = size;
    }

    if ( num_out )
    {
      MEM_Copy( &out->string[out->pos], data, num_out * sizeof ( UShort ) );
      if ( in->properties )
      {
        p_in  = in->properties[in->pos];
        p_out = out->properties;

        for ( i = out->pos; i < out->pos + num_out; i++ )
          p_out[i] = p_in;
      }
    }

    in->pos  += num_in;
    out->pos += num_out;

    out->length = out->pos;

    return TT_Err_Ok;
  }


  static TT_Error  Check_Property( TTO_GDEFHeader*  gdef,
                                   UShort           index,
                                   UShort           flags,
                                   UShort*          property )
  {
    TT_Error  error;


    if ( gdef )
    {
      error = TT_GDEF_Get_Glyph_Property( gdef, index, property );
      if ( error )
        return error;

      /* This is OpenType 1.2 */

      if ( flags & IGNORE_SPECIAL_MARKS )
        if ( (flags & 0xFF00) != *property )
          return TTO_Err_Not_Covered;

      if ( flags & *property )
        return TTO_Err_Not_Covered;
    }

    return TT_Err_Ok;
  }



  /**********************
   * Extension Functions
   **********************/


  static TT_Error  GSUB_Create( void*  ext,
                                PFace  face )
  {
    DEFINE_LOAD_LOCALS( face->stream );

    TTO_GSUBHeader*  gsub = (TTO_GSUBHeader*)ext;
    Long             table;


    /* by convention */

    if ( !gsub )
      return TT_Err_Ok;

    /* a null offset indicates that there is no GSUB table */

    gsub->offset = 0;

    /* we store the start offset and the size of the subtable */

    table = TT_LookUp_Table( face, TTAG_GSUB );
    if ( table < 0 )
      return TT_Err_Ok;             /* The table is optional */

    if ( FILE_Seek( face->dirTables[table].Offset ) ||
         ACCESS_Frame( 4L ) )
      return error;

    gsub->offset  = FILE_Pos() - 4L;    /* undo ACCESS_Frame() */
    gsub->Version = GET_ULong();

    FORGET_Frame();

    gsub->loaded = FALSE;

    return TT_Err_Ok;
  }


  static TT_Error  GSUB_Destroy( void*  ext,
                                 PFace  face )
  {
    TTO_GSUBHeader*  gsub = (TTO_GSUBHeader*)ext;


    /* by convention */

    if ( !gsub )
      return TT_Err_Ok;

    if ( gsub->loaded )
    {
      Free_LookupList( &gsub->LookupList, GSUB );
      Free_FeatureList( &gsub->FeatureList );
      Free_ScriptList( &gsub->ScriptList );
    }

    return TT_Err_Ok;
  }


  EXPORT_FUNC
  TT_Error  TT_Init_GSUB_Extension( TT_Engine  engine )
  {
    PEngine_Instance  _engine = HANDLE_Engine( engine );


    if ( !_engine )
      return TT_Err_Invalid_Engine;

    return  TT_Register_Extension( _engine,
                                   GSUB_ID,
                                   sizeof ( TTO_GSUBHeader ),
                                   GSUB_Create,
                                   GSUB_Destroy );
  }


  EXPORT_FUNC
  TT_Error  TT_Load_GSUB_Table( TT_Face          face,
                                TTO_GSUBHeader*  retptr,
                                TTO_GDEFHeader*  gdef )
  {
    ULong            cur_offset, new_offset, base_offset;

    TT_UShort        i, num_lookups;
    TT_Error         error;
    TT_Stream        stream;
    TTO_GSUBHeader*  gsub;
    TTO_Lookup*      lo;

    PFace  faze = HANDLE_Face( face );


    if ( !retptr )
      return TT_Err_Invalid_Argument;

    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    error = TT_Extension_Get( faze, GSUB_ID, (void**)&gsub );
    if ( error )
      return error;

    if ( gsub->offset == 0 )
      return TT_Err_Table_Missing;      /* no GSUB table; nothing to do */

    /* now access stream */

    if ( USE_Stream( faze->stream, stream ) )
      return error;

    base_offset = gsub->offset;

    /* skip version */

    if ( FILE_Seek( base_offset + 4L ) ||
         ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_ScriptList( &gsub->ScriptList,
                                    faze ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_FeatureList( &gsub->FeatureList,
                                     faze ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_LookupList( &gsub->LookupList,
                                    faze, GSUB ) ) != TT_Err_Ok )
      goto Fail2;

    gsub->gdef = gdef;      /* can be NULL */

    /* We now check the LookupFlags for values larger than 0xFF to find
       out whether we need to load the `MarkAttachClassDef' field of the
       GDEF table -- this hack is necessary for OpenType 1.2 tables since
       the version field of the GDEF table hasn't been incremented.

       For constructed GDEF tables, we only load it if
       `MarkAttachClassDef_offset' is not zero (nevertheless, a build of
       a constructed mark attach table is not supported currently).       */

    if ( gdef &&
         gdef->MarkAttachClassDef_offset && !gdef->MarkAttachClassDef.loaded )
    {
      lo          = gsub->LookupList.Lookup;
      num_lookups = gsub->LookupList.LookupCount;

      for ( i = 0; i < num_lookups; i++ )
      {
        if ( lo[i].LookupFlag & IGNORE_SPECIAL_MARKS )
        {
          if ( FILE_Seek( gdef->MarkAttachClassDef_offset ) ||
               ACCESS_Frame( 2L ) )
            goto Fail1;

          new_offset = GET_UShort();

          FORGET_Frame();

          if ( !new_offset )
            return TTO_Err_Invalid_GDEF_SubTable;

          new_offset += base_offset;

          if ( FILE_Seek( new_offset ) ||
               ( error = Load_ClassDefinition( &gdef->MarkAttachClassDef,
                                               256, faze ) ) != TT_Err_Ok )
            goto Fail1;

          break;
        }
      }
    }

    gsub->loaded = TRUE;
    *retptr = *gsub;
    DONE_Stream( stream );

    return TT_Err_Ok;

  Fail1:
    Free_LookupList( &gsub->LookupList, GSUB );

  Fail2:
    Free_FeatureList( &gsub->FeatureList );

  Fail3:
    Free_ScriptList( &gsub->ScriptList );

    /* release stream */

    DONE_Stream( stream );

    return error;
  }



  /*****************************
   * SubTable related functions
   *****************************/


  /* LookupType 1 */

  /* SingleSubstFormat1 */
  /* SingleSubstFormat2 */

  TT_Error  Load_SingleSubst( TTO_SingleSubst*  ss,
                              PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;
    ULong    cur_offset, new_offset, base_offset;

    UShort*  s;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    ss->SubstFormat = GET_UShort();
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ss->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    switch ( ss->SubstFormat )
    {
    case 1:
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      ss->ssf.ssf1.DeltaGlyphID = GET_UShort();

      FORGET_Frame();

      break;

    case 2:
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      count = ss->ssf.ssf2.GlyphCount = GET_UShort();

      FORGET_Frame();

      ss->ssf.ssf2.Substitute = NULL;

      if ( ALLOC_ARRAY( ss->ssf.ssf2.Substitute, count, UShort ) )
        goto Fail2;

      s = ss->ssf.ssf2.Substitute;

      if ( ACCESS_Frame( count * 2L ) )
        goto Fail1;

      for ( n = 0; n < count; n++ )
        s[n] = GET_UShort();

      FORGET_Frame();

      break;

    default:
      return TTO_Err_Invalid_GSUB_SubTable_Format;
    }

    return TT_Err_Ok;

  Fail1:
    FREE( s );

  Fail2:
    Free_Coverage( &ss->Coverage );
    return error;
  }


  void  Free_SingleSubst( TTO_SingleSubst*  ss )
  {
    switch ( ss->SubstFormat )
    {
    case 1:
      break;

    case 2:
      FREE( ss->ssf.ssf2.Substitute );
      break;
    }

    Free_Coverage( &ss->Coverage );
  }


  static TT_Error  Lookup_SingleSubst( TTO_SingleSubst*  ss,
                                       TTO_GSUB_String*  in,
                                       TTO_GSUB_String*  out,
                                       UShort            flags,
                                       UShort            context_length,
                                       TTO_GDEFHeader*   gdef )
  {
    UShort    index, value[1], property;
    TT_Error  error;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &ss->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    switch ( ss->SubstFormat )
    {
    case 1:
      value[0] = ( in->string[in->pos] + ss->ssf.ssf1.DeltaGlyphID ) & 0xFFFF;
      if ( ADD_String( in, 1, out, 1, value ) )
        return error;
      break;

    case 2:
      if ( index >= ss->ssf.ssf2.GlyphCount )
        return TTO_Err_Invalid_GSUB_SubTable;
      value[0] = ss->ssf.ssf2.Substitute[index];
      if ( ADD_String( in, 1, out, 1, value ) )
        return error;
      break;

    default:
      return TTO_Err_Invalid_GSUB_SubTable;
    }

    if ( gdef && gdef->NewGlyphClasses )
    {
      /* we inherit the old glyph class to the substituted glyph */

      error = Add_Glyph_Property( gdef, value[0], property );
      if ( error && error != TTO_Err_Not_Covered )
        return error;
    }

    return TT_Err_Ok;
  }


  /* LookupType 2 */

  /* Sequence */

  static TT_Error  Load_Sequence( TTO_Sequence*  s,
                                  PFace          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;
    UShort*  sub;


    if ( ACCESS_Frame( 2L ) )
      return error;

    count = s->GlyphCount = GET_UShort();

    FORGET_Frame();

    s->Substitute = NULL;

    if ( count )
    {
      if ( ALLOC_ARRAY( s->Substitute, count, UShort ) )
        return error;

      sub = s->Substitute;

      if ( ACCESS_Frame( count * 2L ) )
      {
        FREE( sub );
        return error;
      }

      for ( n = 0; n < count; n++ )
        sub[n] = GET_UShort();

      FORGET_Frame();
    }

    return TT_Err_Ok;
  }


  static void  Free_Sequence( TTO_Sequence*  s )
  {
    FREE( s->Substitute );
  }


  /* MultipleSubstFormat1 */

  TT_Error  Load_MultipleSubst( TTO_MultipleSubst*  ms,
                                PFace               input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort         n, count;
    ULong          cur_offset, new_offset, base_offset;

    TTO_Sequence*  s;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    ms->SubstFormat = GET_UShort();             /* should be 1 */
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ms->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = ms->SequenceCount = GET_UShort();

    FORGET_Frame();

    ms->Sequence = NULL;

    if ( ALLOC_ARRAY( ms->Sequence, count, TTO_Sequence ) )
      goto Fail2;

    s = ms->Sequence;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Sequence( &s[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_Sequence( &s[n] );

    FREE( s );

  Fail2:
    Free_Coverage( &ms->Coverage );
    return error;
  }


  void  Free_MultipleSubst( TTO_MultipleSubst*  ms )
  {
    UShort         n, count;

    TTO_Sequence*  s;


    if ( ms->Sequence )
    {
      count = ms->SequenceCount;
      s     = ms->Sequence;

      for ( n = 0; n < count; n++ )
        Free_Sequence( &s[n] );

      FREE( s );
    }

    Free_Coverage( &ms->Coverage );
  }


  static TT_Error  Lookup_MultipleSubst( TTO_MultipleSubst*  ms,
                                         TTO_GSUB_String*    in,
                                         TTO_GSUB_String*    out,
                                         UShort              flags,
                                         UShort              context_length,
                                         TTO_GDEFHeader*     gdef )
  {
    TT_Error  error;
    UShort    index, property, n, count;
    UShort*   s;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &ms->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    if ( index >= ms->SequenceCount )
      return TTO_Err_Invalid_GSUB_SubTable;

    count = ms->Sequence[index].GlyphCount;
    s     = ms->Sequence[index].Substitute;

    if ( ADD_String( in, 1, out, count, s ) )
      return error;

    if ( gdef && gdef->NewGlyphClasses )
    {
      /* this is a guess only ... */

      if ( property == TTO_LIGATURE )
        property = TTO_BASE_GLYPH;

      for ( n = 0; n < count; n++ )
      {
        error = Add_Glyph_Property( gdef, s[n], property );
        if ( error && error != TTO_Err_Not_Covered )
          return error;
      }
    }

    return TT_Err_Ok;
  }


  /* LookupType 3 */

  /* AlternateSet */

  static TT_Error  Load_AlternateSet( TTO_AlternateSet*  as,
                                      PFace              input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;
    UShort*  a;


    if ( ACCESS_Frame( 2L ) )
      return error;

    count = as->GlyphCount = GET_UShort();

    FORGET_Frame();

    as->Alternate = NULL;

    if ( ALLOC_ARRAY( as->Alternate, count, UShort ) )
      return error;

    a = as->Alternate;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( a );
      return error;
    }

    for ( n = 0; n < count; n++ )
      a[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  static void  Free_AlternateSet( TTO_AlternateSet*  as )
  {
    FREE( as->Alternate );
  }


  /* AlternateSubstFormat1 */

  TT_Error  Load_AlternateSubst( TTO_AlternateSubst*  as,
                                 PFace                input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_AlternateSet*  aset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    as->SubstFormat = GET_UShort();             /* should be 1 */
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &as->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = as->AlternateSetCount = GET_UShort();

    FORGET_Frame();

    as->AlternateSet = NULL;

    if ( ALLOC_ARRAY( as->AlternateSet, count, TTO_AlternateSet ) )
      goto Fail2;

    aset = as->AlternateSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_AlternateSet( &aset[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_AlternateSet( &aset[n] );

    FREE( aset );

  Fail2:
    Free_Coverage( &as->Coverage );
    return error;
  }


  void  Free_AlternateSubst( TTO_AlternateSubst*  as )
  {
    UShort             n, count;

    TTO_AlternateSet*  aset;


    if ( as->AlternateSet )
    {
      count = as->AlternateSetCount;
      aset  = as->AlternateSet;

      for ( n = 0; n < count; n++ )
        Free_AlternateSet( &aset[n] );

      FREE( aset );
    }

    Free_Coverage( &as->Coverage );
  }


  static TT_Error  Lookup_AlternateSubst( TTO_GSUBHeader*      gsub,
                                          TTO_AlternateSubst*  as,
                                          TTO_GSUB_String*     in,
                                          TTO_GSUB_String*     out,
                                          UShort               flags,
                                          UShort               context_length,
                                          TTO_GDEFHeader*      gdef )
  {
    TT_Error          error;
    UShort            index, alt_index, property;

    TTO_AlternateSet  aset;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &as->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    aset = as->AlternateSet[index];

    /* we use a user-defined callback function to get the alternate index */

    if ( gsub->alt )
      alt_index = (gsub->alt)( out->pos, in->string[in->pos],
                               aset.GlyphCount, aset.Alternate,
                               gsub->data );
    else
      alt_index = 0;

    if ( ADD_String( in, 1, out, 1, &aset.Alternate[alt_index] ) )
      return error;

    if ( gdef && gdef->NewGlyphClasses )
    {
      /* we inherit the old glyph class to the substituted glyph */

      error = Add_Glyph_Property( gdef, aset.Alternate[alt_index],
                                  property );
      if ( error && error != TTO_Err_Not_Covered )
        return error;
    }

    return TT_Err_Ok;
  }


  /* LookupType 4 */

  /* Ligature */

  static TT_Error  Load_Ligature( TTO_Ligature*  l,
                                  PFace          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;
    UShort*  c;


    if ( ACCESS_Frame( 4L ) )
      return error;

    l->LigGlyph       = GET_UShort();
    l->ComponentCount = GET_UShort();

    FORGET_Frame();

    l->Component = NULL;

    count = l->ComponentCount - 1;      /* only ComponentCount - 1 elements */

    if ( ALLOC_ARRAY( l->Component, count, UShort ) )
      return error;

    c = l->Component;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( c );
      return error;
    }

    for ( n = 0; n < count; n++ )
      c[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  static void  Free_Ligature( TTO_Ligature*  l )
  {
    FREE( l->Component );
  }


  /* LigatureSet */

  static TT_Error  Load_LigatureSet( TTO_LigatureSet*  ls,
                                     PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort         n, count;
    ULong          cur_offset, new_offset, base_offset;

    TTO_Ligature*  l;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ls->LigatureCount = GET_UShort();

    FORGET_Frame();

    ls->Ligature = NULL;

    if ( ALLOC_ARRAY( ls->Ligature, count, TTO_Ligature ) )
      return error;

    l = ls->Ligature;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Ligature( &l[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_Ligature( &l[n] );

    FREE( l );
    return error;
  }


  static void  Free_LigatureSet( TTO_LigatureSet*  ls )
  {
    UShort         n, count;

    TTO_Ligature*  l;


    if ( ls->Ligature )
    {
      count = ls->LigatureCount;
      l     = ls->Ligature;

      for ( n = 0; n < count; n++ )
        Free_Ligature( &l[n] );

      FREE( l );
    }
  }


  /* LigatureSubstFormat1 */

  TT_Error  Load_LigatureSubst( TTO_LigatureSubst*  ls,
                                PFace               input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count;
    ULong             cur_offset, new_offset, base_offset;

    TTO_LigatureSet*  lset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    ls->SubstFormat = GET_UShort();             /* should be 1 */
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ls->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = ls->LigatureSetCount = GET_UShort();

    FORGET_Frame();

    ls->LigatureSet = NULL;

    if ( ALLOC_ARRAY( ls->LigatureSet, count, TTO_LigatureSet ) )
      goto Fail2;

    lset = ls->LigatureSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LigatureSet( &lset[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_LigatureSet( &lset[n] );

    FREE( lset );

  Fail2:
    Free_Coverage( &ls->Coverage );
    return error;
  }


  void  Free_LigatureSubst( TTO_LigatureSubst*  ls )
  {
    UShort            n, count;

    TTO_LigatureSet*  lset;


    if ( ls->LigatureSet )
    {
      count = ls->LigatureSetCount;
      lset  = ls->LigatureSet;

      for ( n = 0; n < count; n++ )
        Free_LigatureSet( &lset[n] );

      FREE( lset );
    }

    Free_Coverage( &ls->Coverage );
  }


  static TT_Error  Lookup_LigatureSubst( TTO_LigatureSubst*  ls,
                                         TTO_GSUB_String*    in,
                                         TTO_GSUB_String*    out,
                                         UShort              flags,
                                         UShort              context_length,
                                         TTO_GDEFHeader*     gdef )
  {
    UShort         index, property;
    TT_Error       error;
    UShort         numlig, i, j;
    UShort*        s_in;
    UShort*        c;

    TTO_Ligature*  lig;


    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &ls->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    if ( index >= ls->LigatureSetCount )
       return TTO_Err_Invalid_GSUB_SubTable;

    lig = ls->LigatureSet[index].Ligature;

    for ( numlig = ls->LigatureSet[index].LigatureCount;
          numlig;
          numlig--, lig++ )
    {
      if ( in->pos + lig->ComponentCount > in->length )
        continue;                         /* Not enough glyphs in input */

      s_in = &in->string[in->pos];
      c    = lig->Component;

      if ( context_length != 0xFFFF && context_length < lig->ComponentCount )
        break;

      for ( i = 1, j = 1; i < lig->ComponentCount; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( in->pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != c[i - 1] )
          break;
      }

      if ( i == lig->ComponentCount )
      {
        if ( ADD_String( in, lig->ComponentCount, out, 1, &lig->LigGlyph ) )
          return error;

        if ( gdef && gdef->NewGlyphClasses )
        {
          /* this is just a guess ... */

          error = Add_Glyph_Property( gdef, lig->LigGlyph, TTO_LIGATURE );
          if ( error && error != TTO_Err_Not_Covered )
            return error;
        }

        return TT_Err_Ok;
      }
    }

    return TTO_Err_Not_Covered;
  }


  /* Do the actual substitution for a context substitution (either format
     5 or 6).  This is only called after we've determined that the input
     matches the subrule.                                                 */

  static TT_Error  Do_ContextSubst( TTO_GSUBHeader*         gsub,
                                    UShort                  GlyphCount,
                                    UShort                  SubstCount,
                                    TTO_SubstLookupRecord*  subst,
                                    TTO_GSUB_String*        in,
                                    TTO_GSUB_String*        out,
                                    int                     nesting_level )
  {
    TT_Error  error;
    UShort    i, old_pos;


    i = 0;

    while ( i < GlyphCount )
    {
      if ( SubstCount && i == subst->SequenceIndex )
      {
        old_pos = in->pos;

        /* Do a substitution */

        error = Do_Glyph_Lookup( gsub, subst->LookupListIndex, in, out,
                                 GlyphCount, nesting_level );

        subst++;
        SubstCount--;
        i += in->pos - old_pos;

        if ( error == TTO_Err_Not_Covered )
        {
          /* XXX "can't happen" -- but don't count on it */

          if ( ADD_String( in, 1, out, 1, &in->string[in->pos] ) )
            return error;
          i++;
        }
        else if ( error )
          return error;
      }
      else
      {
        /* No substitution for this index */

        if ( ADD_String( in, 1, out, 1, &in->string[in->pos] ) )
          return error;
        i++;
      }
    }

    return TT_Err_Ok;
  }


  /* LookupType 5 */

  /* SubRule */

  static TT_Error  Load_SubRule( TTO_SubRule*  sr,
                                 PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    UShort*                 i;

    TTO_SubstLookupRecord*  slr;


    if ( ACCESS_Frame( 4L ) )
      return error;

    sr->GlyphCount = GET_UShort();
    sr->SubstCount = GET_UShort();

    FORGET_Frame();

    sr->Input = NULL;

    count = sr->GlyphCount - 1;         /* only GlyphCount - 1 elements */

    if ( ALLOC_ARRAY( sr->Input, count, UShort ) )
      return error;

    i = sr->Input;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
      i[n] = GET_UShort();

    FORGET_Frame();

    sr->SubstLookupRecord = NULL;

    count = sr->SubstCount;

    if ( ALLOC_ARRAY( sr->SubstLookupRecord, count, TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = sr->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    FREE( i );
    return error;
  }


  static void  Free_SubRule( TTO_SubRule*  sr )
  {
    FREE( sr->SubstLookupRecord );
    FREE( sr->Input );
  }


  /* SubRuleSet */

  static TT_Error  Load_SubRuleSet( TTO_SubRuleSet*  srs,
                                    PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort        n, count;
    ULong         cur_offset, new_offset, base_offset;

    TTO_SubRule*  sr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = srs->SubRuleCount = GET_UShort();

    FORGET_Frame();

    srs->SubRule = NULL;

    if ( ALLOC_ARRAY( srs->SubRule, count, TTO_SubRule ) )
      return error;

    sr = srs->SubRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_SubRule( &sr[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_SubRule( &sr[n] );

    FREE( sr );
    return error;
  }


  static void  Free_SubRuleSet( TTO_SubRuleSet*  srs )
  {
    UShort        n, count;

    TTO_SubRule*  sr;


    if ( srs->SubRule )
    {
      count = srs->SubRuleCount;
      sr    = srs->SubRule;

      for ( n = 0; n < count; n++ )
        Free_SubRule( &sr[n] );

      FREE( sr );
    }
  }


  /* ContextSubstFormat1 */

  static TT_Error  Load_ContextSubst1( TTO_ContextSubstFormat1*  csf1,
                                       PFace                     input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort           n, count;
    ULong            cur_offset, new_offset, base_offset;

    TTO_SubRuleSet*  srs;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &csf1->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = csf1->SubRuleSetCount = GET_UShort();

    FORGET_Frame();

    csf1->SubRuleSet = NULL;

    if ( ALLOC_ARRAY( csf1->SubRuleSet, count, TTO_SubRuleSet ) )
      goto Fail2;

    srs = csf1->SubRuleSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_SubRuleSet( &srs[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_SubRuleSet( &srs[n] );

    FREE( srs );

  Fail2:
    Free_Coverage( &csf1->Coverage );
    return error;
  }


  static void  Free_Context1( TTO_ContextSubstFormat1*  csf1 )
  {
    UShort           n, count;

    TTO_SubRuleSet*  srs;


    if ( csf1->SubRuleSet )
    {
      count = csf1->SubRuleSetCount;
      srs   = csf1->SubRuleSet;

      for ( n = 0; n < count; n++ )
        Free_SubRuleSet( &srs[n] );

      FREE( srs );
    }

    Free_Coverage( &csf1->Coverage );
  }


  /* SubClassRule */

  static TT_Error  Load_SubClassRule( TTO_ContextSubstFormat2*  csf2,
                                      TTO_SubClassRule*         scr,
                                      PFace                     input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;

    UShort*                 c;
    TTO_SubstLookupRecord*  slr;
    Bool*                   d;


    if ( ACCESS_Frame( 4L ) )
      return error;

    scr->GlyphCount = GET_UShort();
    scr->SubstCount = GET_UShort();

    if ( scr->GlyphCount > csf2->MaxContextLength )
      csf2->MaxContextLength = scr->GlyphCount;

    FORGET_Frame();

    scr->Class = NULL;

    count = scr->GlyphCount - 1;        /* only GlyphCount - 1 elements */

    if ( ALLOC_ARRAY( scr->Class, count, UShort ) )
      return error;

    c = scr->Class;
    d = csf2->ClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
    {
      c[n] = GET_UShort();

      /* We check whether the specific class is used at all.  If not,
         class 0 is used instead.                                     */

      if ( !d[c[n]] )
        c[n] = 0;
    }

    FORGET_Frame();

    scr->SubstLookupRecord = NULL;

    count = scr->SubstCount;

    if ( ALLOC_ARRAY( scr->SubstLookupRecord, count, TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = scr->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    FREE( c );
    return error;
  }


  static void  Free_SubClassRule( TTO_SubClassRule*  scr )
  {
    FREE( scr->SubstLookupRecord );
    FREE( scr->Class );
  }


  /* SubClassSet */

  static TT_Error  Load_SubClassSet( TTO_ContextSubstFormat2*  csf2,
                                     TTO_SubClassSet*          scs,
                                     PFace                     input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_SubClassRule*  scr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = scs->SubClassRuleCount = GET_UShort();

    FORGET_Frame();

    scs->SubClassRule = NULL;

    if ( ALLOC_ARRAY( scs->SubClassRule, count, TTO_SubClassRule ) )
      return error;

    scr = scs->SubClassRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_SubClassRule( csf2, &scr[n],
                                        input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_SubClassRule( &scr[n] );

    FREE( scr );
    return error;
  }


  static void  Free_SubClassSet( TTO_SubClassSet*  scs )
  {
    UShort             n, count;

    TTO_SubClassRule*  scr;


    if ( scs->SubClassRule )
    {
      count = scs->SubClassRuleCount;
      scr   = scs->SubClassRule;

      for ( n = 0; n < count; n++ )
        Free_SubClassRule( &scr[n] );

      FREE( scr );
    }
  }


  /* ContextSubstFormat2 */

  static TT_Error  Load_ContextSubst2( TTO_ContextSubstFormat2*  csf2,
                                       PFace                     input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count;
    ULong             cur_offset, new_offset, base_offset;

    TTO_SubClassSet*  scs;


    base_offset = FILE_Pos() - 2;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &csf2->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 4L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    /* `SubClassSetCount' is the upper limit for class values, thus we
       read it now to make an additional safety check.                 */

    count = csf2->SubClassSetCount = GET_UShort();

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_ClassDefinition( &csf2->ClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    csf2->SubClassSet      = NULL;
    csf2->MaxContextLength = 0;

    if ( ALLOC_ARRAY( csf2->SubClassSet, count, TTO_SubClassSet ) )
      goto Fail2;

    scs = csf2->SubClassSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      if ( new_offset != base_offset )      /* not a NULL offset */
      {
        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_SubClassSet( csf2, &scs[n],
                                         input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        /* we create a SubClassSet table with no entries */

        csf2->SubClassSet[n].SubClassRuleCount = 0;
        csf2->SubClassSet[n].SubClassRule      = NULL;
      }
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_SubClassSet( &scs[n] );

    FREE( scs );

  Fail2:
    Free_ClassDefinition( &csf2->ClassDef );

  Fail3:
    Free_Coverage( &csf2->Coverage );
    return error;
  }


  static void  Free_Context2( TTO_ContextSubstFormat2*  csf2 )
  {
    UShort            n, count;

    TTO_SubClassSet*  scs;


    if ( csf2->SubClassSet )
    {
      count = csf2->SubClassSetCount;
      scs   = csf2->SubClassSet;

      for ( n = 0; n < count; n++ )
        Free_SubClassSet( &scs[n] );

      FREE( scs );
    }

    Free_ClassDefinition( &csf2->ClassDef );
    Free_Coverage( &csf2->Coverage );
  }


  /* ContextSubstFormat3 */

  static TT_Error  Load_ContextSubst3( TTO_ContextSubstFormat3*  csf3,
                                       PFace                     input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    ULong                   cur_offset, new_offset, base_offset;

    TTO_Coverage*           c;
    TTO_SubstLookupRecord*  slr;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 4L ) )
      return error;

    csf3->GlyphCount = GET_UShort();
    csf3->SubstCount = GET_UShort();

    FORGET_Frame();

    csf3->Coverage = NULL;

    count = csf3->GlyphCount;

    if ( ALLOC_ARRAY( csf3->Coverage, count, TTO_Coverage ) )
      return error;

    c = csf3->Coverage;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &c[n], input ) ) != TT_Err_Ok )
        goto Fail2;
      (void)FILE_Seek( cur_offset );
    }

    csf3->SubstLookupRecord = NULL;

    count = csf3->SubstCount;

    if ( ALLOC_ARRAY( csf3->SubstLookupRecord, count,
                      TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = csf3->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    for ( n = 0; n < count; n++ )
      Free_Coverage( &c[n] );

    FREE( c );
    return error;
  }


  static void  Free_Context3( TTO_ContextSubstFormat3*  csf3 )
  {
    UShort         n, count;

    TTO_Coverage*  c;


    FREE( csf3->SubstLookupRecord );

    if ( csf3->Coverage )
    {
      count = csf3->GlyphCount;
      c     = csf3->Coverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }
  }


  /* ContextSubst */

  TT_Error  Load_ContextSubst( TTO_ContextSubst*  cs,
                               PFace              input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ACCESS_Frame( 2L ) )
      return error;

    cs->SubstFormat = GET_UShort();

    FORGET_Frame();

    switch ( cs->SubstFormat )
    {
    case 1:
      return Load_ContextSubst1( &cs->csf.csf1, input );

    case 2:
      return Load_ContextSubst2( &cs->csf.csf2, input );

    case 3:
      return Load_ContextSubst3( &cs->csf.csf3, input );

    default:
      return TTO_Err_Invalid_GSUB_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  void  Free_ContextSubst( TTO_ContextSubst*  cs )
  {
    switch ( cs->SubstFormat )
    {
    case 1:
      Free_Context1( &cs->csf.csf1 );
      break;

    case 2:
      Free_Context2( &cs->csf.csf2 );
      break;

    case 3:
      Free_Context3( &cs->csf.csf3 );
      break;
    }
  }


  static TT_Error  Lookup_ContextSubst1(
                     TTO_GSUBHeader*           gsub,
                     TTO_ContextSubstFormat1*  csf1,
                     TTO_GSUB_String*          in,
                     TTO_GSUB_String*          out,
                     UShort                    flags,
                     UShort                    context_length,
                     int                       nesting_level )
  {
    UShort           index, property;
    UShort           i, j, k, numsr;
    TT_Error         error;
    UShort*          s_in;

    TTO_SubRule*     sr;
    TTO_GDEFHeader*  gdef;


    gdef = gsub->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &csf1->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    sr    = csf1->SubRuleSet[index].SubRule;
    numsr = csf1->SubRuleSet[index].SubRuleCount;

    for ( k = 0; k < numsr; k++ )
    {
      if ( context_length != 0xFFFF && context_length < sr[k].GlyphCount )
        continue;

      if ( in->pos + sr[k].GlyphCount > in->length )
        continue;                           /* context is too long */

      s_in = &in->string[in->pos];

      for ( i = 1, j = 1; i < sr[k].GlyphCount; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( in->pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != sr[k].Input[i - 1] )
          break;
      }

      if ( i == sr[k].GlyphCount )
        return Do_ContextSubst( gsub, sr[k].GlyphCount,
                                sr[k].SubstCount, sr[k].SubstLookupRecord,
                                in, out,
                                nesting_level );
    }

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Lookup_ContextSubst2(
                     TTO_GSUBHeader*           gsub,
                     TTO_ContextSubstFormat2*  csf2,
                     TTO_GSUB_String*          in,
                     TTO_GSUB_String*          out,
                     UShort                    flags,
                     UShort                    context_length,
                     int                       nesting_level )
  {
    UShort             index, property;
    TT_Error           error;
    UShort             i, j, k, known_classes;

    UShort*            classes;
    UShort*            s_in;
    UShort*            cl;

    TTO_SubClassSet*   scs;
    TTO_SubClassRule*  sr;
    TTO_GDEFHeader*    gdef;


    gdef = gsub->gdef;

    if ( ALLOC_ARRAY( classes, csf2->MaxContextLength, UShort ) )
      return error;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    /* Note: The coverage table in format 2 doesn't give an index into
             anything.  It just lets us know whether or not we need to
             do any lookup at all.                                     */

    error = Coverage_Index( &csf2->Coverage, in->string[in->pos], &index );
    if ( error )
      goto End;

    error = Get_Class( &csf2->ClassDef, in->string[in->pos],
                       &classes[0], NULL );
    if ( error )
      goto End;
    known_classes = 0;

    scs = &csf2->SubClassSet[classes[0]];
    if ( !scs )
    {
      error = TTO_Err_Invalid_GSUB_SubTable;
      goto End;
    }

    for ( k = 0; k < scs->SubClassRuleCount; k++ )
    {
      sr  = &scs->SubClassRule[k];

      if ( context_length != 0xFFFF && context_length < sr->GlyphCount )
        continue;

      if ( in->pos + sr->GlyphCount > in->length )
        continue;                           /* context is too long */

      s_in = &in->string[in->pos];
      cl   = sr->Class;

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < sr->GlyphCount; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( in->pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i > known_classes )
        {
          /* Keeps us from having to do this for each rule */

          error = Get_Class( &csf2->ClassDef, s_in[j], &classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            return error;
          known_classes = i;
        }

        if ( cl[i - 1] != classes[i] )
          break;
      }

      if ( i == sr->GlyphCount )
      {
        error = Do_ContextSubst( gsub, sr->GlyphCount,
                                 sr->SubstCount, sr->SubstLookupRecord,
                                 in, out,
                                 nesting_level );
        goto End;
      }
    }

    error = TTO_Err_Not_Covered;

  End:
    FREE( classes );
    return error;
  }


  static TT_Error  Lookup_ContextSubst3(
                     TTO_GSUBHeader*           gsub,
                     TTO_ContextSubstFormat3*  csf3,
                     TTO_GSUB_String*          in,
                     TTO_GSUB_String*          out,
                     UShort                    flags,
                     UShort                    context_length,
                     int                       nesting_level )
  {
    TT_Error         error;
    UShort           index, i, j, property;
    UShort*          s_in;

    TTO_Coverage*    c;
    TTO_GDEFHeader*  gdef;


    gdef = gsub->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    if ( context_length != 0xFFFF && context_length < csf3->GlyphCount )
      return TTO_Err_Not_Covered;

    if ( in->pos + csf3->GlyphCount > in->length )
      return TTO_Err_Not_Covered;         /* context is too long */

    s_in = &in->string[in->pos];
    c    = csf3->Coverage;

    for ( i = 1, j = 1; i < csf3->GlyphCount; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( in->pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &c[i], s_in[j], &index );
      if ( error )
        return error;
    }

    return Do_ContextSubst( gsub, csf3->GlyphCount,
                            csf3->SubstCount, csf3->SubstLookupRecord,
                            in, out,
                            nesting_level );
  }


  static TT_Error  Lookup_ContextSubst( TTO_GSUBHeader*    gsub,
                                        TTO_ContextSubst*  cs,
                                        TTO_GSUB_String*   in,
                                        TTO_GSUB_String*   out,
                                        UShort             flags,
                                        UShort             context_length,
                                        int                nesting_level )
  {
    switch ( cs->SubstFormat )
    {
    case 1:
      return Lookup_ContextSubst1( gsub, &cs->csf.csf1, in, out,
                                   flags, context_length, nesting_level );

    case 2:
      return Lookup_ContextSubst2( gsub, &cs->csf.csf2, in, out,
                                   flags, context_length, nesting_level );

    case 3:
      return Lookup_ContextSubst3( gsub, &cs->csf.csf3, in, out,
                                   flags, context_length, nesting_level );

    default:
      return TTO_Err_Invalid_GSUB_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  /* LookupType 6 */

  /* ChainSubRule */

  static TT_Error  Load_ChainSubRule( TTO_ChainSubRule*  csr,
                                      PFace              input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    UShort*                 b;
    UShort*                 i;
    UShort*                 l;

    TTO_SubstLookupRecord*  slr;


    if ( ACCESS_Frame( 2L ) )
      return error;

    csr->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    csr->Backtrack = NULL;

    count = csr->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( csr->Backtrack, count, UShort ) )
      return error;

    b = csr->Backtrack;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail4;

    for ( n = 0; n < count; n++ )
      b[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    csr->InputGlyphCount = GET_UShort();

    FORGET_Frame();

    csr->Input = NULL;

    count = csr->InputGlyphCount - 1;  /* only InputGlyphCount - 1 elements */

    if ( ALLOC_ARRAY( csr->Input, count, UShort ) )
      goto Fail4;

    i = csr->Input;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail3;

    for ( n = 0; n < count; n++ )
      i[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    csr->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    csr->Lookahead = NULL;

    count = csr->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( csr->Lookahead, count, UShort ) )
      goto Fail3;

    l = csr->Lookahead;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
      l[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    csr->SubstCount = GET_UShort();

    FORGET_Frame();

    csr->SubstLookupRecord = NULL;

    count = csr->SubstCount;

    if ( ALLOC_ARRAY( csr->SubstLookupRecord, count, TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = csr->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    FREE( l );

  Fail3:
    FREE( i );

  Fail4:
    FREE( b );
    return error;
  }


  static void  Free_ChainSubRule( TTO_ChainSubRule*  csr )
  {
    FREE( csr->SubstLookupRecord );
    FREE( csr->Lookahead );
    FREE( csr->Input );
    FREE( csr->Backtrack );
  }


  /* ChainSubRuleSet */

  static TT_Error  Load_ChainSubRuleSet( TTO_ChainSubRuleSet*  csrs,
                                         PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_ChainSubRule*  csr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = csrs->ChainSubRuleCount = GET_UShort();

    FORGET_Frame();

    csrs->ChainSubRule = NULL;

    if ( ALLOC_ARRAY( csrs->ChainSubRule, count, TTO_ChainSubRule ) )
      return error;

    csr = csrs->ChainSubRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainSubRule( &csr[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_ChainSubRule( &csr[n] );

    FREE( csr );
    return error;
  }


  static void  Free_ChainSubRuleSet( TTO_ChainSubRuleSet*  csrs )
  {
    UShort             n, count;

    TTO_ChainSubRule*  csr;


    if ( csrs->ChainSubRule )
    {
      count = csrs->ChainSubRuleCount;
      csr   = csrs->ChainSubRule;

      for ( n = 0; n < count; n++ )
        Free_ChainSubRule( &csr[n] );

      FREE( csr );
    }
  }


  /* ChainContextSubstFormat1 */

  static TT_Error  Load_ChainContextSubst1(
                     TTO_ChainContextSubstFormat1*  ccsf1,
                     PFace                          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_ChainSubRuleSet*  csrs;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ccsf1->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = ccsf1->ChainSubRuleSetCount = GET_UShort();

    FORGET_Frame();

    ccsf1->ChainSubRuleSet = NULL;

    if ( ALLOC_ARRAY( ccsf1->ChainSubRuleSet, count, TTO_ChainSubRuleSet ) )
      goto Fail2;

    csrs = ccsf1->ChainSubRuleSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainSubRuleSet( &csrs[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_ChainSubRuleSet( &csrs[n] );

    FREE( csrs );

  Fail2:
    Free_Coverage( &ccsf1->Coverage );
    return error;
  }


  static void  Free_ChainContext1( TTO_ChainContextSubstFormat1*  ccsf1 )
  {
    UShort                n, count;

    TTO_ChainSubRuleSet*  csrs;


    if ( ccsf1->ChainSubRuleSet )
    {
      count = ccsf1->ChainSubRuleSetCount;
      csrs  = ccsf1->ChainSubRuleSet;

      for ( n = 0; n < count; n++ )
        Free_ChainSubRuleSet( &csrs[n] );

      FREE( csrs );
    }

    Free_Coverage( &ccsf1->Coverage );
  }


  /* ChainSubClassRule */

  static TT_Error  Load_ChainSubClassRule(
                     TTO_ChainContextSubstFormat2*  ccsf2,
                     TTO_ChainSubClassRule*         cscr,
                     PFace                          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;

    UShort*                 b;
    UShort*                 i;
    UShort*                 l;
    TTO_SubstLookupRecord*  slr;
    Bool*                   d;


    if ( ACCESS_Frame( 2L ) )
      return error;

    cscr->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    if ( cscr->BacktrackGlyphCount > ccsf2->MaxBacktrackLength )
      ccsf2->MaxBacktrackLength = cscr->BacktrackGlyphCount;

    cscr->Backtrack = NULL;

    count = cscr->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( cscr->Backtrack, count, UShort ) )
      return error;

    b = cscr->Backtrack;
    d = ccsf2->BacktrackClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail4;

    for ( n = 0; n < count; n++ )
    {
      b[n] = GET_UShort();

      /* We check whether the specific class is used at all.  If not,
         class 0 is used instead.                                     */

      if ( !d[b[n]] )
        b[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    cscr->InputGlyphCount = GET_UShort();

    FORGET_Frame();

    if ( cscr->InputGlyphCount > ccsf2->MaxInputLength )
      ccsf2->MaxInputLength = cscr->InputGlyphCount;

    cscr->Input = NULL;

    count = cscr->InputGlyphCount - 1; /* only InputGlyphCount - 1 elements */

    if ( ALLOC_ARRAY( cscr->Input, count, UShort ) )
      goto Fail4;

    i = cscr->Input;
    d = ccsf2->InputClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail3;

    for ( n = 0; n < count; n++ )
    {
      i[n] = GET_UShort();

      if ( !d[i[n]] )
        i[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    cscr->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    if ( cscr->LookaheadGlyphCount > ccsf2->MaxLookaheadLength )
      ccsf2->MaxLookaheadLength = cscr->LookaheadGlyphCount;

    cscr->Lookahead = NULL;

    count = cscr->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( cscr->Lookahead, count, UShort ) )
      goto Fail3;

    l = cscr->Lookahead;
    d = ccsf2->LookaheadClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
    {
      l[n] = GET_UShort();

      if ( !d[l[n]] )
        l[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    cscr->SubstCount = GET_UShort();

    FORGET_Frame();

    cscr->SubstLookupRecord = NULL;

    count = cscr->SubstCount;

    if ( ALLOC_ARRAY( cscr->SubstLookupRecord, count,
                      TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = cscr->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    FREE( l );

  Fail3:
    FREE( i );

  Fail4:
    FREE( b );
    return error;
  }


  static void  Free_ChainSubClassRule( TTO_ChainSubClassRule*  cscr )
  {
    FREE( cscr->SubstLookupRecord );
    FREE( cscr->Lookahead );
    FREE( cscr->Input );
    FREE( cscr->Backtrack );
  }


  /* SubClassSet */

  static TT_Error  Load_ChainSubClassSet(
                     TTO_ChainContextSubstFormat2*  ccsf2,
                     TTO_ChainSubClassSet*          cscs,
                     PFace                          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    ULong                   cur_offset, new_offset, base_offset;

    TTO_ChainSubClassRule*  cscr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cscs->ChainSubClassRuleCount = GET_UShort();

    FORGET_Frame();

    cscs->ChainSubClassRule = NULL;

    if ( ALLOC_ARRAY( cscs->ChainSubClassRule, count,
                      TTO_ChainSubClassRule ) )
      return error;

    cscr = cscs->ChainSubClassRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainSubClassRule( ccsf2, &cscr[n],
                                             input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_ChainSubClassRule( &cscr[n] );

    FREE( cscr );
    return error;
  }


  static void  Free_ChainSubClassSet( TTO_ChainSubClassSet*  cscs )
  {
    UShort                  n, count;

    TTO_ChainSubClassRule*  cscr;


    if ( cscs->ChainSubClassRule )
    {
      count = cscs->ChainSubClassRuleCount;
      cscr  = cscs->ChainSubClassRule;

      for ( n = 0; n < count; n++ )
        Free_ChainSubClassRule( &cscr[n] );

      FREE( cscr );
    }
  }


  /* ChainContextSubstFormat2 */

  static TT_Error  Load_ChainContextSubst2(
                     TTO_ChainContextSubstFormat2*  ccsf2,
                     PFace                          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                 n, count;
    ULong                  cur_offset, new_offset, base_offset;
    ULong                  backtrack_offset, input_offset, lookahead_offset;

    TTO_ChainSubClassSet*  cscs;


    base_offset = FILE_Pos() - 2;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ccsf2->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 8L ) )
      goto Fail5;

    backtrack_offset = GET_UShort() + base_offset;
    input_offset     = GET_UShort() + base_offset;
    lookahead_offset = GET_UShort() + base_offset;

    /* `ChainSubClassSetCount' is the upper limit for input class values,
       thus we read it now to make an additional safety check.            */

    count = ccsf2->ChainSubClassSetCount = GET_UShort();

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( backtrack_offset ) ||
         ( error = Load_ClassDefinition( &ccsf2->BacktrackClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail5;
    if ( FILE_Seek( input_offset ) ||
         ( error = Load_ClassDefinition( &ccsf2->InputClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail4;
    if ( FILE_Seek( lookahead_offset ) ||
         ( error = Load_ClassDefinition( &ccsf2->LookaheadClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    ccsf2->ChainSubClassSet   = NULL;
    ccsf2->MaxBacktrackLength = 0;
    ccsf2->MaxInputLength     = 0;
    ccsf2->MaxLookaheadLength = 0;

    if ( ALLOC_ARRAY( ccsf2->ChainSubClassSet, count, TTO_ChainSubClassSet ) )
      goto Fail2;

    cscs = ccsf2->ChainSubClassSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      if ( new_offset != base_offset )      /* not a NULL offset */
      {
        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_ChainSubClassSet( ccsf2, &cscs[n],
                                              input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        /* we create a ChainSubClassSet table with no entries */

        ccsf2->ChainSubClassSet[n].ChainSubClassRuleCount = 0;
        ccsf2->ChainSubClassSet[n].ChainSubClassRule      = NULL;
      }
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_ChainSubClassSet( &cscs[n] );

    FREE( cscs );

  Fail2:
    Free_ClassDefinition( &ccsf2->LookaheadClassDef );

  Fail3:
    Free_ClassDefinition( &ccsf2->InputClassDef );

  Fail4:
    Free_ClassDefinition( &ccsf2->BacktrackClassDef );

  Fail5:
    Free_Coverage( &ccsf2->Coverage );
    return error;
  }


  static void  Free_ChainContext2( TTO_ChainContextSubstFormat2*  ccsf2 )
  {
    UShort                 n, count;

    TTO_ChainSubClassSet*  cscs;


    if ( ccsf2->ChainSubClassSet )
    {
      count = ccsf2->ChainSubClassSetCount;
      cscs  = ccsf2->ChainSubClassSet;

      for ( n = 0; n < count; n++ )
        Free_ChainSubClassSet( &cscs[n] );

      FREE( cscs );
    }

    Free_ClassDefinition( &ccsf2->LookaheadClassDef );
    Free_ClassDefinition( &ccsf2->InputClassDef );
    Free_ClassDefinition( &ccsf2->BacktrackClassDef );

    Free_Coverage( &ccsf2->Coverage );
  }


  /* ChainContextSubstFormat3 */

  static TT_Error  Load_ChainContextSubst3(
                     TTO_ChainContextSubstFormat3*  ccsf3,
                     PFace                          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    UShort                  backtrack_count, input_count, lookahead_count;
    ULong                   cur_offset, new_offset, base_offset;

    TTO_Coverage*           b;
    TTO_Coverage*           i;
    TTO_Coverage*           l;
    TTO_SubstLookupRecord*  slr;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    ccsf3->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    ccsf3->BacktrackCoverage = NULL;

    backtrack_count = ccsf3->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( ccsf3->BacktrackCoverage, backtrack_count,
                      TTO_Coverage ) )
      return error;

    b = ccsf3->BacktrackCoverage;

    for ( n = 0; n < backtrack_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail4;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &b[n], input ) ) != TT_Err_Ok )
        goto Fail4;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    ccsf3->InputGlyphCount = GET_UShort();

    FORGET_Frame();

    ccsf3->InputCoverage = NULL;

    input_count = ccsf3->InputGlyphCount;

    if ( ALLOC_ARRAY( ccsf3->InputCoverage, input_count, TTO_Coverage ) )
      goto Fail4;

    i = ccsf3->InputCoverage;

    for ( n = 0; n < input_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail3;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &i[n], input ) ) != TT_Err_Ok )
        goto Fail3;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    ccsf3->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    ccsf3->LookaheadCoverage = NULL;

    lookahead_count = ccsf3->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( ccsf3->LookaheadCoverage, lookahead_count,
                      TTO_Coverage ) )
      goto Fail3;

    l = ccsf3->LookaheadCoverage;

    for ( n = 0; n < lookahead_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &l[n], input ) ) != TT_Err_Ok )
        goto Fail2;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    ccsf3->SubstCount = GET_UShort();

    FORGET_Frame();

    ccsf3->SubstLookupRecord = NULL;

    count = ccsf3->SubstCount;

    if ( ALLOC_ARRAY( ccsf3->SubstLookupRecord, count,
                      TTO_SubstLookupRecord ) )
      goto Fail2;

    slr = ccsf3->SubstLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      slr[n].SequenceIndex   = GET_UShort();
      slr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( slr );

  Fail2:
    for ( n = 0; n < lookahead_count; n++ )
      Free_Coverage( &l[n] );

    FREE( l );

  Fail3:
    for ( n = 0; n < input_count; n++ )
      Free_Coverage( &i[n] );

    FREE( i );

  Fail4:
    for ( n = 0; n < backtrack_count; n++ )
      Free_Coverage( &b[n] );

    FREE( b );
    return error;
  }


  static void  Free_ChainContext3( TTO_ChainContextSubstFormat3*  ccsf3 )
  {
    UShort         n, count;

    TTO_Coverage*  c;


    FREE( ccsf3->SubstLookupRecord );

    if ( ccsf3->LookaheadCoverage )
    {
      count = ccsf3->LookaheadGlyphCount;
      c     = ccsf3->LookaheadCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }

    if ( ccsf3->InputCoverage )
    {
      count = ccsf3->InputGlyphCount;
      c     = ccsf3->InputCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }

    if ( ccsf3->BacktrackCoverage )
    {
      count = ccsf3->BacktrackGlyphCount;
      c     = ccsf3->BacktrackCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }
  }


  /* ChainContextSubst */

  TT_Error  Load_ChainContextSubst( TTO_ChainContextSubst*  ccs,
                                    PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ACCESS_Frame( 2L ) )
      return error;

    ccs->SubstFormat = GET_UShort();

    FORGET_Frame();

    switch ( ccs->SubstFormat )
    {
    case 1:
      return Load_ChainContextSubst1( &ccs->ccsf.ccsf1, input );

    case 2:
      return Load_ChainContextSubst2( &ccs->ccsf.ccsf2, input );

    case 3:
      return Load_ChainContextSubst3( &ccs->ccsf.ccsf3, input );

    default:
      return TTO_Err_Invalid_GSUB_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  void  Free_ChainContextSubst( TTO_ChainContextSubst*  ccs )
  {
    switch ( ccs->SubstFormat )
    {
    case 1:
      Free_ChainContext1( &ccs->ccsf.ccsf1 );
      break;

    case 2:
      Free_ChainContext2( &ccs->ccsf.ccsf2 );
      break;

    case 3:
      Free_ChainContext3( &ccs->ccsf.ccsf3 );
      break;
    }
  }


  static TT_Error  Lookup_ChainContextSubst1(
                     TTO_GSUBHeader*                gsub,
                     TTO_ChainContextSubstFormat1*  ccsf1,
                     TTO_GSUB_String*               in,
                     TTO_GSUB_String*               out,
                     UShort                         flags,
                     UShort                         context_length,
                     int                            nesting_level )
  {
    UShort             index, property;
    UShort             i, j, k, num_csr, curr_pos;
    UShort             bgc, igc, lgc;
    TT_Error           error;
    UShort*            s_in;

    TTO_ChainSubRule*  csr;
    TTO_ChainSubRule   curr_csr;
    TTO_GDEFHeader*    gdef;


    gdef = gsub->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &ccsf1->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    csr     = ccsf1->ChainSubRuleSet[index].ChainSubRule;
    num_csr = ccsf1->ChainSubRuleSet[index].ChainSubRuleCount;

    for ( k = 0; k < num_csr; k++ )
    {
      curr_csr = csr[k];
      bgc      = curr_csr.BacktrackGlyphCount;
      igc      = curr_csr.InputGlyphCount;
      lgc      = curr_csr.LookaheadGlyphCount;

      if ( context_length != 0xFFFF && context_length < igc )
        continue;

      /* check whether context is too long; it is a first guess only */

      if ( bgc > in->pos || in->pos + igc + lgc > in->length )
        continue;

      if ( bgc )
      {
        /* Since we don't know in advance the number of glyphs to inspect,
           we search backwards for matches in the backtrack glyph array    */

        curr_pos = 0;
        s_in     = &in->string[curr_pos];

        for ( i = bgc, j = in->pos - 1; i > 0; i--, j-- )
        {
          while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
          {
            if ( error && error != TTO_Err_Not_Covered )
              return error;

            if ( j > curr_pos )
              j--;
            else
              break;
          }

          if ( s_in[j] != curr_csr.Backtrack[i - 1] )
            break;
        }

        if ( i != 0 )
          continue;
      }

      curr_pos = in->pos;
      s_in     = &in->string[curr_pos];

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < igc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != curr_csr.Input[i - 1] )
          break;
      }

      if ( i != igc )
        continue;

      /* we are starting to check for lookahead glyphs right after the
         last context glyph                                            */

      curr_pos = j;
      s_in     = &in->string[curr_pos];

      for ( i = 0, j = 0; i < lgc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != curr_csr.Lookahead[i] )
          break;
      }

      if ( i == lgc )
        return Do_ContextSubst( gsub, igc,
                                curr_csr.SubstCount,
                                curr_csr.SubstLookupRecord,
                                in, out,
                                nesting_level );
    }

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Lookup_ChainContextSubst2(
                     TTO_GSUBHeader*                gsub,
                     TTO_ChainContextSubstFormat2*  ccsf2,
                     TTO_GSUB_String*               in,
                     TTO_GSUB_String*               out,
                     UShort                         flags,
                     UShort                         context_length,
                     int                            nesting_level )
  {
    UShort                 index, property;
    TT_Error               error;
    UShort                 i, j, k, curr_pos;
    UShort                 bgc, igc, lgc;
    UShort                 known_backtrack_classes,
                           known_input_classes,
                           known_lookahead_classes;

    UShort*                backtrack_classes;
    UShort*                input_classes;
    UShort*                lookahead_classes;

    UShort*                s_in;

    UShort*                bc;
    UShort*                ic;
    UShort*                lc;

    TTO_ChainSubClassSet*  cscs;
    TTO_ChainSubClassRule  ccsr;
    TTO_GDEFHeader*        gdef;


    gdef = gsub->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    /* Note: The coverage table in format 2 doesn't give an index into
             anything.  It just lets us know whether or not we need to
             do any lookup at all.                                     */

    error = Coverage_Index( &ccsf2->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    if ( ALLOC_ARRAY( backtrack_classes, ccsf2->MaxBacktrackLength, UShort ) )
      return error;
    known_backtrack_classes = 0;

    if ( ALLOC_ARRAY( input_classes, ccsf2->MaxInputLength, UShort ) )
      goto End3;
    known_input_classes = 1;

    if ( ALLOC_ARRAY( lookahead_classes, ccsf2->MaxLookaheadLength, UShort ) )
      goto End2;
    known_lookahead_classes = 0;

    error = Get_Class( &ccsf2->InputClassDef, in->string[in->pos],
                       &input_classes[0], NULL );
    if ( error )
      goto End1;

    cscs = &ccsf2->ChainSubClassSet[input_classes[0]];
    if ( !cscs )
    {
      error = TTO_Err_Invalid_GSUB_SubTable;
      goto End1;
    }

    for ( k = 0; k < cscs->ChainSubClassRuleCount; k++ )
    {
      ccsr = cscs->ChainSubClassRule[k];
      bgc  = ccsr.BacktrackGlyphCount;
      igc  = ccsr.InputGlyphCount;
      lgc  = ccsr.LookaheadGlyphCount;

      if ( context_length != 0xFFFF && context_length < igc )
        continue;

      /* check whether context is too long; it is a first guess only */

      if ( bgc > in->pos || in->pos + igc + lgc > in->length )
        continue;

      if ( bgc )
      {
        /* Since we don't know in advance the number of glyphs to inspect,
           we search backwards for matches in the backtrack glyph array.
           Note that `known_backtrack_classes' starts at index 0.         */

        curr_pos = 0;
        s_in     = &in->string[curr_pos];
        bc       = ccsr.Backtrack;

        for ( i = 0, j = in->pos - 1; i < bgc; i++, j-- )
        {
          while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
          {
            if ( error && error != TTO_Err_Not_Covered )
              return error;

            if ( j > curr_pos )
              j--;
            else
              break;
          }

          if ( i >= known_backtrack_classes )
          {
            /* Keeps us from having to do this for each rule */

            error = Get_Class( &ccsf2->BacktrackClassDef, s_in[j],
                               &backtrack_classes[i], NULL );
            if ( error && error != TTO_Err_Not_Covered )
              goto End1;
            known_backtrack_classes = i;
          }

          if ( bc[bgc - 1 - i] != backtrack_classes[i] )
            break;
        }

        if ( i != bgc )
          continue;
      }

      curr_pos = in->pos;
      s_in     = &in->string[curr_pos];
      ic       = ccsr.Input;

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < igc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i >= known_input_classes )
        {
          error = Get_Class( &ccsf2->InputClassDef, s_in[j],
                             &input_classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;
          known_input_classes = i;
        }

        if ( ic[i - 1] != input_classes[i] )
          break;
      }

      if ( i != igc )
        continue;

      /* we are starting to check for lookahead glyphs right after the
         last context glyph                                            */

      curr_pos = j;
      s_in     = &in->string[curr_pos];
      lc       = ccsr.Lookahead;

      for ( i = 0, j = 0; i < lgc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i >= known_lookahead_classes )
        {
          error = Get_Class( &ccsf2->LookaheadClassDef, s_in[j],
                             &lookahead_classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;
          known_lookahead_classes = i;
        }

        if ( lc[i] != lookahead_classes[i] )
          break;
      }

      if ( i == lgc )
      {
        error = Do_ContextSubst( gsub, igc,
                                 ccsr.SubstCount,
                                 ccsr.SubstLookupRecord,
                                 in, out,
                                 nesting_level );
        goto End1;
      }
    }

    error = TTO_Err_Not_Covered;

  End1:
    FREE( lookahead_classes );

  End2:
    FREE( input_classes );

  End3:
    FREE( backtrack_classes );
    return error;
  }


  static TT_Error  Lookup_ChainContextSubst3(
                     TTO_GSUBHeader*                gsub,
                     TTO_ChainContextSubstFormat3*  ccsf3,
                     TTO_GSUB_String*               in,
                     TTO_GSUB_String*               out,
                     UShort                         flags,
                     UShort                         context_length,
                     int                            nesting_level )
  {
    UShort           index, i, j, curr_pos, property;
    UShort           bgc, igc, lgc;
    TT_Error         error;
    UShort*          s_in;

    TTO_Coverage*    bc;
    TTO_Coverage*    ic;
    TTO_Coverage*    lc;
    TTO_GDEFHeader*  gdef;


    gdef = gsub->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    bgc = ccsf3->BacktrackGlyphCount;
    igc = ccsf3->InputGlyphCount;
    lgc = ccsf3->LookaheadGlyphCount;

    if ( context_length != 0xFFFF && context_length < igc )
      return TTO_Err_Not_Covered;

    /* check whether context is too long; it is a first guess only */

    if ( bgc > in->pos || in->pos + igc + lgc > in->length )
      return TTO_Err_Not_Covered;

    if ( bgc )
    {
      /* Since we don't know in advance the number of glyphs to inspect,
         we search backwards for matches in the backtrack glyph array    */

      curr_pos = 0;
      s_in     = &in->string[curr_pos];
      bc       = ccsf3->BacktrackCoverage;

      for ( i = bgc, j = in->pos - 1; i > 0; i--, j-- )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( j > curr_pos )
            j--;
          else
            return TTO_Err_Not_Covered;
        }

        error = Coverage_Index( &bc[i - 1], s_in[j], &index );
        if ( error )
          return error;
      }
    }

    curr_pos = in->pos;
    s_in     = &in->string[curr_pos];
    ic       = ccsf3->InputCoverage;

    /* Start at 1 because [0] is implied */

    for ( i = 1, j = 1; i < igc; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( curr_pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &ic[i], s_in[j], &index );
      if ( error )
        return error;
    }

    /* we are starting for lookahead glyphs right after the last context
       glyph                                                             */

    curr_pos = j;
    s_in     = &in->string[curr_pos];
    lc       = ccsf3->LookaheadCoverage;

    for ( i = 0, j = 0; i < lgc; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( curr_pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &lc[i], s_in[j], &index );
      if ( error )
        return error;
    }

    return Do_ContextSubst( gsub, igc,
                            ccsf3->SubstCount,
                            ccsf3->SubstLookupRecord,
                            in, out,
                            nesting_level );
  }


  static TT_Error  Lookup_ChainContextSubst(
                     TTO_GSUBHeader*         gsub,
                     TTO_ChainContextSubst*  ccs,
                     TTO_GSUB_String*        in,
                     TTO_GSUB_String*        out,
                     UShort                  flags,
                     UShort                  context_length,
                     int                     nesting_level )
  {
    switch ( ccs->SubstFormat )
    {
    case 1:
      return Lookup_ChainContextSubst1( gsub, &ccs->ccsf.ccsf1, in, out,
                                        flags, context_length,
                                        nesting_level );

    case 2:
      return Lookup_ChainContextSubst2( gsub, &ccs->ccsf.ccsf2, in, out,
                                        flags, context_length,
                                        nesting_level );

    case 3:
      return Lookup_ChainContextSubst3( gsub, &ccs->ccsf.ccsf3, in, out,
                                        flags, context_length,
                                        nesting_level );

    default:
      return TTO_Err_Invalid_GSUB_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }



  /***********
   * GSUB API
   ***********/


  EXPORT_FUNC
  TT_Error  TT_GSUB_Select_Script( TTO_GSUBHeader*  gsub,
                                   TT_ULong         script_tag,
                                   TT_UShort*       script_index )
  {
    UShort             n;

    TTO_ScriptList*    sl;
    TTO_ScriptRecord*  sr;


    if ( !gsub || !script_index )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    for ( n = 0; n < sl->ScriptCount; n++ )
      if ( script_tag == sr[n].ScriptTag )
      {
        *script_index = n;

        return TT_Err_Ok;
      }

    return TTO_Err_Not_Covered;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Select_Language( TTO_GSUBHeader*  gsub,
                                     TT_ULong         language_tag,
                                     TT_UShort        script_index,
                                     TT_UShort*       language_index,
                                     TT_UShort*       req_feature_index )
  {
    UShort              n;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;


    if ( !gsub || !language_index || !req_feature_index )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    for ( n = 0; n < s->LangSysCount; n++ )
      if ( language_tag == lsr[n].LangSysTag )
      {
        *language_index = n;
        *req_feature_index = lsr[n].LangSys.ReqFeatureIndex;

        return TT_Err_Ok;
      }

    return TTO_Err_Not_Covered;
  }


  /* selecting 0xFFFF for language_index asks for the values of the
     default language (DefaultLangSys)                              */

  EXPORT_FUNC
  TT_Error  TT_GSUB_Select_Feature( TTO_GSUBHeader*  gsub,
                                    TT_ULong         feature_tag,
                                    TT_UShort        script_index,
                                    TT_UShort        language_index,
                                    TT_UShort*       feature_index )
  {
    UShort              n;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;
    TTO_LangSys*        ls;
    UShort*             fi;

    TTO_FeatureList*    fl;
    TTO_FeatureRecord*  fr;


    if ( !gsub || !feature_index )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    fl = &gsub->FeatureList;
    fr = fl->FeatureRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( language_index == 0xFFFF )
      ls = &s->DefaultLangSys;
    else
    {
      if ( language_index >= s->LangSysCount )
        return TT_Err_Invalid_Argument;

      ls = &lsr[language_index].LangSys;
    }

    fi = ls->FeatureIndex;

    for ( n = 0; n < ls->FeatureCount; n++ )
    {
      if ( fi[n] >= fl->FeatureCount )
        return TTO_Err_Invalid_GSUB_SubTable_Format;

      if ( feature_tag == fr[fi[n]].FeatureTag )
      {
        *feature_index = fi[n];

        return TT_Err_Ok;
      }
    }

    return TTO_Err_Not_Covered;
  }


  /* The next three functions return a null-terminated list */

  EXPORT_FUNC
  TT_Error  TT_GSUB_Query_Scripts( TTO_GSUBHeader*  gsub,
                                   TT_ULong**       script_tag_list )
  {
    UShort             n;
    TT_Error           error;
    ULong*             stl;

    TTO_ScriptList*    sl;
    TTO_ScriptRecord*  sr;


    if ( !gsub || !script_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    if ( ALLOC_ARRAY( stl, sl->ScriptCount + 1, ULong ) )
      return error;

    for ( n = 0; n < sl->ScriptCount; n++ )
      stl[n] = sr[n].ScriptTag;
    stl[n] = 0;

    *script_tag_list = stl;

    return TT_Err_Ok;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Query_Languages( TTO_GSUBHeader*  gsub,
                                     TT_UShort        script_index,
                                     TT_ULong**       language_tag_list )
  {
    UShort              n;
    TT_Error            error;
    ULong*              ltl;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;


    if ( !gsub || !language_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( ALLOC_ARRAY( ltl, s->LangSysCount + 1, ULong ) )
      return error;

    for ( n = 0; n < s->LangSysCount; n++ )
      ltl[n] = lsr[n].LangSysTag;
    ltl[n] = 0;

    *language_tag_list = ltl;

    return TT_Err_Ok;
  }


  /* selecting 0xFFFF for language_index asks for the values of the
     default language (DefaultLangSys)                              */

  EXPORT_FUNC
  TT_Error  TT_GSUB_Query_Features( TTO_GSUBHeader*  gsub,
                                    TT_UShort        script_index,
                                    TT_UShort        language_index,
                                    TT_ULong**       feature_tag_list )
  {
    UShort              n;
    TT_Error            error;
    ULong*              ftl;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;
    TTO_LangSys*        ls;
    UShort*             fi;

    TTO_FeatureList*    fl;
    TTO_FeatureRecord*  fr;


    if ( !gsub || !feature_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gsub->ScriptList;
    sr = sl->ScriptRecord;

    fl = &gsub->FeatureList;
    fr = fl->FeatureRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( language_index == 0xFFFF )
      ls = &s->DefaultLangSys;
    else
    {
      if ( language_index >= s->LangSysCount )
        return TT_Err_Invalid_Argument;

      ls = &lsr[language_index].LangSys;
    }

    fi = ls->FeatureIndex;

    if ( ALLOC_ARRAY( ftl, ls->FeatureCount + 1, ULong ) )
      return error;

    for ( n = 0; n < ls->FeatureCount; n++ )
    {
      if ( fi[n] >= fl->FeatureCount )
      {
        FREE( ftl );
        return TTO_Err_Invalid_GSUB_SubTable_Format;
      }
      ftl[n] = fr[fi[n]].FeatureTag;
    }
    ftl[n] = 0;

    *feature_tag_list = ftl;

    return TT_Err_Ok;
  }


  /* Do an individual subtable lookup.  Returns TT_Err_Ok if substitution
     has been done, or TTO_Err_Not_Covered if not.                        */

  static TT_Error  Do_Glyph_Lookup( TTO_GSUBHeader*   gsub,
                                    UShort            lookup_index,
                                    TTO_GSUB_String*  in,
                                    TTO_GSUB_String*  out,
                                    UShort            context_length,
                                    int               nesting_level )
  {
    TT_Error     error = TT_Err_Ok;
    UShort       i, flags;
    TTO_Lookup*  lo;


    nesting_level++;

    if ( nesting_level > TTO_MAX_NESTING_LEVEL )
      return TTO_Err_Too_Many_Nested_Contexts;

    lo    = &gsub->LookupList.Lookup[lookup_index];
    flags = lo->LookupFlag;

    for ( i = 0; i < lo->SubTableCount; i++ )
    {
      switch ( lo->LookupType )
      {
      case GSUB_LOOKUP_SINGLE:
        error = Lookup_SingleSubst( &lo->SubTable[i].st.gsub.single,
                                    in, out,
                                    flags, context_length, gsub->gdef );
        break;

      case GSUB_LOOKUP_MULTIPLE:
        error = Lookup_MultipleSubst( &lo->SubTable[i].st.gsub.multiple,
                                      in, out,
                                      flags, context_length, gsub->gdef );
        break;

      case GSUB_LOOKUP_ALTERNATE:
        error = Lookup_AlternateSubst( gsub,
                                       &lo->SubTable[i].st.gsub.alternate,
                                       in, out,
                                       flags, context_length, gsub->gdef );
        break;

      case GSUB_LOOKUP_LIGATURE:
        error = Lookup_LigatureSubst( &lo->SubTable[i].st.gsub.ligature,
                                      in, out,
                                      flags, context_length, gsub->gdef );
        break;

      case GSUB_LOOKUP_CONTEXT:
        error = Lookup_ContextSubst( gsub, &lo->SubTable[i].st.gsub.context,
                                     in, out,
                                     flags, context_length, nesting_level );
        break;

      case GSUB_LOOKUP_CHAIN:
        error = Lookup_ChainContextSubst( gsub,
                                          &lo->SubTable[i].st.gsub.chain,
                                          in, out,
                                          flags, context_length,
                                          nesting_level );
        break;
      }

      /* Check whether we have a successful substitution or an error other
         than TTO_Err_Not_Covered                                          */

      if ( error != TTO_Err_Not_Covered )
        return error;
    }

    return TTO_Err_Not_Covered;
  }


  /* apply one lookup to the input string object */

  static TT_Error  Do_String_Lookup( TTO_GSUBHeader*   gsub,
                                     UShort            lookup_index,
                                     TTO_GSUB_String*  in,
                                     TTO_GSUB_String*  out )
  {
    TT_Error  error = TTO_Err_Not_Covered;

    UShort*  properties = gsub->LookupList.Properties;
    UShort*  p_in       = in->properties;
    UShort*  s_in       = in->string;

    int      nesting_level = 0;


    while ( in->pos < in->length )
    {
      if ( ~p_in[in->pos] & properties[lookup_index] )
      {
        /* 0xFFFF indicates that we don't have a context length yet */
        error = Do_Glyph_Lookup( gsub, lookup_index, in, out,
                                 0xFFFF, nesting_level );
        if ( error && error != TTO_Err_Not_Covered )
          return error;
      }
      else
        error = TTO_Err_Not_Covered;

      if ( error == TTO_Err_Not_Covered ) 
        if ( ADD_String( in, 1, out, 1, &s_in[in->pos] ) )
          return error;
    }

    return error;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Add_Feature( TTO_GSUBHeader*  gsub,
                                 TT_UShort        feature_index,
                                 TT_UShort        property )
  {
    UShort       i;

    TTO_Feature  feature;
    UShort*      properties;
    UShort*      index;


    if ( !gsub ||
         feature_index >= gsub->FeatureList.FeatureCount )
      return TT_Err_Invalid_Argument;

    properties = gsub->LookupList.Properties;

    feature = gsub->FeatureList.FeatureRecord[feature_index].Feature;
    index   = feature.LookupListIndex;

    for ( i = 0; i < feature.LookupListCount; i++ )
      properties[index[i]] |= property;

    return TT_Err_Ok;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Clear_Features( TTO_GSUBHeader*  gsub )
  {
    UShort   i;

    UShort*  properties;


    if ( !gsub )
      return TT_Err_Invalid_Argument;

    properties = gsub->LookupList.Properties;

    for ( i = 0; i < gsub->LookupList.LookupCount; i++ )
      properties[i] = 0;

    return TT_Err_Ok;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Register_Alternate_Function( TTO_GSUBHeader*  gsub,
                                                 TTO_AltFunction  alt,
                                                 void*            data )
  {
    if ( !gsub )
      return TT_Err_Invalid_Argument;

    gsub->alt  = alt;
    gsub->data = data;

    return TT_Err_Ok;
  }


  EXPORT_FUNC
  TT_Error  TT_GSUB_Apply_String( TTO_GSUBHeader*   gsub,
                                  TTO_GSUB_String*  in,
                                  TTO_GSUB_String*  out )
  {
    TT_Error          error = TTO_Err_Not_Covered;
    UShort            j;

    TTO_GSUB_String   tmp1;
    TTO_GSUB_String*  ptmp1;
    TTO_GSUB_String   tmp2;
    TTO_GSUB_String*  ptmp2;
    TTO_GSUB_String*  t;

    UShort*           properties;


    if ( !gsub ||
         !in || !out || in->length == 0 || in->pos >= in->length )
      return TT_Err_Invalid_Argument;

    properties = gsub->LookupList.Properties;

    tmp1.length    = in->length;
    tmp1.allocated = in->length;
    tmp1.pos       = in->pos;

    if ( ALLOC_ARRAY( tmp1.string, tmp1.length, UShort ) )
      return error;
    MEM_Copy( tmp1.string, in->string, in->length * sizeof ( UShort ) );

    /* make sure that we always have a `properties' array in the string
       object                                                           */

    if ( ALLOC_ARRAY( tmp1.properties, tmp1.length, UShort ) )
      return error;
    if ( in->properties )
      MEM_Copy( tmp1.properties, in->properties,
                in->length * sizeof( UShort ) );

    tmp2.allocated  = 0;
    tmp2.pos        = 0;
    tmp2.string     = NULL;
    tmp2.properties = NULL;

    ptmp1 = &tmp1;
    ptmp2 = &tmp2;

    for ( j = 0; j < gsub->LookupList.LookupCount; j++ )
      if ( properties[j] )
      {
        error = Do_String_Lookup( gsub, j, ptmp1, ptmp2 );
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        /* flipping `in' and `out', preparing for the next loop */

        ptmp1->pos    = in->pos;
        ptmp2->length = ptmp2->pos;
        ptmp2->pos    = in->pos;

        t     = ptmp2;
        ptmp2 = ptmp1;
        ptmp1 = t;
      }

    out->length    = ptmp1->length;
    out->pos       = 0;
    out->allocated = ptmp1->allocated;
    out->string    = ptmp1->string;

    if ( in->properties )
      out->properties = ptmp1->properties;
    else
    {
      free( ptmp1->properties );
      out->properties = NULL;
    }

    free( ptmp2->string );
    free( ptmp2->properties );

    return error;
  }


/* END */
