#ifndef verify_h
#define verify_h

#ifdef __cplusplus
extern "C" {
#endif

BOOL IsValidUser(const char *szListFile);

BOOL GetInfoFromFingerprint(const char *szFingerprintFile);

#ifdef __cplusplus
}
#endif

#endif
