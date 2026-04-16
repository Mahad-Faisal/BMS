/*
 * File: coder_assumptions.h
 *
 * Abstract: Coder assumptions header file
 */

#ifndef CODER_ASSUMPTIONS_H
#define CODER_ASSUMPTIONS_H

/* include model specific checks */
#include "single_cell_battery_phaseC_backup_ca.h"

/* global results variable mapping for static code */
#define CA_Expected                    CA_single_cell_battery_phaseC_backup_Exp
#define CA_Actual                      CA_single_cell_battery_phaseC_backup_Act
#define CA_TestResults                 CA_single_cell_battery_phaseC_backup_Res
#define CA_PortableWordSizes_Results   CA_single_cell_battery_phaseC_backup_PWSRes

/* entry point function mapping for static code */
#define CA_Run_Tests                   single_cell_battery_phaseC_backup_caRunTests
#define CA_MODEL_SPECIFIC_CHECKS_ENABLED 0
#endif                                 /* CODER_ASSUMPTIONS_H */
