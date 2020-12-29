/*
 * File: imbinarize.c
 *
 * MATLAB Coder version            : 3.3
 * C/C++ source code generated on  : 08-May-2018 13:02:53
 */

/* Include Files */
#include "./imbw/imbinarize.h"
#include "./imbw/imhist.h"

/* Function Declarations */
static double computeGlobalThreshold(const unsigned char I[172530]);

/* Function Definitions */

/*
 * Otsu's threshold is used to compute the global threshold. We convert
 *  floating point images to uint8 prior to computing the image histogram to
 *  avoid issues with NaN/Inf values in the input data. im2uint8 nicely
 *  handles these so that we get a clean histogram for otsuthresh. For other
 *  types, we compute the histogram in the native type, using 256 bins (this
 *  is the default in imhist).
 * Arguments    : const unsigned char I[172530]
 * Return Type  : double
 */
static double computeGlobalThreshold(const unsigned char I[172530])
{
  double T;
  double sigma_b_squared[256];
  double num_elems;
  int k;
  double omega[256];
  double mu[256];
  double maxval;
  double num_maxval;
  imhist(I, sigma_b_squared);
  num_elems = 0.0;
  for (k = 0; k < 256; k++) {
    num_elems += sigma_b_squared[k];
  }

  omega[0] = sigma_b_squared[0] / num_elems;
  mu[0] = sigma_b_squared[0] / num_elems;
  for (k = 0; k < 255; k++) {
    num_maxval = sigma_b_squared[k + 1] / num_elems;
    omega[k + 1] = omega[k] + num_maxval;
    mu[k + 1] = mu[k] + num_maxval * (2.0 + (double)k);
  }

  maxval = -1.7976931348623157E+308;
  for (k = 0; k < 256; k++) {
    num_elems = mu[255] * omega[k] - mu[k];
    num_elems = num_elems * num_elems / (omega[k] * (1.0 - omega[k]));
    if (!(maxval > num_elems)) {
      maxval = num_elems;
    }

    sigma_b_squared[k] = num_elems;
  }

  num_elems = 0.0;
  num_maxval = 0.0;
  for (k = 0; k < 256; k++) {
    num_elems += (double)((1 + k) * (sigma_b_squared[k] == maxval));
    num_maxval += (double)(sigma_b_squared[k] == maxval);
  }

  num_elems /= num_maxval;
  T = (num_elems - 1.0) / 255.0;

  /* -------------------------------------------------------------------------- */
  /*  Input Parsing */
  /* -------------------------------------------------------------------------- */
  /*  function [I,isNumericThreshold,options] = parseInputs(I, varargin) */
  /*   */
  /*  % validate image */
  /*  validateImage(I); */
  /*   */
  /*  isNumericThreshold = ~isempty(varargin) && ~ischar(varargin{1}); */
  /*   */
  /*  if isNumericThreshold */
  /*       */
  /*      options.T = validateT(varargin{1},size(I)); */
  /*       */
  /*      if numel(varargin)>1 */
  /*          error(message('MATLAB:TooManyInputs')) */
  /*      end */
  /*       */
  /*  else */
  /*      if isempty(varargin) */
  /*          options.Method = 'global'; */
  /*          return; */
  /*      end */
  /*       */
  /*      options.Method = validatestring(varargin{1},{'global','adaptive'},mfilename,'Method',2); */
  /*       */
  /*      if strcmp(options.Method,'global') */
  /*           */
  /*          if numel(varargin)>1 */
  /*              error(message('MATLAB:TooManyInputs')) */
  /*          end */
  /*      else */
  /*          options.Sensitivity = 0.5; */
  /*          options.ForegroundPolarity = 'bright'; */
  /*           */
  /*          numPVArgs = numel(varargin)-1; */
  /*          if mod(numPVArgs,2)~=0 */
  /*              error(message('images:validate:invalidNameValue')); */
  /*          end */
  /*           */
  /*          ParamNames = {'Sensitivity','ForegroundPolarity'}; */
  /*          ValidateFcn = {@validateSensitivity,@validateForegroundPolarity}; */
  /*           */
  /*          for p = 2 : 2 : numel(varargin)-1 */
  /*               */
  /*              Name = varargin{p}; */
  /*              Value = varargin{p+1}; */
  /*               */
  /*              idx = strncmpi(Name, ParamNames, numel(Name)); */
  /*               */
  /*              if ~any(idx) */
  /*                  error(message('images:validate:unknownParamName', Name)); */
  /*              elseif numel(find(idx))>1 */
  /*                  error(message('images:validate:ambiguousParamName', Name)); */
  /*              end */
  /*               */
  /*              validate = ValidateFcn{idx}; */
  /*              options.(ParamNames{idx}) = validate(Value); */
  /*               */
  /*          end */
  /*      end */
  /*       */
  /*       */
  /*  end */
  /*  end */
  /*   */
  /*  function validateImage(I) */
  /*   */
  /*  supportedClasses = {'uint8','uint16','uint32','int8','int16','int32','single','double'}; */
  /*  supportedAttribs = {'real','nonsparse','2d'}; */
  /*  validateattributes(I,supportedClasses,supportedAttribs,mfilename,'I'); */
  /*   */
  /*  end */
  /*   */
  /*  function T = validateT(T,sizeI) */
  /*   */
  /*  validateattributes(T,{'numeric'},{'real','nonsparse','2d'},mfilename,'Threshold',2); */
  /*   */
  /*  if ~( isscalar(T) || isequal(size(T),sizeI) ) */
  /*      error(message('images:imbinarize:badSizedThreshold')) */
  /*  end */
  /*   */
  /*  end */
  /*   */
  /*  function s = validateSensitivity(s) */
  /*  validateattributes(s,{'numeric'},{'real','nonsparse','scalar','nonnegative','<=',1},mfilename,'Sensitivity'); */
  /*  end */
  /*   */
  /*  function fgp = validateForegroundPolarity(fgp) */
  /*  fgp = validatestring(fgp,{'bright','dark'},mfilename,'ForegroundPolarity'); */
  /*  end */
  return T;
}

