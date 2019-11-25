#ifndef __CIRCULARFIRPROCESSOR_HPP__
#define __CIRCULARFIRPROCESSOR_HPP__

#include "FIRProcessor.hpp"
#include "DelayBuffer.hpp"

class CircularFIRProcessor : public FIRProcessor{
  DelayBuffer & delayBuffer;
public:
  CircularFIRProcessor(DelayBuffer & d)
    : delayBuffer(d)
  {
  }
  void calculate(const sample_t * const in, sample_t * out, const FIRCoefficients & coeffs, const uint32_t size)
  {
    for(uint32_t i=0; i<size; i++){
      delayBuffer.insertSample(in[i]);
      sample_t acc = 0;
      for(uint32_t j=0; j<coeffs.num_taps; j+=8){
        //acc += delayBuffer.getDelayedSample(j) * coeffs.coeffs[j];

        acc +=
          (delayBuffer.getDelayedSample(j+0) *coeffs.coeffs[j+0])
        + (delayBuffer.getDelayedSample(j+1) *coeffs.coeffs[j+1])
        + (delayBuffer.getDelayedSample(j+2) *coeffs.coeffs[j+2])
        + (delayBuffer.getDelayedSample(j+3) *coeffs.coeffs[j+3])
        + (delayBuffer.getDelayedSample(j+4) *coeffs.coeffs[j+4])
        + (delayBuffer.getDelayedSample(j+5) *coeffs.coeffs[j+5])
        + (delayBuffer.getDelayedSample(j+6) *coeffs.coeffs[j+6])
        + (delayBuffer.getDelayedSample(j+7) *coeffs.coeffs[j+7])
          ;


        //acc += delayBuffer.getDelayedSample(j*8+0) * coeffs.coeffs[j*8+0];
        //acc += delayBuffer.getDelayedSample(j*8+1) * coeffs.coeffs[j*8+1];
        //acc += delayBuffer.getDelayedSample(j*8+2) * coeffs.coeffs[j*8+2];
        //acc += delayBuffer.getDelayedSample(j*8+3) * coeffs.coeffs[j*8+3];
        //acc += delayBuffer.getDelayedSample(j*8+4) * coeffs.coeffs[j*8+4];
        //acc += delayBuffer.getDelayedSample(j*8+5) * coeffs.coeffs[j*8+5];
        //acc += delayBuffer.getDelayedSample(j*8+6) * coeffs.coeffs[j*8+6];
        //acc += delayBuffer.getDelayedSample(j*8+7) * coeffs.coeffs[j*8+7];
      }
      out[i] = acc;
     }
  }
};

#endif




