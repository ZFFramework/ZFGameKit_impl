
function(zfprojConfigBefore_ZFGameKit_impl projName ZF_SRC_FILES)
    zfprojExec("${ZF_ROOT_PATH}/tools/common/zfsh" "${ZF_ROOT_PATH}/ZFModule/ZFGameKit_impl/zfproj/cmake/ZFGameKit_impl/zf3rd_setup_SDL_GameControllerDB.zfsh")

    set(ZFSDLGAMEPADDB_PATH "${ZF_ROOT_PATH}/ZFModule/ZFGameKit_impl/zf3rd/_repo/SDL_GameControllerDB/gamecontrollerdb.txt")
    if(${projName} STREQUAL "ZFGameKit_impl")
        install(
            FILES "${ZFSDLGAMEPADDB_PATH}"
            DESTINATION "${ZF_ROOT_PATH}/_release/cmake/module/ZFGameKit_impl/zfres/ZFGameKit_impl/sys_SDL/gamepad"
            )
    else()
        install(
            FILES "${ZFSDLGAMEPADDB_PATH}"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/zfres/ZFGameKit_impl/sys_SDL/gamepad"
            )
    endif()
endfunction()

