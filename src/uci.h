/*
 * uci.h
 *
 *  Created on: 3 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_UCI_UCI_H_
#define SRC_UCI_UCI_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef UNICODE

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


void main_UCI();
void uciParseCmd(char* str);

#ifdef __cplusplus
}
#endif

#endif /* SRC_UCI_UCI_H_ */
