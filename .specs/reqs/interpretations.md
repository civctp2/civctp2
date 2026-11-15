# Current interpretations

## Preserve an existing lowercase-i assignment

- **Decision:** Add Auto-Explore to an older saved keymap on lowercase `i` only when `i` is unassigned; if the player already bound `i`, preserve that action and leave Auto-Explore available to remap through `K`.
- **Basis:** The keymap maps one action per case-sensitive key. The installed keymap maps uppercase `I` to `INVESTIGATE_CITY` and leaves lowercase `i` free.
- **Decider:** AI, following the explicit no-binding-loss requirement and existing keymap data structure.

## Complete legacy migration after reading the saved keymap

- **Decision:** Reach the fallback binding migration after normal end-of-file, so old `userkeymap.txt` files receive Auto-Explore when lowercase `i` is unassigned.
- **Basis:** The original constructor returned on `fgets` EOF before reaching the new-action migration block; the user's saved keymap lacks `AUTO_EXPLORE` and lowercase `i`.
- **Decider:** AI, based on source inspection and the reported keypress failure.
