# SLIC city validity member

## Requirement

The built-in `city_t` structure shall expose `valid` as an integer member.

## Contract

- Reading `city.valid` shall return `1` when the city handle resolves through `GetCity`.
- Reading `city.valid` shall return `0` when the handle does not resolve.
- Reading `city.valid` shall itself succeed for either handle state.
- Registering this member shall not change existing city member behavior.

## Validation

- Build the Windows `Final-SDL|Win32` executable.
- Load the MoM scenario and advance three turns through the UI harness.
- Require zero SLIC errors and a live, responsive game through turn three.
