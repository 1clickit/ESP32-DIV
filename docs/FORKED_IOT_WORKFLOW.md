# Forked IoT Firmware Workflow

This lightweight policy applies when adapting a mature third-party IoT firmware project. It is intentionally narrower than the governance used for internally developed production projects.

## Preserve upstream

- Treat the original project as authoritative during initial bring-up.
- Keep the fork's `main` suitable for synchronization with upstream; perform custom work on dedicated branches.
- Avoid unrelated refactoring, cleanup, dependency upgrades, and behavioral changes while validating a baseline.
- Preserve easy comparison with upstream and a straightforward return to a known-good state.

## Validate hardware in strict order

1. Identify the exact hardware.
2. Inspect upstream documentation.
3. Select an upstream-supported hardware target.
4. Reproduce the documented or pinned toolchain.
5. Build without flashing.
6. Identify the intended serial/USB device.
7. Review the exact flash operation.
8. Flash only after explicit authorization.
9. Verify write, hash, and reset results.
10. Establish the resulting configuration as the known-good baseline.

Stop rather than guess when repository state, documentation, hardware identity, artifacts, or flash parameters conflict.

## Handle dependencies conservatively

- Prefer versions required by, or demonstrably compatible with, the mature upstream project.
- Do not modernize old dependencies merely because newer versions exist.
- Record compatibility pins discovered during bring-up.
- Record material workstation changes outside the repository when they affect reproducibility.

## Codex operating policy

During baseline establishment:

- Keep scope narrow and make no unrelated source edits.
- Require explicit authorization before hardware writes.
- Do not publish, push, or open pull requests automatically.
- Preserve concise handoffs covering material actions, deviations, paths, artifacts, and final Git state.

After a known-good baseline exists, experimentation may become progressively less restrictive. Put custom features on dedicated development branches and retain a simple route back to the baseline. Apply restrictions in proportion to project and hardware risk rather than copying heavyweight production controls mechanically.
