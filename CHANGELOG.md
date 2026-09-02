# Changelog

The #xx.n tags at the end of each change refer to a private TODO
document.

## [Unreleased]

_Nothing yet._

## [0.2.2] - 2026-09-02

### Changed
- Players have the weapon slot equipped automatically at start of
  round now

### Fixed
- Client never had item instances, so item effects were unpredicted on
  client. A player holding a movement-modifying item desynced every
  tick. Server now sends equipped item id's in the snapshot and the
  client predicts passive item effects (item use() stays
  server-authoritative) #14.2
- gameLevels.h's level list was non-inline and was an ODR violation.
  

## [0.2.1] - 2026-09-02

### Fixed
- GameState player health const incorrectly defaulted to 100 instead
  of 120 #14.5
- Client wasn't reading Snapshot::phase so client-side phase never
  updated after match end #14.1
- Main incorrectly calls network.connectToServer, even though
  StateManager already handles it #14.7
- GameState starts round 1 with 3 dashes instead of the correct 2
  #14.6

### Added
- Server picks a random level from gameLevels.h and sends the path to
  clients in the welcome packet (client now loads the correct level)
  #14.3