//blkCnt = size >> 3;
//
//   while(blkCnt > 0u)
//   {
//      /* Copy four new input samples into the state buffer */
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//
//      /* Set all accumulators to zero */
//      acc0 = 0.0f;
//      acc1 = 0.0f;
//      acc2 = 0.0f;
//      acc3 = 0.0f;
//      acc4 = 0.0f;
//      acc5 = 0.0f;
//      acc6 = 0.0f;
//      acc7 = 0.0f;
//
//      /* Initialize state pointer */
//      px = pState;
//
//      /* Initialize coeff pointer */
//      pb = (pCoeffs);
//
//      /* This is separated from the others to avoid
//       * a call to __aeabi_memmove which would be slower
//       */
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//      *pStateCurnt++ = *in++;
//
//      /* Read the first seven samples from the state buffer:  x[n-numTaps], x[n-numTaps-1], x[n-numTaps-2] */
//      x0 = delayBuffer.getDelayedSample(0);
//      x1 = delayBuffer.getDelayedSample(1);
//      x2 = delayBuffer.getDelayedSample(2);
//      x3 = delayBuffer.getDelayedSample(3);
//      x4 = delayBuffer.getDelayedSample(4);
//      x5 = delayBuffer.getDelayedSample(5);
//      x6 = delayBuffer.getDelayedSample(6);
//
//      /* Loop unrolling.  Process 8 taps at a time. */
//      tapCnt = numTaps >> 3u;
//
//      /* Loop over the number of taps.  Unroll by a factor of 8.
//       ** Repeat until we've computed numTaps-8 coefficients. */
//      while(tapCnt > 0u)
//      {
//         /* Read the b[numTaps-1] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-3] sample */
//         x7 = *(px++);
//
//         /* acc0 +=  b[numTaps-1] * x[n-numTaps] */
//         acc0 += x0 * c0;
//
//         /* acc1 +=  b[numTaps-1] * x[n-numTaps-1] */
//         acc1 += x1 * c0;
//
//         /* acc2 +=  b[numTaps-1] * x[n-numTaps-2] */
//         acc2 += x2 * c0;
//
//         /* acc3 +=  b[numTaps-1] * x[n-numTaps-3] */
//         acc3 += x3 * c0;
//
//         /* acc4 +=  b[numTaps-1] * x[n-numTaps-4] */
//         acc4 += x4 * c0;
//
//         /* acc1 +=  b[numTaps-1] * x[n-numTaps-5] */
//         acc5 += x5 * c0;
//
//         /* acc2 +=  b[numTaps-1] * x[n-numTaps-6] */
//         acc6 += x6 * c0;
//
//         /* acc3 +=  b[numTaps-1] * x[n-numTaps-7] */
//         acc7 += x7 * c0;
//
//         /* Read the b[numTaps-2] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-4] sample */
//         x0 = *(px++);
//
//         /* Perform the multiply-accumulate */
//         acc0 += x1 * c0;
//         acc1 += x2 * c0;
//         acc2 += x3 * c0;
//         acc3 += x4 * c0;
//         acc4 += x5 * c0;
//         acc5 += x6 * c0;
//         acc6 += x7 * c0;
//         acc7 += x0 * c0;
//
//         /* Read the b[numTaps-3] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-5] sample */
//         x1 = *(px++);
//
//         /* Perform the multiply-accumulates */
//         acc0 += x2 * c0;
//         acc1 += x3 * c0;
//         acc2 += x4 * c0;
//         acc3 += x5 * c0;
//         acc4 += x6 * c0;
//         acc5 += x7 * c0;
//         acc6 += x0 * c0;
//         acc7 += x1 * c0;
//
//         /* Read the b[numTaps-4] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-6] sample */
//         x2 = *(px++);
//
//         /* Perform the multiply-accumulates */
//         acc0 += x3 * c0;
//         acc1 += x4 * c0;
//         acc2 += x5 * c0;
//         acc3 += x6 * c0;
//         acc4 += x7 * c0;
//         acc5 += x0 * c0;
//         acc6 += x1 * c0;
//         acc7 += x2 * c0;
//
//         /* Read the b[numTaps-4] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-6] sample */
//         x3 = *(px++);
//         /* Perform the multiply-accumulates */
//         acc0 += x4 * c0;
//         acc1 += x5 * c0;
//         acc2 += x6 * c0;
//         acc3 += x7 * c0;
//         acc4 += x0 * c0;
//         acc5 += x1 * c0;
//         acc6 += x2 * c0;
//         acc7 += x3 * c0;
//
//         /* Read the b[numTaps-4] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-6] sample */
//         x4 = *(px++);
//
//         /* Perform the multiply-accumulates */
//         acc0 += x5 * c0;
//         acc1 += x6 * c0;
//         acc2 += x7 * c0;
//         acc3 += x0 * c0;
//         acc4 += x1 * c0;
//         acc5 += x2 * c0;
//         acc6 += x3 * c0;
//         acc7 += x4 * c0;
//
//         /* Read the b[numTaps-4] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-6] sample */
//         x5 = *(px++);
//
//         /* Perform the multiply-accumulates */
//         acc0 += x6 * c0;
//         acc1 += x7 * c0;
//         acc2 += x0 * c0;
//         acc3 += x1 * c0;
//         acc4 += x2 * c0;
//         acc5 += x3 * c0;
//         acc6 += x4 * c0;
//         acc7 += x5 * c0;
//
//         /* Read the b[numTaps-4] coefficient */
//         c0 = *(pb++);
//
//         /* Read x[n-numTaps-6] sample */
//         x6 = *(px++);
//
//         /* Perform the multiply-accumulates */
//         acc0 += x7 * c0;
//         acc1 += x0 * c0;
//         acc2 += x1 * c0;
//         acc3 += x2 * c0;
//         acc4 += x3 * c0;
//         acc5 += x4 * c0;
//         acc6 += x5 * c0;
//         acc7 += x6 * c0;
//
//         tapCnt--;
//      }
//
//
//      /* Advance the state pointer by 8 to process the next group of 8 samples */
//      pState = pState + 8;
//
//      /* The results in the 8 accumulators, store in the destination buffer. */
//      *pDst++ = acc0;
//      *pDst++ = acc1;
//      *pDst++ = acc2;
//      *pDst++ = acc3;
//      *pDst++ = acc4;
//      *pDst++ = acc5;
//      *pDst++ = acc6;
//      *pDst++ = acc7;
//
//      blkCnt--;
//   }
//
//   /* Processing is complete.
//   ** Now copy the last numTaps - 1 samples to the start of the state buffer.
//   ** This prepares the state buffer for the next function call. */
//
//   /* Points to the start of the state buffer */
//   //pStateCurnt = S->pState;
//   pStateCurnt = blah;
//
//   tapCnt = (numTaps - 1u) >> 2u;
//
//   /* copy data */
//   while(tapCnt > 0u)
//   {
//      *pStateCurnt++ = *pState++;
//      *pStateCurnt++ = *pState++;
//      *pStateCurnt++ = *pState++;
//      *pStateCurnt++ = *pState++;
//
//      /* Decrement the loop counter */
//      tapCnt--;
//   }
//
//   /* Calculate remaining number of copies */
//   tapCnt = (numTaps - 1u) % 0x4u;
//
//   /* Copy the remaining q31_t data */
//   while(tapCnt > 0u)
//   {
//      *pStateCurnt++ = *pState++;
//
//      /* Decrement the loop counter */
//      tapCnt--;
//   }
//
//
//
//
//
