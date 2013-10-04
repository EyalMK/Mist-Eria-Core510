# Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  Pandaria/boss_sha_of_anger.cpp
  Pandaria/MoguShanPalace/boss_gekkan.cpp
  Pandaria/MoguShanPalace/boss_xin_the_weaponmaster.cpp
  Pandaria/MoguShanPalace/instance_mogushan_palace.cpp
  Pandaria/MoguShanPalace/mogushan_palace.h
  Pandaria/ShadoPanMonastery/shadopan_monastery.h
  Pandaria/ShadoPanMonastery/instance_shadopan_monastery.cpp
  Pandaria/ShadoPanMonastery/boss_gu_cloudstrike.cpp
  Pandaria/ShadoPanMonastery/boss_master_snowdrift.cpp
  Pandaria/ShadoPanMonastery/boss_sha_of_violence.cpp
  Pandaria/ShadoPanMonastery/boss_taran_zhu.cpp
  Pandaria/StormstoutBrewery/boss_hoptallus.cpp
  Pandaria/StormstoutBrewery/boss_ook_ook.cpp
  Pandaria/StormstoutBrewery/boss_yan_zhu.cpp
  Pandaria/TempleOfTheJadeSerpent/temple_of_the_jade_serpent.h
  Pandaria/TempleOfTheJadeSerpent/instance_temple_of_the_jade_serpent.cpp
  Pandaria/TempleOfTheJadeSerpent/boss_wise_mari.cpp
  Pandaria/TempleOfTheJadeSerpent/boss_lorewalker_stonestep.cpp
  Pandaria/TempleOfTheJadeSerpent/boss_liu_flameheart.cpp
  Pandaria/TempleOfTheJadeSerpent/boss_sha_of_doubt.cpp

)

message("  -> Prepared: Pandaria")
