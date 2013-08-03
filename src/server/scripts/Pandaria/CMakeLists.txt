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

  Pandaria/GateoftheSettingSun/boss_commander_rimok.cpp
  Pandaria/GateoftheSettingSun/boss_raigonn.cpp
  Pandaria/GateoftheSettingSun/boss_saboteur_kiptilak.cpp
  Pandaria/GateoftheSettingSun/boss_striker_gadok.cpp
  Pandaria/GateoftheSettingSun/gate_of_the_setting_sun.h
     Pandaria/GateoftheSettingSun/instance_gate_of_the_setting_sun.cpp

  Pandaria/HeartofFear/boss_ambershaper_unsok.cpp
  Pandaria/HeartofFear/boss_blade_lord_tayak.cpp
  Pandaria/HeartofFear/boss_garalon.cpp
  Pandaria/HeartofFear/boss_grand_empress_shekzeer.cpp
  Pandaria/HeartofFear/boss_imperial_vizier_zorlok.cpp
  Pandaria/HeartofFear/boss_wind_lord_meljarak.cpp
  Pandaria/HeartofFear/heart_of_fear.h
  Pandaria/HeartofFear/instance_heart_of_fear.cpp

  Pandaria/Mogu'shanPalace/boss_gekkan.cpp
  Pandaria/Mogu'shanPalace/boss_trial_of_the_king.cpp
  Pandaria/Mogu'shanPalace/boss_xin_the_weaponmaster.cpp
  Pandaria/Mogu'shanPalace/instance_mogushan_palace.cpp
  Pandaria/Mogu'shanPalace/mogushan_palace.h

  Pandaria/Mogu'shanVaults/boss_elegon.cpp
  Pandaria/Mogu'shanVaults/boss_feng_the_accursed.cpp
  Pandaria/Mogu'shanVaults/boss_garajal_the_spiritbinder.cpp
  Pandaria/Mogu'shanVaults/boss_the_spirit_kings.cpp
  Pandaria/Mogu'shanVaults/boss_the_stone_guard.cpp
  Pandaria/Mogu'shanVaults/boss_will_of_the_emperor.cpp
  Pandaria/Mogu'shanVaults/instance_mogushan_vaults.cpp
  Pandaria/Mogu'shanVaults/mogushan_vaults.h

  Pandaria/ScarletHalls/boss_armsmaster_harlan.cpp
  Pandaria/ScarletHalls/boss_flameweaver_koegler.cpp
  Pandaria/ScarletHalls/boss_houndmaster_braun.cpp
  Pandaria/ScarletHalls/instance_scarlet_halls.cpp
  Pandaria/ScarletHalls/scarlet_halls.h

  Pandaria/ScarletMonastery/boss_brother_korloff.cpp
  Pandaria/ScarletMonastery/boss_high_inquisitor_whitemane.cpp
  Pandaria/ScarletMonastery/boss_thalnos_the_souldrender.cpp
  Pandaria/ScarletMonastery/instance_scarlet_monastery.cpp
  Pandaria/ScarletMonastery/scarlet_monastery.h

  Pandaria/Scholomance/boss_darkmaster_gandling.cpp
  Pandaria/Scholomance/boss_instructor_chillheart.cpp
  Pandaria/Scholomance/boss_jandice_barov.cpp
  Pandaria/Scholomance/boss_lilian_voss.cpp
  Pandaria/Scholomance/boss_rattlegore.cpp
  Pandaria/Scholomance/instance_scholomance.cpp
  Pandaria/Scholomance/scholomance.h

  Pandaria/ShadoPanMonastery/boss_gu_cloudstrike.cpp
  Pandaria/ShadoPanMonastery/boss_master_snowdrift.cpp
  Pandaria/ShadoPanMonastery/boss_sha_of_violence.cpp
  Pandaria/ShadoPanMonastery/boss_taran_zhu.cpp
  Pandaria/ShadoPanMonastery/instance_shadopan_monastery.cpp
  Pandaria/ShadoPanMonastery/shadopan_monastery.h

  Pandaria/SiegeofNiuzaoTemple/boss_commander_vojak.cpp
  Pandaria/SiegeofNiuzaoTemple/boss_general_pavalak.cpp
  Pandaria/SiegeofNiuzaoTemple/boss_vizier_jinbak.cpp
  Pandaria/SiegeofNiuzaoTemple/boss_wing_leader_neronok.cpp
  Pandaria/SiegeofNiuzaoTemple/instance_siege_of_niuzao_temple.cpp
  Pandaria/SiegeofNiuzaoTemple/siege_of_niuzao_temple.h

  Pandaria/StormstoutBrewery/boss_hoptallus.cpp
  Pandaria/StormstoutBrewery/boss_ookook.cpp
  Pandaria/StormstoutBrewery/boss_yanzhu_the_uncasked.cpp
  Pandaria/StormstoutBrewery/instance_stormstout_brewery.cpp
  Pandaria/StormstoutBrewery/stormstout_brewery.h

  Pandaria/TempleoftheJadeSerpent/boss_liu_flameheart.cpp
  Pandaria/TempleoftheJadeSerpent/boss_lorewalker_stonestep.cpp
  Pandaria/TempleoftheJadeSerpent/boss_sha_of_doubt.cpp
  Pandaria/TempleoftheJadeSerpent/boss_wise_mari.cpp
  Pandaria/TempleoftheJadeSerpent/instance_temple_of_the_jade_serpent.cpp
  Pandaria/TempleoftheJadeSerpent/temple_of_the_jade_serpent.h

  Pandaria/TerraceofEndlessSpring/boss_lei_shi.cpp
  Pandaria/TerraceofEndlessSpring/boss_protectors_of_the_endless.cpp
  Pandaria/TerraceofEndlessSpring/boss_sha_of_fear.cpp
  Pandaria/TerraceofEndlessSpring/boss_tsulong.cpp
  Pandaria/TerraceofEndlessSpring/instance_terrace_of_the_endless_spring.cpp
  Pandaria/TerraceofEndlessSpring/terrace_of_the_endless_spring.h

  )



message("  -> Prepared: Pandaria")
