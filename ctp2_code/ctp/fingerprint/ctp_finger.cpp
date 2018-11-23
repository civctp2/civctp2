#include "c3.h"

#include "CivPaths.h"
#include "verify.h"
#include "ctp_finger.h"

extern CivPaths		*g_civPaths;

BOOL ctpfinger_Check(void)
{
	BOOL	success = FALSE;
	MBCHAR	fingerprintPath[_MAX_PATH];
	MBCHAR	userListPath[_MAX_PATH];

	if (!g_civPaths->FindFile(k_FINGERPRINT_ASSET_DIR, k_FINGERPRINT_ASSET, fingerprintPath))
		return FALSE;

	if (!g_civPaths->FindFile(k_USER_LIST_ASSET_DIR, k_USER_LIST_ASSET, userListPath))
		return FALSE;

	if (GetInfoFromFingerprint(fingerprintPath) && IsValidUser(userListPath))
		success = TRUE;

	return success;
}
