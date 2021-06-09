/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HAL_DEF
#define __HAL_DEF

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
    HAL_OK             = 0x00U,
    HAL_INVALIAD_PARAM    = 0x01U,
    HAL_BUSY           = 0x02U,
    HAL_STATE_ERROR    = 0x03U,
    HAL_TIMEOUT  = 0x04U,
    HAL_ERROR    = 0x05U,
    HAL_INVALID_OPERATION = 0x06U,
} HAL_StatusTypeDef;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
    HAL_UNLOCKED = 0x00U,
    HAL_LOCKED = 0x01U
} HAL_LockTypeDef;

typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus,
    ITStatus;

typedef enum
{
    ERROR = 0,
    SUCCESS = !ERROR
} ErrorStatus;    


/* Exported macro ------------------------------------------------------------*/
#define HAL_MAX_DELAY      0xFFFFFFFFU

#define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0U)

#define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0U)

#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#endif /* __GNUC__ */

#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */

#endif    // /* ___LE501X_HAL_DEF */

/****END OF FILE****/
