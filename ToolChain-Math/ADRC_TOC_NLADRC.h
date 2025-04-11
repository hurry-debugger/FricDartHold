/*
 * File: ADRC_TOC_NLADRC.h
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

#ifndef ADRC_TOC_NLADRC_h_
#define ADRC_TOC_NLADRC_h_
#ifndef ADRC_TOC_NLADRC_COMMON_INCLUDES_
#define ADRC_TOC_NLADRC_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* ADRC_TOC_NLADRC_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T Integrator1_DSTATE;           /* '<S6>/Integrator1' */
  real_T Integrator2_DSTATE;           /* '<S6>/Integrator2' */
  real_T Integrator2_DSTATE_o;         /* '<S4>/Integrator2' */
  real_T Integrator_DSTATE;            /* '<S6>/Integrator' */
  real_T Integrator_DSTATE_c;          /* '<S4>/Integrator' */
} DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Set;                          /* '<Root>/In1' */
  real_T Set_d;                        /* '<Root>/ref' */
} ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Out1;                         /* '<Root>/Out1' */
} ExtY;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;
};

/* Block signals and states (default storage) */
extern DW rtDW;

/* External inputs (root inport signals with default storage) */
extern ExtU rtU;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY rtY;

/* Model entry point functions */
extern void ADRC_TOC_NLADRC_initialize(void);
extern void ADRC_TOC_NLADRC_step(void);

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'ADRC_TOC_NLADRC'
 * '<S1>'   : 'ADRC_TOC_NLADRC/NLADRC3'
 * '<S2>'   : 'ADRC_TOC_NLADRC/NLADRC3/NLSELF yaw'
 * '<S3>'   : 'ADRC_TOC_NLADRC/NLADRC3/Subsystem'
 * '<S4>'   : 'ADRC_TOC_NLADRC/NLADRC3/TD continuous yaw'
 * '<S5>'   : 'ADRC_TOC_NLADRC/NLADRC3/NLSELF yaw/MATLAB Function'
 * '<S6>'   : 'ADRC_TOC_NLADRC/NLADRC3/Subsystem/ESO nonlinear continuous yaw'
 * '<S7>'   : 'ADRC_TOC_NLADRC/NLADRC3/Subsystem/ESO nonlinear continuous yaw/MATLAB Function'
 * '<S8>'   : 'ADRC_TOC_NLADRC/NLADRC3/TD continuous yaw/MATLAB Function'
 */
#endif                                 /* ADRC_TOC_NLADRC_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
