/*
 * File: imhist.c
 *
 * MATLAB Coder version            : 3.3
 * C/C++ source code generated on  : 08-May-2018 13:02:53
 */

/* Include Files */
#include "./imbw/imbinarize.h"
#include "./imbw/imhist.h"

/* Function Definitions */

/*
 * Arguments    : const unsigned char varargin_1[172530]
 *                double yout[256]
 * Return Type  : void
 */
void imhist(const unsigned char varargin_1[172530], double yout[256])
{
  double localBins1[256];
  double localBins2[256];
  double localBins3[256];
  int i;
  memset(&yout[0], 0, sizeof(double) << 8);
  memset(&localBins1[0], 0, sizeof(double) << 8);
  memset(&localBins2[0], 0, sizeof(double) << 8);
  memset(&localBins3[0], 0, sizeof(double) << 8);
  for (i = 0; i + 4 <= 172530; i += 4) {
    localBins1[varargin_1[i]]++;
    localBins2[varargin_1[i + 1]]++;
    localBins3[varargin_1[i + 2]]++;
    yout[varargin_1[i + 3]]++;
  }

  while (i + 1 <= 172530) {
    yout[varargin_1[i]]++;
    i++;
  }

  for (i = 0; i < 256; i++) {
    yout[i] = ((yout[i] + localBins1[i]) + localBins2[i]) + localBins3[i];
  }
}

/*
 * File trailer for imhist.c
 *
 * [EOF]
 */
