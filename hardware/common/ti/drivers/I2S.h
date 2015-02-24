/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       I2S.h
 *
 *  @brief      I2S driver interface
 *
 *  The I2S header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/I2S.h>
 *  @endcode
 *
 *  # Operation #
 *  The I2S driver simplifies reading and writing to any of the McASP
 *  peripherals on the board with Receive and Transmit support.
 *  These include blocking, non-blocking, read and write characters on the
 *  McASP peripheral. The board's I2S peripheral and pins must
 *  be configured before initializing the I2S driver. The application
 *  initializes the I2S driver by calling I2S_init() and is then ready to
 *  open a I2S by calling I2S_open() and passing in a I2S parameters data
 *  structure.
 *
 *  The APIs in this driver serve as an interface to a typical OSAL
 *  application. The specific peripheral implementations are responsible to
 *  create all the OSAL specific primitives to allow for thread-safe
 *  operation.
 *
 *  ## Opening the driver #
 *
 *  @code
 *  I2S_Handle      handle;
 *  I2S_Params      params;
 *  I2SCC3200DMA_SerialPinParams customParams;
 *
 *  I2S_Params_init(&params);
 *  params.operationMode                     = I2S_MODE_TX_RX_SYNC;
 *  < Change other params as required >
 *
 *  I2SCC3200DMA_Params_init(&customParams);
 *  < Change custom params as required >
 *  params->customParams = (I2SCC3200DMA_SerialPinParams *)&customParams;
 *
 *  handle = I2S_open(someI2S_configIndexValue, &params);
 *  if (!handle) {
 *      System_printf("I2S did not open");
 *  }
 *  @endcode
 *
 *  ## Writing data #
 *
 *  @code
 *  const unsigned char hello[] = "Hello World\n";
 *  const unsigned char hello1[] = "Hello World1\n";
 *  I2S_BufDesc writeBuffer1;
 *  I2S_BufDesc writeBuffer2;
 *  I2S_BufDesc *pDesc = NULL;
 *
 *  writeBuffer1.bufPtr  = &hello;
 *  writeBuffer1.bufSize = sizeof(hello);
 *  writeBuffer2.bufPtr  = &hello1;
 *  writeBuffer2.bufSize = sizeof(hello1);
 *
 *  ret = I2S_write(handle, &writeBuffer1);
 *  ret = I2S_write(handle, &writeBuffer2);
 *
 *  while(1)
 *  {
 *      ret = I2S_writeReclaim(handle, &pDesc);
 *      System_printf("The I2S wrote %d bytes\n", ret);
 *      pDesc->bufPtr  = &hello;;
 *      pDesc->bufSize = sizeof(hello);
 *      ret = I2S_write(handle, pDesc);
 *  }
 *
 *  @endcode
 *
 *  ## Reading data #
 *
 *  @code
 *  unsigned char rxBuffer[20];
 *  unsigned char rxBuffer1[20];
 *  I2S_BufDesc readBuffer1;
 *  I2S_BufDesc readBuffer2;
 *  I2S_BufDesc *pDesc = NULL;
 *
 *  readBuffer1.bufPtr = &rxBuffer;
 *  readBuffer1.bufSize = 20;
 *  readBuffer2.bufPtr = &rxBuffer1;
 *  readBuffer2.bufSize = 20;
 *
 *  ret = I2S_read(handle, &readBuffer1);
 *  ret = I2S_read(handle, &readBuffer2);
 *
 *  while(1)
 *  {
 *      ret = I2S_readReclaim(handle, &pDesc);
 *      System_printf("The I2S read %d bytes\n", ret);
 *      pDesc->bufPtr = &rxBuffer;
 *      pDesc->bufSize = 20;
 *      ret = I2S_read(handle, pDesc);
 *  }
 *  @endcode
 *
 *  # Implementation #
 *
 *  This module serves as the main interface for OSAL
 *  applications. Its purpose is to redirect the module's APIs to specific
 *  peripheral implementations which are specified using a pointer to a
 *  I2S_FxnTable.
 *
 *  The I2S driver interface module is joined (at link time) to a
 *  NULL-terminated array of I2S_Config data structures named *I2S_config*.
 *  *I2S_config* is implemented in the application with each entry being an
 *  instance of a I2S peripheral. Each entry in *I2S_config* contains a:
 *  - (I2S_FxnTable *) to a set of functions that implement a I2S peripheral
 *  - (void *) data object that is associated with the I2S_FxnTable
 *  - (void *) hardware attributes that are associated to the I2S_FxnTable
 *
 *  Currently the following I2S peripheral implementations are supported:
 *  - @ref I2SCC3200DMA.h
 *
 *  ===========================================================================
 */

