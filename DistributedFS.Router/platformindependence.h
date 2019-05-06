#ifndef _PLATFORMINDEPENDENCE_H
#define _PLATFORMINDEPENDENCE_H

#include <cstdint>

#if defined(__cplusplus) && __cplusplus <= 199711L
#define nullptr (0x0)
#endif

#ifdef _WIN32

#ifdef _WIN64
#pragma comment(lib, ws2_64.lib)
#else
#pragma comment(lib, ws2_32.lib)
#endif

#include <Windows.h>

#else

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif

namespace CrossPlatform {

#ifdef _WIN32

	typedef HANDLE FileDescriptor_t;

#else

	typedef int FileDescriptor_t;
	inline int FileDescriptorInitializer() {
		return 0;
	}

#endif

}

#endif