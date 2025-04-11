/*
 * File: ADRC_TOC_NLADRC.c
 *
 * Code generated for Simulink model 'ADRC_TOC_NLADRC'.
 *
 * Model version                  : 1.13
 * Simulink Coder version         : 24.1 (R2024a) 19-Nov-2023
 * C/C++ source code generated on : Fri Apr 11 01:27:31 2025
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "ADRC_TOC_NLADRC.h"
#include <math.h>
#include "rtwtypes.h"
#include "math.h"

/* Block signals and states (default storage) */
DW rtDW;

/* External inputs (root inport signals with default storage) */
ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
ExtY rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
extern real_T rt_powd_snf(real_T u0, real_T u1);
static real_T rtGetInf(void);
static real32_T rtGetInfF(void);
static real_T rtGetMinusInf(void);
static real32_T rtGetMinusInfF(void);
static real_T rtGetNaN(void);
static real32_T rtGetNaNF(void);
extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
real_T rtNaN = -(real_T)NAN;
real_T rtInf = (real_T)INFINITY;
real_T rtMinusInf = -(real_T)INFINITY;
real32_T rtNaNF = -(real32_T)NAN;
real32_T rtInfF = (real32_T)INFINITY;
real32_T rtMinusInfF = -(real32_T)INFINITY;

/* Return rtInf needed by the generated code. */
static real_T rtGetInf(void)
{
  return rtInf;
}

/* Get rtInfF needed by the generated code. */
static real32_T rtGetInfF(void)
{
  return rtInfF;
}

/* Return rtMinusInf needed by the generated code. */
static real_T rtGetMinusInf(void)
{
  return rtMinusInf;
}

/* Return rtMinusInfF needed by the generated code. */
static real32_T rtGetMinusInfF(void)
{
  return rtMinusInfF;
}

/* Return rtNaN needed by the generated code. */
static real_T rtGetNaN(void)
{
  return rtNaN;
}

