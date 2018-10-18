AC_DEFUN([SST_ELEMENT_CONFIG_OUTPUT], [
   SST_ariel_CONFIG([config_ariel=1],[config_ariel=0])
   AS_IF([test $config_ariel -eq 1], [active_element_libraries="$active_element_libraries ariel"])
   AC_CONFIG_FILES([src/sst/elements/ariel/Makefile])
   dist_element_libraries="$dist_element_libraries ariel"
   
   active_element_libraries="$active_element_libraries cacheTracer"
   AC_CONFIG_FILES([src/sst/elements/cacheTracer/Makefile])
   dist_element_libraries="$dist_element_libraries cacheTracer"
   
   active_element_libraries="$active_element_libraries cassini"
   AC_CONFIG_FILES([src/sst/elements/cassini/Makefile])
   dist_element_libraries="$dist_element_libraries cassini"
   
   active_element_libraries="$active_element_libraries CramSim"
   AC_CONFIG_FILES([src/sst/elements/CramSim/Makefile])
   dist_element_libraries="$dist_element_libraries CramSim"
   
   SST_ember_CONFIG([config_ember=1],[config_ember=0])
   AS_IF([test $config_ember -eq 1], [active_element_libraries="$active_element_libraries ember"])
   AC_CONFIG_FILES([src/sst/elements/ember/Makefile])
   dist_element_libraries="$dist_element_libraries ember"
   
   active_element_libraries="$active_element_libraries firefly"
   AC_CONFIG_FILES([src/sst/elements/firefly/Makefile])
   dist_element_libraries="$dist_element_libraries firefly"
   
   active_element_libraries="$active_element_libraries hermes"
   AC_CONFIG_FILES([src/sst/elements/hermes/Makefile])
   dist_element_libraries="$dist_element_libraries hermes"
   
   SST_HMC_CONFIG([config_HMC=1],[config_HMC=0])
   AS_IF([test $config_HMC -eq 1], [active_element_libraries="$active_element_libraries HMC"])
   AC_CONFIG_FILES([src/sst/elements/HMC/Makefile])
   dist_element_libraries="$dist_element_libraries HMC"
   
   active_element_libraries="$active_element_libraries kingsley"
   AC_CONFIG_FILES([src/sst/elements/kingsley/Makefile])
   dist_element_libraries="$dist_element_libraries kingsley"
   
   SST_memHierarchy_CONFIG([config_memHierarchy=1],[config_memHierarchy=0])
   AS_IF([test $config_memHierarchy -eq 1], [active_element_libraries="$active_element_libraries memHierarchy"])
   AC_CONFIG_FILES([src/sst/elements/memHierarchy/Makefile])
   dist_element_libraries="$dist_element_libraries memHierarchy"
   
   active_element_libraries="$active_element_libraries merlin"
   AC_CONFIG_FILES([src/sst/elements/merlin/Makefile])
   dist_element_libraries="$dist_element_libraries merlin"
   
   SST_Messier_CONFIG([config_Messier=1],[config_Messier=0])
   AS_IF([test $config_Messier -eq 1], [active_element_libraries="$active_element_libraries Messier"])
   AC_CONFIG_FILES([src/sst/elements/Messier/Makefile])
   dist_element_libraries="$dist_element_libraries Messier"
   
   SST_miranda_CONFIG([config_miranda=1],[config_miranda=0])
   AS_IF([test $config_miranda -eq 1], [active_element_libraries="$active_element_libraries miranda"])
   AC_CONFIG_FILES([src/sst/elements/miranda/Makefile])
   dist_element_libraries="$dist_element_libraries miranda"
   
   SST_Opal_CONFIG([config_Opal=1],[config_Opal=0])
   AS_IF([test $config_Opal -eq 1], [active_element_libraries="$active_element_libraries Opal"])
   AC_CONFIG_FILES([src/sst/elements/Opal/Makefile])
   dist_element_libraries="$dist_element_libraries Opal"
   
   SST_prospero_CONFIG([config_prospero=1],[config_prospero=0])
   AS_IF([test $config_prospero -eq 1], [active_element_libraries="$active_element_libraries prospero"])
   AC_CONFIG_FILES([src/sst/elements/prospero/Makefile])
   dist_element_libraries="$dist_element_libraries prospero"
   
   SST_Samba_CONFIG([config_Samba=1],[config_Samba=0])
   AS_IF([test $config_Samba -eq 1], [active_element_libraries="$active_element_libraries Samba"])
   AC_CONFIG_FILES([src/sst/elements/Samba/Makefile])
   dist_element_libraries="$dist_element_libraries Samba"
   
   SST_scheduler_CONFIG([config_scheduler=1],[config_scheduler=0])
   AS_IF([test $config_scheduler -eq 1], [active_element_libraries="$active_element_libraries scheduler"])
   AC_CONFIG_FILES([src/sst/elements/scheduler/Makefile])
   dist_element_libraries="$dist_element_libraries scheduler"
   
   active_element_libraries="$active_element_libraries simpleElementExample"
   AC_CONFIG_FILES([src/sst/elements/simpleElementExample/Makefile])
   dist_element_libraries="$dist_element_libraries simpleElementExample"
   
   active_element_libraries="$active_element_libraries simpleSimulation"
   AC_CONFIG_FILES([src/sst/elements/simpleSimulation/Makefile])
   dist_element_libraries="$dist_element_libraries simpleSimulation"
   
   SST_SysC_CONFIG([config_SysC=1],[config_SysC=0])
   AS_IF([test $config_SysC -eq 1], [active_element_libraries="$active_element_libraries SysC"])
   AC_CONFIG_FILES([src/sst/elements/SysC/Makefile])
   dist_element_libraries="$dist_element_libraries SysC"
   
   active_element_libraries="$active_element_libraries thornhill"
   AC_CONFIG_FILES([src/sst/elements/thornhill/Makefile])
   dist_element_libraries="$dist_element_libraries thornhill"
   
   SST_VaultSimC_CONFIG([config_VaultSimC=1],[config_VaultSimC=0])
   AS_IF([test $config_VaultSimC -eq 1], [active_element_libraries="$active_element_libraries VaultSimC"])
   AC_CONFIG_FILES([src/sst/elements/VaultSimC/Makefile])
   dist_element_libraries="$dist_element_libraries VaultSimC"
   
   SST_zodiac_CONFIG([config_zodiac=1],[config_zodiac=0])
   AS_IF([test $config_zodiac -eq 1], [active_element_libraries="$active_element_libraries zodiac"])
   AC_CONFIG_FILES([src/sst/elements/zodiac/Makefile])
   dist_element_libraries="$dist_element_libraries zodiac"
   
   SST_ACTIVE_ELEMENT_LIBRARIES="$active_element_libraries"
   SST_DIST_ELEMENT_LIBRARIES="$dist_element_libraries"
   AC_SUBST(SST_ACTIVE_ELEMENT_LIBRARIES)
   AC_SUBST(SST_DIST_ELEMENT_LIBRARIES)
])
