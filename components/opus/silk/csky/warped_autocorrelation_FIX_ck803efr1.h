
#ifndef __WARPED_AUTOCORRELATION_FIX_CK803EFR1_H__
#define __WARPED_AUTOCORRELATION_FIX_CK803EFR1_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "main_FIX.h"

#undef QC
#define QC  10

#undef QS
#define QS  14

/* Autocorrelations for a warped frequency axis */
#define OVERRIDE_silk_warped_autocorrelation_FIX
void silk_warped_autocorrelation_FIX(
          opus_int32                *corr,                                  /* O    Result [order + 1]                                                          */
          opus_int                  *scale,                                 /* O    Scaling of the correlation vector                                           */
    const opus_int16                *input,                                 /* I    Input data to correlate                                                     */
    const opus_int                  warping_Q16,                            /* I    Warping coefficient                                                         */
    const opus_int                  length,                                 /* I    Length of input                                                             */
    const opus_int                  order,                                  /* I    Correlation order (even)                                                    */
    int                             arch                                    /* I    Run-time architecture                                                       */
)
{
    opus_int   n, i, lsh;
    opus_int32 tmp1_QS=0, tmp2_QS=0, tmp3_QS=0, tmp4_QS=0, tmp5_QS=0, tmp6_QS=0, tmp7_QS=0, tmp8_QS=0, start_1=0, start_2=0, start_3=0;
    opus_int32 state_QS[ MAX_SHAPE_LPC_ORDER + 1 ] = { 0 };
    opus_int64 corr_QC[  MAX_SHAPE_LPC_ORDER + 1 ] = { 0 };
    opus_int64 temp64;

    opus_int32 val;
    val = 2 * QS - QC;

    /* Order must be even */
    silk_assert( ( order & 1 ) == 0 );
    silk_assert( 2 * QS - QC >= 0 );

    /* Loop over samples */
    for( n = 0; n < length; n=n+4 ) {

        tmp1_QS = silk_LSHIFT32( (opus_int32)input[ n ], QS );
        start_1 = tmp1_QS;
        tmp3_QS = silk_LSHIFT32( (opus_int32)input[ n+1], QS );
        start_2 = tmp3_QS;
        tmp5_QS = silk_LSHIFT32( (opus_int32)input[ n+2], QS );
        start_3 = tmp5_QS;
        tmp7_QS = silk_LSHIFT32( (opus_int32)input[ n+3], QS );

        /* Loop over allpass sections */
        for( i = 0; i < order; i += 2 ) {
            /* Output of allpass section */
            tmp2_QS = silk_SMLAWB( state_QS[ i ], state_QS[ i + 1 ] - tmp1_QS, warping_Q16 );
            corr_QC[  i ] = silk_MADD_ck803efr1( corr_QC[  i ], tmp1_QS,  start_1);

            tmp4_QS = silk_SMLAWB( tmp1_QS, tmp2_QS - tmp3_QS, warping_Q16 );
            corr_QC[  i ] = silk_MADD_ck803efr1( corr_QC[  i ], tmp3_QS,  start_2);

            tmp6_QS = silk_SMLAWB( tmp3_QS, tmp4_QS - tmp5_QS, warping_Q16 );
            corr_QC[  i ] = silk_MADD_ck803efr1( corr_QC[  i ], tmp5_QS,  start_3);

            tmp8_QS = silk_SMLAWB( tmp5_QS, tmp6_QS - tmp7_QS, warping_Q16 );
            state_QS[ i ]  = tmp7_QS;
            corr_QC[  i ] = silk_MADD_ck803efr1( corr_QC[  i ], tmp7_QS, state_QS[0]);

            /* Output of allpass section */
            tmp1_QS = silk_SMLAWB( state_QS[ i + 1 ], state_QS[ i + 2 ] - tmp2_QS, warping_Q16 );
            corr_QC[  i+1 ] = silk_MADD_ck803efr1( corr_QC[  i+1 ], tmp2_QS,  start_1);

            tmp3_QS = silk_SMLAWB( tmp2_QS, tmp1_QS - tmp4_QS, warping_Q16 );
            corr_QC[  i+1 ] = silk_MADD_ck803efr1( corr_QC[  i+1 ], tmp4_QS,  start_2);

            tmp5_QS = silk_SMLAWB( tmp4_QS, tmp3_QS - tmp6_QS, warping_Q16 );
            corr_QC[  i+1 ] = silk_MADD_ck803efr1( corr_QC[  i+1 ], tmp6_QS,  start_3);

            tmp7_QS = silk_SMLAWB( tmp6_QS, tmp5_QS - tmp8_QS, warping_Q16 );
            state_QS[ i + 1 ]  = tmp8_QS;
            corr_QC[  i+1 ] = silk_MADD_ck803efr1( corr_QC[  i+1 ], tmp8_QS,  state_QS[ 0 ]);

        }
        state_QS[ order ] = tmp7_QS;

        corr_QC[  order ] = silk_MADD_ck803efr1( corr_QC[  order ], tmp1_QS,  start_1);
        corr_QC[  order ] = silk_MADD_ck803efr1( corr_QC[  order ], tmp3_QS,  start_2);
        corr_QC[  order ] = silk_MADD_ck803efr1( corr_QC[  order ], tmp5_QS,  start_3);
        corr_QC[  order ] = silk_MADD_ck803efr1( corr_QC[  order ], tmp7_QS,  state_QS[ 0 ]);
    }

    for(;n< length; n++ ) {

        tmp1_QS = silk_LSHIFT32( (opus_int32)input[ n ], QS );

        /* Loop over allpass sections */
        for( i = 0; i < order; i += 2 ) {

            /* Output of allpass section */
            tmp2_QS = silk_SMLAWB( state_QS[ i ], state_QS[ i + 1 ] - tmp1_QS, warping_Q16 );
            state_QS[ i ] = tmp1_QS;
            corr_QC[  i ] = silk_MADD_ck803efr1( corr_QC[  i ], tmp1_QS,   state_QS[ 0 ]);

            /* Output of allpass section */
            tmp1_QS = silk_SMLAWB( state_QS[ i + 1 ], state_QS[ i + 2 ] - tmp2_QS, warping_Q16 );
            state_QS[ i + 1 ]  = tmp2_QS;
            corr_QC[  i+1 ] = silk_MADD_ck803efr1( corr_QC[  i+1 ], tmp2_QS,   state_QS[ 0 ]);
        }
        state_QS[ order ] = tmp1_QS;
        corr_QC[  order ] = silk_MADD_ck803efr1( corr_QC[  order ], tmp1_QS,   state_QS[ 0 ]);
    }

    #if 1
    temp64 =  corr_QC[ 0 ];
    temp64 = csky_shilo(temp64, val);
    lsh = silk_CLZ64( temp64 ) - 35;
    #else
    #error "xxx"
    #endif

    lsh = silk_LIMIT( lsh, -12 - QC, 30 - QC );
    *scale = -( QC + lsh );
    silk_assert( *scale >= -30 && *scale <= 12 );

    #if 1
    if( lsh >= 0 ) {
        for( i = 0; i < order + 1; i++ ) {
            temp64 = corr_QC[ i ];
            //temp64 = csky_shilo(temp64, val);
            temp64 = (val >= 0) ? (temp64 >> val) : (temp64 << -val);
            corr[ i ] = (opus_int32)silk_CHECK_FIT32( csky_shilo( temp64, -lsh ) );
        }
    } else {
        for( i = 0; i < order + 1; i++ ) {
            temp64 = corr_QC[ i ];
            //temp64 = csky_shilo(temp64, val);
            temp64 = (val >= 0) ? (temp64 >> val) : (temp64 << -val);
            corr[ i ] = (opus_int32)silk_CHECK_FIT32( csky_shilo( temp64, -lsh ) );
        }
    }

     corr_QC[ 0 ] = csky_shilo(corr_QC[ 0 ], val);
    #else
    if( lsh >= 0 ) {
        for( i = 0; i < order + 1; i++ ) {
            corr[ i ] = (opus_int32)silk_CHECK_FIT32( silk_LSHIFT64( corr_QC[ i ], lsh ) );
        }
    } else {
        for( i = 0; i < order + 1; i++ ) {
            corr[ i ] = (opus_int32)silk_CHECK_FIT32( silk_RSHIFT64( corr_QC[ i ], -lsh ) );
        }
    }
    silk_assert( corr_QC[ 0 ] >= 0 ); /* If breaking, decrease QC*/
    #endif

     silk_assert( corr_QC[ 0 ] >= 0 ); /* If breaking, decrease QC*/
}
#endif /* __WARPED_AUTOCORRELATION_FIX_CK803EFR1_H__ */