/* Return rtNaNF needed by the generated code. */
static real32_T rtGetNaNF(void)
{
  return rtNaNF;
}

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  return (boolean_T)(isnan(value) != 0);
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    real_T tmp;
    real_T tmp_0;
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Model step function */
void ADRC_TOC_NLADRC_step(void)
{
  real_T a;
  real_T rtb_Sum4;
  real_T rtb_Sum5;
  real_T rtb_Sum9;
  real_T rtb_dz1;
  real_T x1;
  real_T y;

  /* Sum: '<S1>/Sum4' incorporates:
   *  DiscreteIntegrator: '<S4>/Integrator2'
   *  DiscreteIntegrator: '<S6>/Integrator'
   */
  rtb_Sum4 = rtDW.Integrator2_DSTATE_o - rtDW.Integrator_DSTATE;

  /* Sum: '<S1>/Sum5' incorporates:
   *  DiscreteIntegrator: '<S4>/Integrator'
   *  DiscreteIntegrator: '<S6>/Integrator1'
   */
  rtb_Sum5 = rtDW.Integrator_DSTATE_c - rtDW.Integrator1_DSTATE;

  /* MATLAB Function: '<S2>/MATLAB Function' incorporates:
   *  Constant: '<Root>/Constant3'
   *  Constant: '<Root>/alpha1_N2'
   *  Constant: '<Root>/alpha2_N2'
   */
  a = fabs(rtb_Sum4);
  if (a > 0.1) {
    if (rtIsNaN(rtb_Sum4)) {
      x1 = (rtNaN);
    } else if (rtb_Sum4 < 0.0) {
      x1 = -1.0;
    } else {
      x1 = (rtb_Sum4 > 0.0);
    }

    rtb_dz1 = rt_powd_snf(a, 0.75) * x1;
  } else {
    rtb_dz1 = rtb_Sum4 / 0.56234132519034907;
  }

  a = fabs(rtb_Sum5);
  if (a > 0.1) {
    if (rtIsNaN(rtb_Sum5)) {
      x1 = (rtNaN);
    } else if (rtb_Sum5 < 0.0) {
      x1 = -1.0;
    } else {
      x1 = (rtb_Sum5 > 0.0);
    }

    rtb_Sum5 = rt_powd_snf(a, 1.5) * x1;
  } else {
    rtb_Sum5 /= 3.1622776601683791;
  }

  /* Sum: '<S1>/Sum6' incorporates:
   *  Constant: '<Root>/Constant9'
   *  Constant: '<Root>/beta1_N2'
   *  Constant: '<Root>/beta2_N2'
   *  DiscreteIntegrator: '<S6>/Integrator2'
   *  MATLAB Function: '<S2>/MATLAB Function'
   *  Product: '<S1>/Divide'
   */
  rtb_Sum4 = (24.0 * rtb_dz1 + 29.0 * rtb_Sum5) - rtDW.Integrator2_DSTATE /
    7.2067;

  /* Sum: '<S3>/Sum9' incorporates:
   *  DiscreteIntegrator: '<S6>/Integrator'
   *  Inport: '<Root>/ref'
   */
  rtb_Sum9 = rtDW.Integrator_DSTATE - rtU.Set_d;

  /* MATLAB Function: '<S6>/MATLAB Function' incorporates:
   *  Constant: '<Root>/Constant10'
   *  Constant: '<Root>/Constant3'
   *  Constant: '<Root>/alpha1_E2'
   *  Constant: '<Root>/alpha2_E2'
   *  Constant: '<Root>/alpha3_E2'
   *  DiscreteIntegrator: '<S6>/Integrator1'
   */
  a = fabs(rtb_Sum9);
  if (a > 0.1) {
    if (rtIsNaN(rtb_Sum9)) {
      x1 = (rtNaN);
    } else if (rtb_Sum9 < 0.0) {
      x1 = -1.0;
    } else {
      x1 = (rtb_Sum9 > 0.0);
    }

    rtb_dz1 = rt_powd_snf(a, 1.0) * x1;
    if (!rtIsNaN(rtb_Sum9)) {
      if (rtb_Sum9 < 0.0) {
        rtb_Sum9 = -1.0;
      } else {
        rtb_Sum9 = (rtb_Sum9 > 0.0);
      }
    }

    rtb_Sum5 = rt_powd_snf(a, 0.75) * rtb_Sum9;
    rtb_Sum9 *= rt_powd_snf(a, 1.5);
  } else {
    rtb_dz1 = rtb_Sum9;
    rtb_Sum5 = rtb_Sum9 / 0.56234132519034907;
    rtb_Sum9 /= 3.1622776601683791;
  }

  rtb_dz1 = rtDW.Integrator1_DSTATE - 15.0296 * rtb_dz1;

  /* Saturate: '<S1>/Saturation' */
  if (rtb_Sum4 > 16383.0) {
    /* Outport: '<Root>/Out1' */
    rtY.Out1 = 16383.0;
  } else if (rtb_Sum4 < -16383.0) {
    /* Outport: '<Root>/Out1' */
    rtY.Out1 = -16383.0;
  } else {
    /* Outport: '<Root>/Out1' */
    rtY.Out1 = rtb_Sum4;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* MATLAB Function: '<S4>/MATLAB Function' incorporates:
   *  Constant: '<Root>/h2'
   *  Constant: '<Root>/r2'
   *  DiscreteIntegrator: '<S4>/Integrator'
   *  DiscreteIntegrator: '<S4>/Integrator2'
   *  Inport: '<Root>/In1'
   */
  x1 = rtDW.Integrator2_DSTATE_o - rtU.Set;
  y = 0.001 * rtDW.Integrator_DSTATE_c + x1;
  a = fabs(y);
  if (a > x1 + 0.0015) {
    if (rtIsNaN(y)) {
      x1 = (rtNaN);
    } else if (y < 0.0) {
      x1 = -1.0;
    } else {
      x1 = (y > 0.0);
    }

    a = (sqrt(12000.0 * a + 2.25) - 1.5) / 2.0 * x1 + rtDW.Integrator_DSTATE_c;
  } else {
    a = y / 0.001 + rtDW.Integrator_DSTATE_c;
  }

  if (fabs(a) > 1.5) {
    if (rtIsNaN(a)) {
      x1 = (rtNaN);
    } else if (a < 0.0) {
      x1 = -1.0;
    } else {
      x1 = (a > 0.0);
    }

    a = -1500.0 * x1;
  } else {
    a = -1500.0 * a / 1.5;
  }

  /* End of MATLAB Function: '<S4>/MATLAB Function' */

  /* Update for DiscreteIntegrator: '<S6>/Integrator1' incorporates:
   *  Constant: '<Root>/Constant1'
   *  Constant: '<Root>/Constant9'
   *  DiscreteIntegrator: '<S6>/Integrator2'
   *  MATLAB Function: '<S6>/MATLAB Function'
   */
  rtDW.Integrator1_DSTATE += ((rtDW.Integrator2_DSTATE - 97.6422 * rtb_Sum5) +
    7.2067 * rtb_Sum4) * 0.001;

  /* Update for DiscreteIntegrator: '<S6>/Integrator2' incorporates:
   *  Constant: '<Root>/Constant2'
   *  MATLAB Function: '<S6>/MATLAB Function'
   */
  rtDW.Integrator2_DSTATE += -124949.0824 * rtb_Sum9 * 0.001;

  /* Update for DiscreteIntegrator: '<S4>/Integrator2' incorporates:
   *  DiscreteIntegrator: '<S4>/Integrator'
   */
  rtDW.Integrator2_DSTATE_o += 0.001 * rtDW.Integrator_DSTATE_c;

  /* Update for DiscreteIntegrator: '<S6>/Integrator' */
  rtDW.Integrator_DSTATE += 0.001 * rtb_dz1;

  /* Update for DiscreteIntegrator: '<S4>/Integrator' */
  rtDW.Integrator_DSTATE_c += 0.001 * a;
}

/* Model initialize function */
void ADRC_TOC_NLADRC_initialize(void)
{
  /* (no initialization code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
