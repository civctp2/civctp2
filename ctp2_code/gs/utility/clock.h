#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef CLOCK_H
#define CLOCK_H 1


extern void init_clock(); 
extern void update_clock(); 
extern float dt();
extern float get_time();

#endif 
