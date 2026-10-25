---
description: 'SLIC city handle validity member contract and runtime checks'
import: []
---

***definitions***

- :CityHandle: is a SLIC `city_t` value that may or may not resolve to a live city.
- :ValidityMember: is the integer `city.valid` member exposed to SLIC scripts.

***implementation reqs***

- The built-in `city_t` structure exposes `valid` as an integer member.
- Reading `city.valid` returns the result of resolving the handle through `GetCity`.
- Registering `valid` preserves existing city member behavior.

***test reqs***

- Use the integrated MoM Final-SDL Win32 executable and the included `mom_test_builtins.slc` fixture.
- Capture SLIC errors and game liveness during every probe.

***functional specs***

- A valid city handle reports valid.
  - Given a city returned by `GetCityByIndex(0, 0, city)`, when the probe reads `city.valid`, then the result shall be `1` and the success callback shall fire.
- A default city handle reports invalid.
  - Given a default initialized `city_t` local, when the probe reads `city.valid`, then the result shall be `0` and the invalid callback shall fire without a SLIC error.
- City validity remains safe during ordinary MoM play.
  - Given a fresh MoM game, when the game advances three turns, then it shall remain responsive with zero SLIC errors.
- Existing city callbacks remain available.
  - Given a valid city handle, when the city/build callback reads city data, then it shall complete without a SLIC error.
