#ifndef _PLATFORMINDEPENDENCE_H
#define _PLATFORMINDEPENDENCE_H

#include <cstdint>

#if defined(__cplusplus) && __cplusplus <= 199711L
#define nullptr (0x0)
#endif

#ifdef _WIN32

#else

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

#include <Windows.h>

#else

	typedef int FileDescriptor_t;
	inline int FileDescriptorInitializer() {
		return 0;
	}

#endif

}

#endif