#ifndef __FARN_COMMON_HAPP
#define __FARN_COMMON_HAPP

#include <utility>
#include <vector>

typedef std::pair<float, float> farn_point_t;

typedef struct {
    float a,b,c,d,e,f,p;
} farn_params_t;

typedef std::vector<farn_params_t> farn_params_list_t;

#endif