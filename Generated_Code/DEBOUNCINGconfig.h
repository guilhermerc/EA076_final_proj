/**
 * \file
 * \brief Configuration header file for Wait
 *
 * This header file is used to configure settings of the busy waiting module.
 */

#ifndef __DEBOUNCING_CONFIG_H
#define __DEBOUNCING_CONFIG_H

#include "MCUC1.h" /* include library configuration */

#ifndef DEBOUNCING_CONFIG_USE_CYCLE_COUNTER
  #define DEBOUNCING_CONFIG_USE_CYCLE_COUNTER  (0 && (MCUC1_CONFIG_CORTEX_M>=3))
    /*!< 1: Use hardware cycle counter (if present, only on Cortex-M3 or higher), 0: not using hardware cycle counter */
#endif

#ifndef DEBOUNCING_CONFIG_USE_RTOS_WAIT
  #define DEBOUNCING_CONFIG_USE_RTOS_WAIT  (0 && MCUC1_CONFIG_SDK_USE_FREERTOS)
    /*!< 1: Use RTOS wait if RTOS is present; 0: use normal busy waiting */
#endif

#if DEBOUNCING_CONFIG_USE_CYCLE_COUNTER
  #include "KIN1.h" /* include Cortex utility functions */
#endif

#endif /* __DEBOUNCING_CONFIG_H */
