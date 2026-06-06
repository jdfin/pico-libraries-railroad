#pragma once

#include <cstdint>

struct Loco;

namespace DesktopOps {

constexpr int stop = 0;
extern int creep_mms;
extern int slow_mms;
extern int medium_mms;
extern int fast_mms;
extern int zippy_mms;

void set_loop(void (*loop)(int32_t us));

bool fetch(int loco_id, const Loco *loco, int spur_num);
void uncouple(int loco_id, const Loco *loco);
bool spot(int loco_id, const Loco *loco, int spur_num);
void home(int loco_id, const Loco *loco);

}; // namespace DesktopOps
