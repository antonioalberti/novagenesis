#ifndef _NGRUNTIMEPROFILE_H
#define _NGRUNTIMEPROFILE_H

#include <cstdlib>
#include <cstring>

// Runtime policy for the staged deprecation of standalone services.
// Normal mode is the default; legacy mode must be explicitly requested.
inline bool NGUseLegacyStandaloneRuntime()
{
  const char* Profile = std::getenv("NG_RUNTIME_PROFILE");

  return Profile != nullptr &&
         (std::strcmp(Profile, "legacy") == 0 ||
          std::strcmp(Profile, "LEGACY") == 0);
}

#endif
