/*
 * estimator_variant.h
 *
 *  Created on: May 16, 2026
 *      Author: mahad
 */

#ifndef ESTIMATOR_VARIANT_H_
#define ESTIMATOR_VARIANT_H_


#define ESTIMATOR_VARIANT_CC              0
#define ESTIMATOR_VARIANT_BASIC_EKF       1
#define ESTIMATOR_VARIANT_ADAPTIVE_R_EKF  2
#define ESTIMATOR_VARIANT_DAEKF           3
#define ESTIMATOR_VARIANT_UKF             4

/*
 * Select one estimator per build.
 *
 * Current options:
 *   CC             = Coulomb counting + open-loop voltage diagnostic
 *   BASIC_EKF      = 3-state EKF, fixed R0, fixed R
 *   ADAPTIVE_R_EKF = 3-state EKF, fixed R0, adaptive R
 *   DAEKF          = current full ADEKF, adaptive R + R0 adaptation
 */

#endif /* ESTIMATOR_VARIANT_H_ */
