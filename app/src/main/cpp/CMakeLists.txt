# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

add_library(native-activity SHARED
    main.cpp

    gles/GLESEngine.c

    shader/BaseShader.cpp

    texture/TextureUtils.cpp

    view/Triangles.cpp view/Cube.cpp view/Shape.cpp view/ObjModel.cpp

    utils/AndroidAssetUtils.cpp
    utils/ObjHelper.cpp utils/TouchEventHandler.cpp
    utils/ShaderUtils.c utils/CoordinatesUtils.cpp
    utils/cjson/cJSON.c utils/cjson/cJSON_Utils.c)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native-activity PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue)

# libpng static
add_subdirectory(./utils/libpng1_6_29)

target_link_libraries(native-activity
        log native_app_glue android EGL GLESv3 png)