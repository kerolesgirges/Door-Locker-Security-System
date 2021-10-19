 /******************************************************************************
 *
 * Module: Common - Platform Types Abstraction
 *
 * File Name: stdtypes.h
 *
 * Description: types for AVR
 *
 * Author: keroles girges                                         
 *
 *******************************************************************************/

#ifndef STDTYPES_H_
#define STDTYPES_H_

#ifndef TRUE
#define TRUE                  (1U)
#endif

#ifndef FALSE
#define FALSE                 (0U)
#endif

typedef unsigned char               uint8;          /*           0 .. 255             */
typedef signed char                 sint8;          /*        -128 .. +127            */
typedef unsigned short              uint16;         /*           0 .. 65535           */
typedef signed short                sint16;         /*      -32768 .. +32767          */
typedef unsigned long               uint32;         /*           0 .. 4294967295      */
typedef signed long                 sint32;         /* -2147483648 .. +2147483647     */
typedef unsigned long long          uint64;
typedef signed long long            sint64;
typedef float                       float32;
typedef double                      float64;


#endif /* STDTYPES_H_ */
