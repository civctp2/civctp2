//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Director queue actions
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leak
//
//----------------------------------------------------------------------------
//
/// \file   DirectorActions.cpp
/// \brief  Action types for processing by the director

#include "c3.h"
#include "DirectorActions.h"

#include "EffectActor.h"        // EffectActor destructor

DQActionInvokeResearchAdvance::~DQActionInvokeResearchAdvance()
{
    delete [] message;
}

DQActionMoveProjectile::DQActionMoveProjectile()
:
    pshooting_actor     (NULL),
    ptarget_actor       (NULL),
    pmove_oldPos        (),
    pmove_newPos        (),
    end_projectile      (NULL),
    projectile_path     (0)
{
}

DQActionMoveProjectile::~DQActionMoveProjectile()
{
    // pshooting_actor  : reference only
    // ptarget_actor    : reference only
    delete end_projectile;
}
