# PCW Fork Architecture

This branch targets the Amstrad PCW running CP/M Plus. Fork-specific behavior is kept in additive `pcw_*` files so upstream ZMC changes remain straightforward to merge.

## Fork-owned files

- `pcw_platform.c/.h` - physical keyboard input, function-key dispatch, and platform validation
- `pcwkeys.asm` - Amstrad XBIOS keyboard calls
- `pcw_env.asm`, `pcw_sysenv.asm`, `pcw.asm` - embedded 90x32 environment and terminal profile

The `Makefile` replaces only the upstream embedded environment with the PCW environment and adds the PCW keyboard modules. It retains the standard `vlib.asm` adapter and `-lvlib`; VLIB initializes and interprets the embedded PCW terminal profile. Shared `main.c` contains only platform initialization, diagnostics, and key-dispatch hooks.

## Updating from upstream

Keep an upstream remote and merge upstream into the fork branch:

```sh
git remote add upstream <upstream-url>
git fetch upstream
git merge upstream/main
```

Resolve conflicts in the small integration surface (`Makefile`, `main.c`, and occasionally `zmc.h`) while retaining the `pcw_*` modules. Keep the upstream `vlib.asm` adapter and avoid copying PCW hardware logic into shared rendering sources.

For larger upstream updates, first merge them into a clean tracking branch, then merge that branch into the PCW branch. Keep fork infrastructure and PCW platform changes in separate commits from imported upstream changes.
