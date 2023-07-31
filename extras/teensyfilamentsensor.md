---
layout: plugin

id: teensyfilamentsensor
title: OctoPrint-Teensyfilamentsensor
description: A Teensy based filament sensor using Bigtreetech's 'Smart Filament Sensor'. Halts the print in case of filement runout or jam.
authors:
- Johan Kanflo
license: AGPLv3

# TODO
date: 2023-07-31

homepage: https://github.com/kanflo/OctoPrint-Teensyfilamentsensor
source: https://github.com/kanflo/OctoPrint-Teensyfilamentsensor
archive: https://github.com/kanflo/OctoPrint-Teensyfilamentsensor/archive/master.zip

# TODO
# Set this to true if your plugin uses the dependency_links setup parameter to include
# library versions not yet published on PyPi. SHOULD ONLY BE USED IF THERE IS NO OTHER OPTION!
#follow_dependency_links: false

# TODO
tags:
- filement
- sensor

# TODO
# When registering a plugin on plugins.octoprint.org, all screenshots should be uploaded not linked from external sites.
screenshots:
- url: extras/octoprint-status.png
  alt: OctoPrint Filement Status
  caption: OctoPrint Filement Status

featuredimage: /extras/smart-filament-sensor.png

# TODO
# You only need the following if your plugin requires specific OctoPrint versions or
# specific operating systems to function - you can safely remove the whole
# "compatibility" block if this is not the case.

compatibility:

  # List of compatible versions
  #
  # A single version number will be interpretated as a minimum version requirement,
  # e.g. "1.3.1" will show the plugin as compatible to OctoPrint versions 1.3.1 and up.
  # More sophisticated version requirements can be modelled too by using PEP440
  # compatible version specifiers.
  #
  # You can also remove the whole "octoprint" block. Removing it will default to all
  # OctoPrint versions being supported.

  octoprint:
  - 1.4.0

  # List of compatible operating systems
  #
  # Valid values:
  #
  # - windows
  # - linux
  # - macos
  # - freebsd
  #
  # There are also two OS groups defined that get expanded on usage:
  #
  # - posix: linux, macos and freebsd
  # - nix: linux and freebsd
  #
  # You can also remove the whole "os" block. Removing it will default to all
  # operating systems being supported.

  os:
  - linux
  - windows
  - macos
  - freebsd

  # Compatible Python version
  #
  # It is recommended to only support Python 3 for new plugins, in which case this should be ">=3,<4"
  #
  # Plugins that wish to support both Python 2 and 3 should set it to ">=2.7,<4".
  #
  # Plugins that only support Python 2 will not be accepted into the plugin repository.

  python: ">=3,<4"

---

This is a plugin that talks to a microcontroller interfacing a "Smart Filament Sensor" from Bigtreetech. Currently a Teensy 2.0 is used but any microcontroller will do. The result is a filement sensor that works on any 3D printer.