/*
 * IMBINARIZE Binarize image by thresholding.
 *    BW = IMBINARIZE(I) binarizes image I with a global threshold computed
 *    using Otsu's method, which chooses the threshold to minimize the
 *    intraclass variance of the thresholded black and white pixels. BW is
 *    the output binary image.
 *
 *    BW = IMBINARIZE(I, METHOD) binarizes image I with the threshold method
 *    specified using METHOD. Available methods are (names can be
 *    abbreviated):
 *
 *    'global'    - Global image threshold using Otsu's method, chosen to
 *                  minimize the intraclass variance of the thresholded black
 *                  and white pixels. See GRAYTHRESH for details.
 *
 *    'adaptive'  - Locally adaptive image threshold chosen using local
 *                  first-order image statistics around each pixel. See
 *                  ADAPTTHRESH for details.
 *
 *    BW = IMBINARIZE(I, 'adaptive', PARAM1,VAL1,PARAM2,VAL2,...) binarizes
 *    image I using name-value pairs to control aspects of adaptive
 *    thresholding.
 *
 *    Parameters include:
 *
 *    'Sensitivity'           - Specifies the sensitivity factor in the range
 *                              [0 1] for adaptive thresholding. A high
 *                              sensitivity value leads to thresholding more
 *                              pixels as foreground, at the risk of
 *                              including some background pixels. Default
 *                              value: 0.50
 *
 *    'ForegroundPolarity'    - Specifies the polarity of the foreground with
 *                              respect to the background. Available options
 *                              are:
 *
 *            'bright'        : The foreground is brighter than the
 *                              background. (Default)
 *            'dark'          : The foreground is darker than the background.
 *
 *    BW = IMBINARIZE(I, T) binarizes image I using threshold T. T can be a
 *    global image threshold, specified as a scalar luminance value or a
 *    locally adaptive threshold specified as a matrix of luminance values. T
 *    must have values between 0 and 1. If T is a matrix, it must be of the
 *    same size as image I. Note that the functions GRAYTHRESH, OTSUTHRESH
 *    and ADAPTTHRESH can be used to compute T automatically.
 *
 *    Class Support
 *    -------------
 *    The input image I can be a real, non-sparse, 2-D matrix of one of the
 *    following classes: uint8, int8, uint16, int16, uint32, int32, single or
 *    double. The output binary image BW is a logical matrix of the same size
 *    as I.
 *
 *    Notes
 *    -----
 *    1. The 'global' method uses a 256-bin image histogram to compute Otsu's
 *       threshold. To use a different histogram, see OTSUTHRESH.
 *
 *    2. The 'adaptive' method binarizes the image using a locally adaptive
 *       threshold. A threshold is computed for each pixel using the local
 *       mean intensity around the neighborhood of the pixel. This technique
 *       is also called Bradley's method. To use a different first order
 *       local statistic, see ADAPTTHRESH.
 *
 *    3. The 'adaptive' method uses a neighborhood size of approximately
 *       1/8th of the size of the image (computed as 2*floor(size(I)/16)+1).
 *       To use a different neighborhood size, see ADAPTTHRESH.
 *
 *    4. To produce a binary image from an indexed image, use IND2GRAY to
 *       first convert the image to an intensity image. To produce a binary
 *       image from an RGB image, use RGB2GRAY to first convert the image to
 *       a grayscale intensity image.
 *
 *    5. IMBINARIZE expects floating point images to be normalized in the
 *       range [0,1].
 *
 *    6. If the image contains Infs or NaNs, the behavior of IMBINARIZE for
 *       the 'adaptive' method is undefined. Propagation of Infs or NaNs may
 *       not be localized to the neighborhood around Inf/NaN pixels.
 *
 *
 *    Example 1
 *    ---------
 *    This example binarizes an image using a global image threshold.
 *
 *    I = imread('coins.png');
 *    BW = imbinarize(I);
 *    figure, imshow(BW)
 *
 *    Example 2
 *    ---------
 *    This example binarizes an image using a locally adaptive threshold.
 *
 *    I = imread('rice.png');
 *    BW = imbinarize(I, 'adaptive');
 *    figure, imshow(BW)
 *
 *    Example 3
 *    ---------
 *    This example shows image binarization for images that have darker
 *    foreground than background.
 *
 *    I = imread('printedtext.png');
 *    BW = imbinarize(I,'adaptive','ForegroundPolarity','dark','Sensitivity',0.4);
 *    figure, imshow(BW)
 *
 *
 *    See also GRAYTHRESH, OTSUTHRESH, ADAPTTHRESH, imageSegmenter.
 * Arguments    : const unsigned char I[172530]
 *                boolean_T BW[172530]
 * Return Type  : void
 */
void imbinarize(const unsigned char I[172530], boolean_T BW[172530])
{
  double T;
  int i0;

  /*  Copyright 2015 The MathWorks, Inc. */
  /*  [I,isNumericThreshold,options] = parseInputs(I,varargin{:}); */
  /*   */
  /*  if isNumericThreshold */
  /*      T = options.T; */
  /*  else */
  /*      method = options.Method; */
  /*       */
  /*      if strcmp(method,'global') */
  /*          T = computeGlobalThreshold(I); */
  /*      else */
  /*          sensitivity = options.Sensitivity; */
  /*          fgPolarity  = options.ForegroundPolarity; */
  /*           */
  /*          T = adaptthresh(I,sensitivity,'ForegroundPolarity',fgPolarity); */
  /*      end */
  /*       */
  /*  end */
  /*  Binarize image using computed threshold */
  T = computeGlobalThreshold(I) * 255.0;
  for (i0 = 0; i0 < 172530; i0++) {
    BW[i0] = (I[i0] > T);
  }
}

/*
 * File trailer for imbinarize.c
 *
 * [EOF]
 */