#ifndef ti_drivers_I2S__include
#define ti_drivers_I2S__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include <ti/drivers/ports/ListP.h>

#define I2S_ERROR  -1

/*!
 *  @brief    Wait forever define
 */
#define I2S_WAIT_FOREVER ~(0)

/*!
 *  @brief      A handle that is returned from a I2S_open() call.
 */
typedef struct I2S_Config      *I2S_Handle;

/*!
 *  @brief I2S buffer descriptor for issue/reclaim mode.
 */
typedef struct I2S_BufDesc {

    /*! Used internally to link descriptors together */
    ListP_Elem              qElem;

    /*! Pointer to the buffer */
    void                   *bufPtr;

    /*! Size of the buffer (target MAUs). */
    size_t                 bufSize;

    /*! Optional argument associated with the descriptor. */
    uintptr_t              arg;
} I2S_BufDesc;

/*!
 *  @brief      The definition of a callback function used by the I2S driver
 *              when used in ::I2S_MODE_CALLBACK
 *
 *  @param      I2S_Handle             I2S_Handle
 *
 *  @param      buf                     Pointer to read/write buffer
 *
 *  @param      count                   Number of elements read/written
 */
typedef void        (*I2S_Callback)    (I2S_Handle, I2S_BufDesc *desc);

/*!
 *  @brief      I2S mode settings
 *
 *  This enum defines the read and write modes for the
 *  configured I2S.
 */
typedef enum I2S_DataMode {
    /*!
     *  Non-blocking and will return immediately.  When the transfer by the intr
     *  is finished the configured callback function is called.
     */
    I2S_MODE_CALLBACK,

    /*!
     *  Use I2S_readIssue, I2S_writeIssue calls to queue buffers to the
     *  I2S.  I2S_readReclaim() blocks until a buffer of data is available.
     *  I2S_writeReclaim() blocks until a buffer of data has been written
     *  and the descriptor can be returned back to the caller.
     */
    I2S_MODE_ISSUERECLAIM
} I2S_DataMode;

/*!
 *  @brief      I2S mode settings
 *
 *  This enumeration defines the mode for I2S operation.
 */
typedef enum I2S_OpMode {
    I2S_OPMODE_TX_ONLY,       /*!< Only Transmit enabled */
    I2S_OPMODE_RX_ONLY,       /*!< Only Receive enabled */
    I2S_OPMODE_TX_RX_SYNC     /*!< Receive and Transmit are enabled in Sync */
} I2S_OpMode;

/*!
 *  @brief    I2S Serializer InActive state settings
 *
 *  This enumeration defines the Serializer configuration
 *  in inactive state.
 */
typedef enum I2S_SerInActiveConfig {
    I2S_SERCONFIG_INACT_TRI_STATE,  /*!< Inactive state to tristate */
    I2S_SERCONFIG_INACT_LOW_LEVEL,  /*!< Inactive state to low */
    I2S_SERCONFIG_INACT_HIGH_LEVEL  /*!< Inactive state to high */
} I2S_SerInActiveConfig;

/*!
 *  @brief    I2S serial pin mode
 *
 *  This enumeration defines the Serial pin configuration
 */
typedef enum I2S_PinMode {
    I2S_PINMODE_RX,                 /*!< Operate the pin in Rx mode */
    I2S_PINMODE_TX,                 /*!< Operate the pin in Tx mode */
    I2S_PINMODE_INACTIVE            /*!< Pin in inactive mode       */
} I2S_PinMode;

