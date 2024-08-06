// RRTW
//Realtime ray - tracer, maded as experiment / learning project.
//@2024 (IHarzI)Maslianka Zakhar
//Basic logic is from Ray Tracing books.
//For now, ray - tracer is multithreaded, Window native api used as output Window, with possible custom output to PPm image.
//WIP.
#pragma once

#include <utility>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <stdlib.h>

#include "RTW_Types.h"
#include "RTW_Defines.h"