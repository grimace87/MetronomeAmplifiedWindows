# DirectX Tool Kit for DirectX 11
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.
#
# http://go.microsoft.com/fwlink/?LinkId=248929

# CMake 3.16 required for precompiled headers
cmake_minimum_required (VERSION 3.16)

project (MetronomeAmplifiedWindows LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/CMake")

set(LIBRARY_SOURCES
        Common/DeviceResources.cpp
        Common/DirectXHelper.cpp
        Common/WICTextureLoader.cpp
        Common/Font.cpp
        Content/Components/BaseShader.cpp
        Content/Components/Shaders/AlphaTextureShader.cpp
        Content/Components/Shaders/AlphaTextureTransformShader.cpp
        Content/Components/Shaders/FontShader.cpp
        Content/Components/Shaders/FontTransformShader.cpp
        Content/Components/BaseTexture.cpp
        Content/Components/Textures/FontTexture.cpp
        Content/Components/Textures/IconsTexture.cpp
        Content/Components/Textures/OverlayTexture.cpp
        Content/Components/Textures/SampleImageTexture.cpp
        Content/Components/Textures/WoodTexture.cpp
        Content/Components/BaseVertexBuffer.cpp
        Content/Components/VertexBuffers/BackgroundVertexBuffer.cpp
        Content/Components/VertexBuffers/MainScreenIconsVertexBuffer.cpp
        Content/Components/VertexBuffers/MainScreenIconLabelsVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsDetailsIconsVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsDetailsTranslucentOverlayVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsHubLabelsVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsNavigatingImagesVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsNavigatingTextsVertexBuffer.cpp
        Content/Scenes/MainSceneRenderer.cpp
        Content/ShaderCache.cpp
        Content/TextureCache.cpp
        Content/VertexBufferCache.cpp
        App.cpp
        MetronomeAmplifiedWindowsMain.cpp
        Content/Components/VertexBuffers/MainScreenTranslucentOverlayVertexBuffer.cpp
        Content/Components/VertexBuffers/MainScreenIconsVertexBuffer.cpp
        Content/Scenes/SettingsNavigationScene.cpp
        Content/Components/Textures/SampleImageTexture.cpp
        Content/Components/VertexBuffers/SettingsDetailsTranslucentOverlayVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsDetailsIconsVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsNavigatingTextsVertexBuffer.cpp
        Content/Components/VertexBuffers/SettingsNavigatingImagesVertexBuffer.cpp)

set(SHADER_SOURCES
        Content/AlphaTextureVertexShader.hlsl
        Content/AlphaTexturePixelShader.hlsl
        Content/AlphaTextureTransformVertexShader.hlsl
        Content/AlphaTextureTransformPixelShader.hlsl
        Content/FontVertexShader.hlsl
        Content/FontPixelShader.hlsl
        Content/FontTransformVertexShader.hlsl
        Content/FontTransformPixelShader.hlsl)

include_directories(.)

add_library(${PROJECT_NAME} STATIC
        ${LIBRARY_SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

# Many of the things below are MSVC-specific or WIN32-specific
target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast)

if(ENABLE_CODE_ANALYSIS)
    target_compile_options(${PROJECT_NAME} PRIVATE /analyze)
endif()

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor /wd5104 /wd5105)
endif()

if (${CMAKE_SIZEOF_VOID_P} EQUAL "4")
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:SSE2)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE /permissive- /JMC- /Zc:__cplusplus)
target_compile_definitions(${PROJECT_NAME} PRIVATE _UNICODE UNICODE)
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)