/*!
 *  @brief    Basic I2S Parameters
 */
typedef struct I2S_Params {
    /*!< I2S operational mode */
    I2S_OpMode            operationMode;

    /*!< I2S sampling frequency configuration in samples/second */
    uint32_t              samplingFrequency;

    /*!< Slot length */
    unsigned char         slotLength;

    /*!< Bits per sample (Word length) */
    unsigned char         bitsPerSample;

    /*!< Number of channels (slots per frame) */
    unsigned char         numChannels;

    /*!< Mode for all read calls   */
    I2S_DataMode          readMode;

    /*!< Pointer to read callback */
    I2S_Callback          readCallback;

    /*!< Timeout for read semaphore */
    uint32_t              readTimeout;

    /*!< Mode for all write calls   */
    I2S_DataMode          writeMode;

    /*!< Pointer to write callback */
    I2S_Callback          writeCallback;

    /*!< Timeout for write semaphore */
    uint32_t              writeTimeout;

    /*!< Pointer to device specific custom params */
    void *                customParams;
}I2S_Params;

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_CloseFxn().
 */
typedef void        (*I2S_CloseFxn)          (I2S_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_control().
 */
typedef int         (*I2S_ControlFxn)        (I2S_Handle handle,
                                              unsigned int cmd,
                                              void *arg);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_init().
 */
typedef void        (*I2S_InitFxn)           (I2S_Handle handle);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_OpenFxn().
 */
typedef I2S_Handle  (*I2S_OpenFxn)           (I2S_Handle handle,
                                              I2S_Params *params);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_IssueFxn().
 */
typedef int        (*I2S_IssueFxn)      (I2S_Handle handle,I2S_BufDesc *desc);

/*!
 *  @brief      A function pointer to a driver specific implementation of
 *              I2S_ReclaimFxn().
 */
typedef size_t    (*I2S_ReclaimFxn)    (I2S_Handle handle,I2S_BufDesc **desc);

/*!
 *  @brief      The definition of a I2S function table that contains the
 *              required set of functions to control a specific I2S driver
 *              implementation.
 */
typedef struct I2S_FxnTable {
    /*! Function to close the specified peripheral */
    I2S_CloseFxn           closeFxn;

    /*! Function to implementation specific control function */
    I2S_ControlFxn         controlFxn;

    /*! Function to initialize the given data object */
    I2S_InitFxn            initFxn;

    /*! Function to open the specified peripheral */
    I2S_OpenFxn            openFxn;

    /*! Function to queue a buffer for reading from the specified peripheral */
    I2S_IssueFxn           readIssueFxn;

    /*! Function to retrieve a received buffer of data from the specified peripheral */
    I2S_ReclaimFxn         readReclaimFxn;

    /*! Function to queue a buffer for writing from the specified peripheral */
    I2S_IssueFxn           writeIssueFxn;

    /*! Function to retrieve a sent buffer of data from the specified peripheral */
    I2S_ReclaimFxn         writeReclaimFxn;

} I2S_FxnTable;

/*! @brief I2S Global configuration */
typedef struct I2S_Config {
    /*! Pointer to a table of a driver-specific implementation of I2S
        functions */
    I2S_FxnTable const    *fxnTablePtr;

    /*! Pointer to a driver specific data object */
    void                   *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void          const    *hwAttrs;
} I2S_Config;

/*!
 *  @brief  Function to closes a given I2S peripheral specified by the I2S
 *  handle.
 *
 *  @pre    I2S_open() had to be called first.
 *
 *  @param  handle  A I2S_Handle returned from I2S_open
 *
 *  @sa     I2S_open
 */
extern void I2S_close(I2S_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a given
 *          I2S_Handle.
 *
 *  @pre    I2S_open() has to be called first.
 *
 *  @param  handle A I2S handle returned from I2S_open()
 *
 *  @param  cmd    A command value defined by the driver specific implementation
 *
 *  @param  arg    An optional argument that is accompanied with cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @sa     I2S_open()
 */
extern int I2S_control(I2S_Handle handle, unsigned int cmd, void *arg);

/*!
 *  @brief  Function to initializes the I2S module
 *
 *  @pre    The I2S controller needs to be powered up and clocked. The
 *          I2S_config structure must exist and be persistent before this
 *          function can be called. This function must also be called before
 *          any other I2S driver APIs.
 */
extern void I2S_init(void);

/*!
 *  @brief  Function to initialize a given I2S peripheral specified by the
 *          particular index value. The parameter specifies which mode the I2S
 *          will operate.
 *
 *  @pre    I2S controller has been initialized
 *
 *  @param  index         Logical peripheral number indexed into the HWAttrs
 *                        table
 *
 *  @param  params        Pointer to an parameter block, if NULL it will use
 *                        default values
 *
 *  @return A I2S_Handle on success or a NULL on an error or if it has been
 *          already opened
 *
 *  @sa     I2S_close
 */
extern I2S_Handle I2S_open(unsigned int index, I2S_Params *params);

/*!
 *  @brief  Function to initialize the I2S_Params struct to its defaults
 *
 *  Defaults values are:
 *  params.operationMode                     = I2S_MODE_TX_RX_SYNC;
 *  params.samplingFrequency                 = 16000;
 *  params.slotLength                        = 16;
 *  params.bitsPerSample                     = 16;
 *  params.numChannels                       = 2;
 *  params.readMode                          = I2S_MODE_ISSUERECLAIM;
 *  params.readCallback                      = NULL;
 *  params.readTimeout                       = I2S_WAIT_FOREVER;
 *  params.writeMode                         = I2S_MODE_ISSUERECLAIM;
 *  params.writeCallback                     = NULL;
 *  params.writeTimeout                      = I2S_WAIT_FOREVER;
 *  params.customParams                      = NULL;
 *
 *  @param  params  Parameter structure to initialize
 */
extern void I2S_Params_init(I2S_Params *params);

/*!

 *  @brief Function to queue a buffer of data to the I2S in callback mode
 *         for reading.
 *
 *  @param  handle      A I2S_Handle
 *
 *  @param  desc        A pointer to a I2S_BufDesc object.  The bufPtr
 *                      and bufSize fields must be set to a buffer and the
 *                      size of the buffer before passing to this function.
 *  @return             Returns 0 if successful else would return
 *                      I2S_CMD_UNDEFINED on an error.
 */
extern int I2S_read(I2S_Handle handle, I2S_BufDesc *desc);

extern int I2S_readIssue(I2S_Handle handle, I2S_BufDesc *desc);

/*!
 *  @brief Function to retrieve a full buffer of data read by the I2S.
 *
 *  @param  handle      A I2S_Handle
 *
 *  @param  pDesc       A pointer to a I2S_BufDesc pointer.
 *
 *  @return Returns the number of bytes read from the I2S, or I2S_ERROR
 *          on an error.
 */
extern size_t I2S_readReclaim(I2S_Handle handle, I2S_BufDesc **pDesc);

/*!
 *  @brief Function to queue a buffer of data to the I2S in
 *         callback mode for writing.
 *
 *  @param  handle      A I2S_Handle
 *
 *  @param  desc        A pointer to a I2S_BufDesc object.  The bufPtr
 *                      and bufSize fields must be set to a buffer and the
 *                      size of the buffer before passing to this function.
 *  @return             Returns 0 if successful else would return
 *                      I2S_CMD_UNDEFINED on an error.
 */
extern int I2S_write(I2S_Handle handle, I2S_BufDesc *desc);

extern int I2S_writeIssue(I2S_Handle handle, I2S_BufDesc *desc);

/*!
 *  @brief Function to retrieve a buffer that the I2S has finished writing.
 *
 *  @param  handle      A I2S_Handle
 *
 *  @param  pDesc       A pointer to a I2S_BufDesc pointer.
 *
 *  @return Returns the number of bytes that have been written to the I2S,
 *          I2S_ERROR on an error.
 */
extern size_t I2S_writeReclaim(I2S_Handle handle, I2S_BufDesc **pDesc);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_I2S__include */