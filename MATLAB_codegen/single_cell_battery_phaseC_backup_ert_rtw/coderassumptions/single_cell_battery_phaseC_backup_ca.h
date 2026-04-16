/*
 * File: single_cell_battery_phaseC_backup_ca.h
 *
 * Abstract: Tests assumptions in the generated code.
 */

#ifndef SINGLE_CELL_BATTERY_PHASEC_BACKUP_CA_H
#define SINGLE_CELL_BATTERY_PHASEC_BACKUP_CA_H

/* preprocessor validation checks */
#include "single_cell_battery_phaseC_backup_ca_preproc.h"
#include "coder_assumptions_hwimpl.h"

/* variables holding test results */
extern CA_ChecksTestResults CA_single_cell_battery_phaseC_backup_Res;
extern CA_PWS_TestResults CA_single_cell_battery_phaseC_backup_PWSRes;

/* variables holding "expected" and "actual" hardware implementation */
extern const CA_Checks CA_single_cell_battery_phaseC_backup_Exp;
extern CA_Checks CA_single_cell_battery_phaseC_backup_Act;
extern const int numberOfImportedTypes;

/* entry point function to run tests */
void single_cell_battery_phaseC_backup_caRunTests(void);

#endif                                 /* SINGLE_CELL_BATTERY_PHASEC_BACKUP_CA_H */
