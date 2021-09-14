#include "logger.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_

enum _ENGINE_ERROR
{
	ENGINE_OK = 1,
	ENGINE_ERROR_INIT_FAILED = 2,
	ENGINE_ERROR_OUT_OF_MEMORY = 3,
	ENGINE_ERROR_INCOMPATIBLE_DRIVER = 4
};
typedef enum _ENGINE_ERROR ENGINE_ERROR;

#define ENGINE_ASSERT(_expr) if (!(_expr)) \
	{ LOG_FATAL("Engine assert in %s()", __func__); abort(); }

#define ENGINE_RETURN_IF_ERROR(_err) if (_err != ENGINE_OK) { return _err; }

#define ENGINE_GOTO_IF_ERROR(_err, _goto) if (_err != ENGINE_OK) { goto _goto; }

#define ENGINE_LOG_RETURN_IF_ERROR(_err, _msg) if (_err != ENGINE_OK) \
	{ \
		LOG_ERROR("%s %s()", _msg); \
		return _err; \
	}

#define ENGINE_LOG_GOTO_IF_ERROR(_err, _msg, _goto) if (_err != ENGINE_OK) \
	{ \
		LOG_ERROR("%s", _msg); \
		goto _goto; \
	}

#endif /* _DEBUG_H_ */