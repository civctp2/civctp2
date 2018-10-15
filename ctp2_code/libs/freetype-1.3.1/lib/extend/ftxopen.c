/*******************************************************************
 *
 *  ftxopen.c
 *
 *    TrueType Open common table support.
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

#include "tttypes.h"
#include "ttload.h"
#include "ttextend.h"
#include "ttmemory.h"
#include "ttfile.h"

#include "ftxopen.h"
#include "ftxopenf.h"


  /***************************
   * Script related functions
   ***************************/


  /* LangSys */

  static TT_Error  Load_LangSys( TTO_LangSys*  ls,
                                 PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;

    UShort*  fi;


    if ( ACCESS_Frame( 6L ) )
      return error;

    ls->LookupOrderOffset    = GET_UShort();    /* should be 0 */
    ls->ReqFeatureIndex      = GET_UShort();
    count = ls->FeatureCount = GET_UShort();

    FORGET_Frame();

    ls->FeatureIndex = NULL;

    if ( ALLOC_ARRAY( ls->FeatureIndex, count, UShort ) )
      return error;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( ls->FeatureIndex );
      return error;
    }

    fi = ls->FeatureIndex;

    for ( n = 0; n < count; n++ )
      fi[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  static void  Free_LangSys( TTO_LangSys*  ls )
  {
    FREE( ls->FeatureIndex );
  }


  /* Script */

  static TT_Error  Load_Script( TTO_Script*  s,
                                PFace        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort              n, count;
    ULong               cur_offset, new_offset, base_offset;

    TTO_LangSysRecord*  lsr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    if ( new_offset != base_offset )        /* not a NULL offset */
    {
      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LangSys( &s->DefaultLangSys,
                                   input ) ) != TT_Err_Ok )
        return error;
      (void)FILE_Seek( cur_offset );
    }
    else
    {
      /* we create a DefaultLangSys table with no entries */

      s->DefaultLangSys.LookupOrderOffset = 0;
      s->DefaultLangSys.ReqFeatureIndex   = 0xFFFF;
      s->DefaultLangSys.FeatureCount      = 0;
      s->DefaultLangSys.FeatureIndex      = NULL;
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = s->LangSysCount = GET_UShort();

    /* safety check; otherwise the official handling of TrueType Open
       fonts won't work */

    if ( s->LangSysCount == 0 && s->DefaultLangSys.FeatureCount == 0 )
    {
      error = TTO_Err_Invalid_SubTable;
      goto Fail2;
    }

    FORGET_Frame();

    s->LangSysRecord = NULL;

    if ( ALLOC_ARRAY( s->LangSysRecord, count, TTO_LangSysRecord ) )
      goto Fail2;

    lsr = s->LangSysRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 6L ) )
        goto Fail1;

      lsr[n].LangSysTag = GET_ULong();
      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LangSys( &lsr[n].LangSys, input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_LangSys( &lsr[n].LangSys );

    FREE( s->LangSysRecord );

  Fail2:
    Free_LangSys( &s->DefaultLangSys );
    return error;
  }


  static void  Free_Script( TTO_Script*  s )
  {
    UShort              n, count;

    TTO_LangSysRecord*  lsr;


    Free_LangSys( &s->DefaultLangSys );

    if ( s->LangSysRecord )
    {
      count = s->LangSysCount;
      lsr   = s->LangSysRecord;

      for ( n = 0; n < count; n++ )
        Free_LangSys( &lsr[n].LangSys );

      FREE( lsr );
    }
  }


  /* ScriptList */

  TT_Error  Load_ScriptList( TTO_ScriptList*  sl,
                             PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_ScriptRecord*  sr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = sl->ScriptCount = GET_UShort();

    FORGET_Frame();

    sl->ScriptRecord = NULL;

    if ( ALLOC_ARRAY( sl->ScriptRecord, count, TTO_ScriptRecord ) )
      return error;

    sr = sl->ScriptRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 6L ) )
        goto Fail;

      sr[n].ScriptTag = GET_ULong();
      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Script( &sr[n].Script, input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_Script( &sr[n].Script );

    FREE( sl->ScriptRecord );
    return error;
  }


  void  Free_ScriptList( TTO_ScriptList*  sl )
  {
    UShort             n, count;

    TTO_ScriptRecord*  sr;


    if ( sl->ScriptRecord )
    {
      count = sl->ScriptCount;
      sr    = sl->ScriptRecord;

      for ( n = 0; n < count; n++ )
        Free_Script( &sr[n].Script );

      FREE( sr );
    }
  }



  /*********************************
   * Feature List related functions
   *********************************/


  /* Feature */

  static TT_Error  Load_Feature( TTO_Feature*  f,
                                 PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;

    UShort*  lli;


    if ( ACCESS_Frame( 4L ) )
      return error;

    f->FeatureParams           = GET_UShort();    /* should be 0 */
    count = f->LookupListCount = GET_UShort();

    FORGET_Frame();

    f->LookupListIndex = NULL;

    if ( ALLOC_ARRAY( f->LookupListIndex, count, UShort ) )
      return error;

    lli = f->LookupListIndex;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( f->LookupListIndex );
      return error;
    }

    for ( n = 0; n < count; n++ )
      lli[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  static void  Free_Feature( TTO_Feature*  f )
  {
    FREE( f->LookupListIndex );
  }


  /* FeatureList */

  TT_Error  Load_FeatureList( TTO_FeatureList*  fl,
                              PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort              n, count;
    ULong               cur_offset, new_offset, base_offset;

    TTO_FeatureRecord*  fr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = fl->FeatureCount = GET_UShort();

    FORGET_Frame();

    fl->FeatureRecord = NULL;

    if ( ALLOC_ARRAY( fl->FeatureRecord, count, TTO_FeatureRecord ) )
      return error;

    fr = fl->FeatureRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 6L ) )
        goto Fail;

      fr[n].FeatureTag = GET_ULong();
      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Feature( &fr[n].Feature, input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_Feature( &fr[n].Feature );

    FREE( fl->FeatureRecord );
    return error;
  }


  void  Free_FeatureList( TTO_FeatureList*  fl )
  {
    UShort              n, count;

    TTO_FeatureRecord*  fr;


    if ( fl->FeatureRecord )
    {
      count = fl->FeatureCount;
      fr    = fl->FeatureRecord;

      for ( n = 0; n < count; n++ )
        Free_Feature( &fr[n].Feature );

      FREE( fr );
    }
  }



  /********************************
   * Lookup List related functions
   ********************************/

  /* the subroutines of the following two functions are defined in
     ftxgsub.c and ftxgpos.c respectively                          */


  /* SubTable */

  static TT_Error  Load_SubTable( TTO_SubTable*  st,
                                  PFace          input,
                                  TTO_Type       table_type,
                                  UShort         lookup_type )
  {
    if ( table_type == GSUB )
      switch ( lookup_type )
      {
      case GSUB_LOOKUP_SINGLE:
        return Load_SingleSubst( &st->st.gsub.single, input );

      case GSUB_LOOKUP_MULTIPLE:
        return Load_MultipleSubst( &st->st.gsub.multiple, input );

      case GSUB_LOOKUP_ALTERNATE:
        return Load_AlternateSubst( &st->st.gsub.alternate, input );

      case GSUB_LOOKUP_LIGATURE:
        return Load_LigatureSubst( &st->st.gsub.ligature, input );

      case GSUB_LOOKUP_CONTEXT:
        return Load_ContextSubst( &st->st.gsub.context, input );

      case GSUB_LOOKUP_CHAIN:
        return Load_ChainContextSubst( &st->st.gsub.chain, input );

      default:
        return TTO_Err_Invalid_GSUB_SubTable_Format;
      }
    else
      switch ( lookup_type )
      {
        case GPOS_LOOKUP_SINGLE:
          return Load_SinglePos( &st->st.gpos.single, input );

        case GPOS_LOOKUP_PAIR:
          return Load_PairPos( &st->st.gpos.pair, input );

        case GPOS_LOOKUP_CURSIVE:
          return Load_CursivePos( &st->st.gpos.cursive, input );

        case GPOS_LOOKUP_MARKBASE:
          return Load_MarkBasePos( &st->st.gpos.markbase, input );

        case GPOS_LOOKUP_MARKLIG:
          return Load_MarkLigPos( &st->st.gpos.marklig, input );

        case GPOS_LOOKUP_MARKMARK:
          return Load_MarkMarkPos( &st->st.gpos.markmark, input );

        case GPOS_LOOKUP_CONTEXT:
          return Load_ContextPos( &st->st.gpos.context, input );

        case GPOS_LOOKUP_CHAIN:
          return Load_ChainContextPos ( &st->st.gpos.chain, input );

        default:
          return TTO_Err_Invalid_GPOS_SubTable_Format;
      }

    return TT_Err_Ok;           /* never reached */
  }


  static void  Free_SubTable( TTO_SubTable*  st,
                              TTO_Type       table_type,
                              UShort         lookup_type )
  {
    if ( table_type == GSUB )
      switch ( lookup_type )
      {
      case GSUB_LOOKUP_SINGLE:
        Free_SingleSubst( &st->st.gsub.single );
        break;

      case GSUB_LOOKUP_MULTIPLE:
        Free_MultipleSubst( &st->st.gsub.multiple );
        break;

      case GSUB_LOOKUP_ALTERNATE:
        Free_AlternateSubst( &st->st.gsub.alternate );
        break;

      case GSUB_LOOKUP_LIGATURE:
        Free_LigatureSubst( &st->st.gsub.ligature );
        break;

      case GSUB_LOOKUP_CONTEXT:
        Free_ContextSubst( &st->st.gsub.context );
        break;

      case GSUB_LOOKUP_CHAIN:
        Free_ChainContextSubst( &st->st.gsub.chain );
        break;
      }
    else
      switch ( lookup_type )
      {
      case GPOS_LOOKUP_SINGLE:
        Free_SinglePos( &st->st.gpos.single );
        break;

      case GPOS_LOOKUP_PAIR:
        Free_PairPos( &st->st.gpos.pair );
        break;

      case GPOS_LOOKUP_CURSIVE:
        Free_CursivePos( &st->st.gpos.cursive );
        break;

      case GPOS_LOOKUP_MARKBASE:
        Free_MarkBasePos( &st->st.gpos.markbase );
        break;

      case GPOS_LOOKUP_MARKLIG:
        Free_MarkLigPos( &st->st.gpos.marklig );
        break;

      case GPOS_LOOKUP_MARKMARK:
        Free_MarkMarkPos( &st->st.gpos.markmark );
        break;

      case GPOS_LOOKUP_CONTEXT:
        Free_ContextPos( &st->st.gpos.context );
        break;

      case GPOS_LOOKUP_CHAIN:
        Free_ChainContextPos ( &st->st.gpos.chain );
        break;
      }
  }


  /* Lookup */

  static TT_Error  Load_Lookup( TTO_Lookup*   l,
                                PFace         input,
                                TTO_Type      type )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort         n, count;
    ULong          cur_offset, new_offset, base_offset;

    TTO_SubTable*  st;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 6L ) )
      return error;

    l->LookupType            = GET_UShort();
    l->LookupFlag            = GET_UShort();
    count = l->SubTableCount = GET_UShort();

    FORGET_Frame();

    l->SubTable = NULL;

    if ( ALLOC_ARRAY( l->SubTable, count, TTO_SubTable ) )
      return error;

    st = l->SubTable;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_SubTable( &st[n], input,
                                    type, l->LookupType ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_SubTable( &st[n], type, l->LookupType );

    FREE( l->SubTable );
    return error;
  }


  static void  Free_Lookup( TTO_Lookup*   l,
                            TTO_Type      type )
  {
    UShort         n, count;

    TTO_SubTable*  st;


    if ( l->SubTable )
    {
      count = l->SubTableCount;
      st    = l->SubTable;

      for ( n = 0; n < count; n++ )
        Free_SubTable( &st[n], type, l->LookupType );

      FREE( st );
    }
  }


  /* LookupList */

  TT_Error  Load_LookupList( TTO_LookupList*  ll,
                             PFace            input,
                             TTO_Type         type )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort       n, count;
    ULong        cur_offset, new_offset, base_offset;

    TTO_Lookup*  l;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ll->LookupCount = GET_UShort();

    FORGET_Frame();

    ll->Lookup = NULL;

    if ( ALLOC_ARRAY( ll->Lookup, count, TTO_Lookup ) )
      return error;
    if ( ALLOC_ARRAY( ll->Properties, count, UShort ) )
      goto Fail2;

    l = ll->Lookup;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Lookup( &l[n], input, type ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    FREE( ll->Properties );

    for ( n = 0; n < count; n++ )
      Free_Lookup( &l[n], type );

  Fail2:
    FREE( ll->Lookup );
    return error;
  }


  void  Free_LookupList( TTO_LookupList*  ll,
                         TTO_Type         type )
  {
    UShort       n, count;

    TTO_Lookup*  l;


    FREE( ll->Properties );

    if ( ll->Lookup )
    {
      count = ll->LookupCount;
      l     = ll->Lookup;

      for ( n = 0; n < count; n++ )
        Free_Lookup( &l[n], type );

      FREE( l );
    }
  }



  /*****************************
   * Coverage related functions
   *****************************/


  /* CoverageFormat1 */

  static TT_Error  Load_Coverage1( TTO_CoverageFormat1*  cf1,
                                   PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;

    UShort*  ga;


    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cf1->GlyphCount = GET_UShort();

    FORGET_Frame();

    cf1->GlyphArray = NULL;

    if ( ALLOC_ARRAY( cf1->GlyphArray, count, UShort ) )
      return error;

    ga = cf1->GlyphArray;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( cf1->GlyphArray );
      return error;
    }

    for ( n = 0; n < count; n++ )
      ga[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  static void  Free_Coverage1( TTO_CoverageFormat1*  cf1 )
  {
    FREE( cf1->GlyphArray );
  }


  /* CoverageFormat2 */

  static TT_Error  Load_Coverage2( TTO_CoverageFormat2*  cf2,
                                   PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count;

    TTO_RangeRecord*  rr;


    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cf2->RangeCount = GET_UShort();

    FORGET_Frame();

    cf2->RangeRecord = NULL;

    if ( ALLOC_ARRAY( cf2->RangeRecord, count, TTO_RangeRecord ) )
      return error;

    rr = cf2->RangeRecord;

    if ( ACCESS_Frame( count * 6L ) )
      goto Fail;

    for ( n = 0; n < count; n++ )
    {
      rr[n].Start              = GET_UShort();
      rr[n].End                = GET_UShort();
      rr[n].StartCoverageIndex = GET_UShort();

      /* sanity check; we are limited to 16bit integers */
      if ( rr[n].Start > rr[n].End ||
           ( rr[n].End - rr[n].Start + (long)rr[n].StartCoverageIndex ) >=
             0x10000L )
      {
        error = TTO_Err_Invalid_SubTable;
        goto Fail;
      }
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail:
    FREE( cf2->RangeRecord );
    return error;
  }


  static void  Free_Coverage2( TTO_CoverageFormat2*  cf2 )
  {
    FREE( cf2->RangeRecord );
  }


  TT_Error  Load_Coverage( TTO_Coverage*  c,
                           PFace          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ACCESS_Frame( 2L ) )
      return error;

    c->CoverageFormat = GET_UShort();

    FORGET_Frame();

    switch ( c->CoverageFormat )
    {
    case 1:
      return Load_Coverage1( &c->cf.cf1, input );

    case 2:
      return Load_Coverage2( &c->cf.cf2, input );

    default:
      return TTO_Err_Invalid_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  void  Free_Coverage( TTO_Coverage*  c )
  {
    switch ( c->CoverageFormat )
    {
    case 1:
      Free_Coverage1( &c->cf.cf1 );
      break;

    case 2:
      Free_Coverage2( &c->cf.cf2 );
      break;
    }
  }


  static TT_Error  Coverage_Index1( TTO_CoverageFormat1*  cf1,
                                    UShort                glyphID,
                                    UShort*               index )
  {
    UShort   min, max, new_min, new_max, middle;

    UShort*  array = cf1->GlyphArray;


    /* binary search */

    new_min = 0;
    new_max = cf1->GlyphCount - 1;

    do
    {
      min = new_min;
      max = new_max;

      /* we use (min + max) / 2 = max - (max - min) / 2  to avoid
         overflow and rounding errors                             */

      middle = max - ( ( max - min ) >> 1 );

      if ( glyphID == array[middle] )
      {
        *index = middle;
        return TT_Err_Ok;
      }
      else if ( glyphID < array[middle] )
      {
        if ( middle == min )
          break;
        new_max = middle - 1;
      }
      else
      {
        if ( middle == max )
          break;
        new_min = middle + 1;
      }
    } while ( min < max );

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Coverage_Index2( TTO_CoverageFormat2*  cf2,
                                    UShort                glyphID,
                                    UShort*               index )
  {
    UShort            min, max, new_min, new_max, middle;

    TTO_RangeRecord*  rr = cf2->RangeRecord;


    /* binary search */

    new_min = 0;
    new_max = cf2->RangeCount - 1;

    do
    {
      min = new_min;
      max = new_max;

      /* we use (min + max) / 2 = max - (max - min) / 2  to avoid
         overflow and rounding errors                             */

      middle = max - ( ( max - min ) >> 1 );

      if ( glyphID >= rr[middle].Start && glyphID <= rr[middle].End )
      {
        *index = rr[middle].StartCoverageIndex + glyphID - rr[middle].Start;
        return TT_Err_Ok;
      }
      else if ( glyphID < rr[middle].Start )
      {
        if ( middle == min )
          break;
        new_max = middle - 1;
      }
      else
      {
        if ( middle == max )
          break;
        new_min = middle + 1;
      }
    } while ( min < max );

    return TTO_Err_Not_Covered;
  }


  TT_Error  Coverage_Index( TTO_Coverage*  c,
                            UShort         glyphID,
                            UShort*        index )
  {
    switch ( c->CoverageFormat )
    {
    case 1:
      return Coverage_Index1( &c->cf.cf1, glyphID, index );

    case 2:
      return Coverage_Index2( &c->cf.cf2, glyphID, index );

    default:
      return TTO_Err_Invalid_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }



  /*************************************
   * Class Definition related functions
   *************************************/


  /* ClassDefFormat1 */

  static TT_Error  Load_ClassDef1( TTO_ClassDefinition*  cd,
                                   UShort                limit,
                                   PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;

    UShort*               cva;
    Bool*                 d;

    TTO_ClassDefFormat1*  cdf1;


    cdf1 = &cd->cd.cd1;

    if ( ACCESS_Frame( 4L ) )
      return error;

    cdf1->StartGlyph         = GET_UShort();
    count = cdf1->GlyphCount = GET_UShort();

    FORGET_Frame();

    /* sanity check; we are limited to 16bit integers */

    if ( cdf1->StartGlyph + (long)count >= 0x10000L )
      return TTO_Err_Invalid_SubTable;

    cdf1->ClassValueArray = NULL;

    if ( ALLOC_ARRAY( cdf1->ClassValueArray, count, UShort ) )
      return error;

    d   = cd->Defined;
    cva = cdf1->ClassValueArray;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail;

    for ( n = 0; n < count; n++ )
    {
      cva[n] = GET_UShort();
      if ( cva[n] >= limit )
      {
        error = TTO_Err_Invalid_SubTable;
        goto Fail;
      }
      d[cva[n]] = TRUE;
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail:
    FREE( cva );

    return error;
  }


  static void  Free_ClassDef1( TTO_ClassDefFormat1*  cdf1 )
  {
    FREE( cdf1->ClassValueArray );
  }


  /* ClassDefFormat2 */

  static TT_Error  Load_ClassDef2 ( TTO_ClassDefinition*  cd,
                                    UShort                limit,
                                    PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                 n, count;

    TTO_ClassRangeRecord*  crr;
    Bool*                  d;

    TTO_ClassDefFormat2*   cdf2;


    cdf2 = &cd->cd.cd2;

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cdf2->ClassRangeCount = GET_UShort();

    FORGET_Frame();

    cdf2->ClassRangeRecord = NULL;

    if ( ALLOC_ARRAY( cdf2->ClassRangeRecord, count, TTO_ClassRangeRecord ) )
      return error;

    d   = cd->Defined;
    crr = cdf2->ClassRangeRecord;

    if ( ACCESS_Frame( count * 6L ) )
      goto Fail;

    for ( n = 0; n < count; n++ )
    {
      crr[n].Start = GET_UShort();
      crr[n].End   = GET_UShort();
      crr[n].Class = GET_UShort();

      /* sanity check */

      if ( crr[n].Start > crr[n].End ||
           crr[n].Class >= limit )
      {
        error = TTO_Err_Invalid_SubTable;
        goto Fail;
      }
      d[crr[n].Class] = TRUE;
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail:
    FREE( crr );

    return error;
  }


  static void  Free_ClassDef2( TTO_ClassDefFormat2*  cdf2 )
  {
    FREE( cdf2->ClassRangeRecord );
  }


  /* ClassDefinition */

  TT_Error  Load_ClassDefinition( TTO_ClassDefinition*  cd,
                                  UShort                limit,
                                  PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ALLOC_ARRAY( cd->Defined, limit, Bool ) )
      return error;

    if ( ACCESS_Frame( 2L ) )
      goto Fail;

    cd->ClassFormat = GET_UShort();

    FORGET_Frame();

    switch ( cd->ClassFormat )
    {
    case 1:
      error = Load_ClassDef1( cd, limit, input );
      break;

    case 2:
      error = Load_ClassDef2( cd, limit, input );
      break;

    default:
      error = TTO_Err_Invalid_SubTable_Format;
      break;
    }

    if ( error )
      goto Fail;

    cd->loaded = TRUE;

    return TT_Err_Ok;

  Fail:
    FREE( cd->Defined );
    return error;
  }


  void  Free_ClassDefinition( TTO_ClassDefinition*  cd )
  {
    if ( !cd->loaded )
      return;

    FREE( cd->Defined );

    switch ( cd->ClassFormat )
    {
    case 1:
      Free_ClassDef1( &cd->cd.cd1 );
      break;

    case 2:
      Free_ClassDef2( &cd->cd.cd2 );
      break;
    }
  }


  static TT_Error  Get_Class1( TTO_ClassDefFormat1*  cdf1,
                               UShort                glyphID,
                               UShort*               class,
                               UShort*               index )
  {
    UShort*  cva = cdf1->ClassValueArray;


    *index = 0;

    if ( glyphID >= cdf1->StartGlyph &&
         glyphID <= cdf1->StartGlyph + cdf1->GlyphCount )
    {
      *class = cva[glyphID - cdf1->StartGlyph];
      return TT_Err_Ok;
    }
    else
    {
      *class = 0;
      return TTO_Err_Not_Covered;
    }
  }


  /* we need the index value of the last searched class range record
     in case of failure for constructed GDEF tables                  */

  static TT_Error  Get_Class2( TTO_ClassDefFormat2*  cdf2,
                               UShort                glyphID,
                               UShort*               class,
                               UShort*               index )
  {
    TT_Error               error = TT_Err_Ok;
    UShort                 min, max, new_min, new_max, middle;

    TTO_ClassRangeRecord*  crr = cdf2->ClassRangeRecord;


    /* binary search */

    new_min = 0;
    new_max = cdf2->ClassRangeCount - 1;

    do
    {
      min = new_min;
      max = new_max;

      /* we use (min + max) / 2 = max - (max - min) / 2  to avoid
         overflow and rounding errors                             */

      middle = max - ( ( max - min ) >> 1 );

      if ( glyphID >= crr[middle].Start && glyphID <= crr[middle].End )
      {
        *class = crr[middle].Class;
        error  = TT_Err_Ok;
        break;
      }
      else if ( glyphID < crr[middle].Start )
      {
        if ( middle == min )
        {
          *class = 0;
          error  = TTO_Err_Not_Covered;
          break;
        }
        new_max = middle - 1;
      }
      else
      {
        if ( middle == max )
        {
          *class = 0;
          error  = TTO_Err_Not_Covered;
          break;
        }
        new_min = middle + 1;
      }
    } while ( min < max );

    if ( index )
      *index = middle;

    return error;
  }


  TT_Error  Get_Class( TTO_ClassDefinition*  cd,
                       UShort                glyphID,
                       UShort*               class,
                       UShort*               index )
  {
    switch ( cd->ClassFormat )
    {
    case 1:
      return Get_Class1( &cd->cd.cd1, glyphID, class, index );

    case 2:
      return Get_Class2( &cd->cd.cd2, glyphID, class, index );

    default:
      return TTO_Err_Invalid_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }



  /***************************
   * Device related functions
   ***************************/


  TT_Error  Load_Device( TTO_Device*  d,
                         PFace        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;

    UShort*  dv;


    if ( ACCESS_Frame( 6L ) )
      return error;

    d->StartSize   = GET_UShort();
    d->EndSize     = GET_UShort();
    d->DeltaFormat = GET_UShort();

    FORGET_Frame();

    if ( d->StartSize > d->EndSize ||
         d->DeltaFormat == 0 || d->DeltaFormat > 3 )
      return TTO_Err_Invalid_SubTable;

    d->DeltaValue = NULL;

    count = ( ( d->EndSize - d->StartSize + 1 ) >>
                ( 4 - d->DeltaFormat ) ) + 1;

    if ( ALLOC_ARRAY( d->DeltaValue, count, UShort ) )
      return error;

    if ( ACCESS_Frame( count * 2L ) )
    {
      FREE( d->DeltaValue );
      return error;
    }

    dv = d->DeltaValue;

    for ( n = 0; n < count; n++ )
      dv[n] = GET_UShort();

    FORGET_Frame();

    return TT_Err_Ok;
  }


  void  Free_Device( TTO_Device*  d )
  {
    FREE( d->DeltaValue );
  }


  /* Since we have the delta values stored in compressed form, we must
     uncompress it now.  To simplify the interface, the function always
     returns a meaningful value in `value'; the error is just for
     information.
                                 |
     format = 1: 0011223344556677|8899101112131415|...
                                 |
                      byte 1           byte 2

       00: (byte >> 14) & mask
       11: (byte >> 12) & mask
       ...

       mask = 0x0003
                                 |
     format = 2: 0000111122223333|4444555566667777|...
                                 |
                      byte 1           byte 2

       0000: (byte >> 12) & mask
       1111: (byte >>  8) & mask
       ...

       mask = 0x000F
                                 |
     format = 3: 0000000011111111|2222222233333333|...
                                 |
                      byte 1           byte 2

       00000000: (byte >> 8) & mask
       11111111: (byte >> 0) & mask
       ....

       mask = 0x00FF                                    */

  TT_Error  Get_Device( TTO_Device*  d,
                        UShort       size,
                        Short*       value )
  {
    UShort  byte, bits, mask, f, s;


    f = d->DeltaFormat;

    if ( size >= d->StartSize && size <= d->EndSize )
    {
      s    = size - d->StartSize;
      byte = d->DeltaValue[s >> ( 4 - f )];
      bits = byte >> ( 16 - ( s % ( 1 << ( 4 - f ) ) + 1 ) * ( 1 << f ) );
      mask = 0xFFFF >> ( 16 - ( 1 << f ) );

      *value = (Short)( bits & mask );

      /* conversion to a signed value */

      if ( *value >= ( ( mask + 1 ) >> 1 ) )
        *value -= mask + 1;

      return TT_Err_Ok;
    }
    else
    {
      *value = 0;
      return TTO_Err_Not_Covered;
    }
  }


/* END */
