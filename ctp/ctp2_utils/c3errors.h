











#pragma once
#ifndef __C3ERRORS_H__
#define __C3ERRORS_H__





void c3errors_FatalDialog(const char *module, const char *fmt, ...);
void c3errors_FatalDialogFromDB(const char *module, const char *err, ...);
void c3errors_ErrorDialog(const char *module, const char *fmt, ...);
void c3errors_ErrorDialogFromDB(const char *module, const char *err, ...);






#endif
