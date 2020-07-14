#pragma once
#ifndef S3Includes
#define S3Includes

typedef struct {
	char	*name;
	void	*base;
	int	size;
	int   type;
} GLOBAL_TABLE;

#define DBM_PID_ERROR   -1
#define S3_STRLEN       32

#endif /* S3Includes */