# [Changelog]

The #xx.n tags at the end of each change refer to a private TODO
document.

## [Unreleased]

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
