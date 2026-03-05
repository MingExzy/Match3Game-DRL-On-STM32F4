/**
  ******************************************************************************
  * @file    cnndqn_data_params.h
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-02-20T17:10:56+0800
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef CNNDQN_DATA_PARAMS_H
#define CNNDQN_DATA_PARAMS_H

#include "ai_platform.h"

/*
#define AI_CNNDQN_DATA_WEIGHTS_PARAMS \
  (AI_HANDLE_PTR(&ai_cnndqn_data_weights_params[1]))
*/

#define AI_CNNDQN_DATA_CONFIG               (NULL)


#define AI_CNNDQN_DATA_ACTIVATIONS_SIZES \
  { 8192, }
#define AI_CNNDQN_DATA_ACTIVATIONS_SIZE     (8192)
#define AI_CNNDQN_DATA_ACTIVATIONS_COUNT    (1)
#define AI_CNNDQN_DATA_ACTIVATION_1_SIZE    (8192)



#define AI_CNNDQN_DATA_WEIGHTS_SIZES \
  { 366432, }
#define AI_CNNDQN_DATA_WEIGHTS_SIZE         (366432)
#define AI_CNNDQN_DATA_WEIGHTS_COUNT        (1)
#define AI_CNNDQN_DATA_WEIGHT_1_SIZE        (366432)



#define AI_CNNDQN_DATA_ACTIVATIONS_TABLE_GET() \
  (&g_cnndqn_activations_table[1])

extern ai_handle g_cnndqn_activations_table[1 + 2];



#define AI_CNNDQN_DATA_WEIGHTS_TABLE_GET() \
  (&g_cnndqn_weights_table[1])

extern ai_handle g_cnndqn_weights_table[1 + 2];


#endif    /* CNNDQN_DATA_PARAMS_H */
