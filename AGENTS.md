# AGENTS.md — READ FIRST

All Codex/AI work in this repository must read and follow this file before doing anything else.

## Environment classification

This project is developed for a private home-lab / home-network environment. The security target is roughly comparable to a well-run Home Assistant installation: sensible least privilege, protected credentials, no unnecessary Internet exposure, practical backups, and straightforward recovery.

This is **not** an FBI, CIA, NASA, critical-infrastructure, or other high-assurance mission environment. Do not introduce enterprise/government ceremony, redundant controls, or architectural complexity unless a specific risk actually justifies it.

## Operating philosophy — rollback first, then move forward

Use a practical “SpaceX-style” iteration model:

- Establish a usable rollback path before destructive, risky, or availability-affecting changes.
- Once rollback is verified, prefer forward progress and real-world validation over repeated approval checkpoints.
- Fix forward when failure is bounded, understood, recoverable, and rollback remains intact.
- Roll back when continuing risks irreplaceable data, credentials, lockout, significant exposure, or the failure is no longer understood quickly.
- Prefer the simplest native, reversible solution and avoid irrelevant overengineering.

Rollback-first does not mean reckless testing. Preserve irreplaceable data and isolate destructive experiments.

## Preflight and execution

Read this file first, then project-specific startup/state/task docs. Before substantial work, preflight material conflicts, prerequisites, persistence/rollback, resource limits, and test gaps. Raise blocking/material questions before implementation. Once work begins, continue through ordinary recoverable issues until the planned checkpoint. Distinguish tested candidates from actually deployed/running code and verify deployment in the real system.

## Resource safety

Long-running builds/tests/scans/downloads/SSH/subprocesses must use bounded timeouts where practical. Monitor the system actually executing the work. If critical memory/swap/disk pressure, severe sustained I/O pressure, or prolonged `D`-state occurs, stop only the affected task-owned child/process group where possible, preserve work and rollback, and report rather than automatically rebooting/deploying/committing/pushing.

## Infrastructure baseline before changes

Before changing a CT, VM, server, appliance, or important service, verify and record the applicable host/hardware, ID/role, hostname, IP/MAC/gateway/bridge, CPU/RAM/swap/disk, mounts, service ports, users/access, application/service paths, Git state, dependencies, current known-good behavior, and rollback path. Do not infer facts that can be verified.

Update current-state/infrastructure/change notes when operational facts change. Private repos may contain internal topology; public repos must sanitize it.

## Security and secrets

Never commit or publish passwords, private keys, API tokens, cookies/session secrets, recovery codes, VPN private keys/PSKs, or unsanitized configuration backups containing secrets. Do not weaken authentication, firewalling, or isolation merely for convenience unless explicitly justified and reversible.

## Repository and publication policy

Working repositories are private by default during infrastructure-specific development. Public releases should be deliberately sanitized exports with reviewed history rather than casual visibility changes.

## Project-specific rules

This file establishes the common minimum operating philosophy. More specific or stricter project rules **supplement and take precedence over this file** where applicable.
