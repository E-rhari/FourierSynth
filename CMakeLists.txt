# ==============================================================
#   Template para novos plugins

#   1) copie a pasta atual para a pasta do seu plugin

#   2) prepare o build na pasta "build"
#   cmake -B build
#
#   3) compile o projeto na pasta "build"
#   cmake --build build
#
# ==============================================================

# Versao minima de cmake
cmake_minimum_required(VERSION 3.22)

# TODO: Nome e versão do plugin
project(FourierSynth VERSION 0.0.1)

# Caminho de instalacao do JUCE
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Building on Windows")
	list(APPEND CMAKE_PREFIX_PATH "C:/install/JUCE")
else()
    message("Building on Unix-like system")
	list(APPEND CMAKE_PREFIX_PATH "~/JUCE") 
endif()


# Adiciona dependencias do JUCE ao projeto
find_package(JUCE CONFIG REQUIRED)

# Define a configuração do app JUCE como plugin
juce_add_plugin(${PROJECT_NAME}
	# instala o plugin no caminho default da plataforma
	# macos: ~/Library/Audio/Plug-Ins/VST3
	# linux: ~/.vst3
	# windows: C:\Program Files\Common Files\VST3
	COPY_PLUGIN_AFTER_BUILD TRUE

	# build no formato VST3 e aplicativo standalone. 
	# Para Logic/GarageBand adicionar AU ou AUv3. Para ProTools usar AAX
	FORMATS VST3 Standalone

	# Nome amigavel do plugin/nome do executavel
	PRODUCT_NAME ${PROJECT_NAME}

	# Nome do criador do plugin
	COMPANY_NAME "E-Rhari"

	NEEDS_MIDI_INPUT TRUE

	# Outras opcoes validas:
	# PLUGIN_MANUFACTURER_CODE mc0   # AU/GarageBand. 4 caracters, o primeiro maiusculo, o resto minusculo
	# PLUGIN_CODE  Basic              # AU/GarageBand. 4 caracters, o primeiro maiusculo, o resto minusculo
	# ICON_BIG ...                   # ICON_* arguments especifica caminho para icone do app standalone
	# ICON_SMALL ...
	# IS_SYNTH TRUE/FALSE                       # Sintetizador ou efeito?
	# NEEDS_MIDI_INPUT TRUE/FALSE               # Plugin usa MIDI input?
	# NEEDS_MIDI_OUTPUT TRUE/FALSE              # Plugin usa MIDI output?
	# IS_MIDI_EFFECT TRUE/FALSE                 # Efeito MIDI?
	# EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Editor precisa de foco do teclado?
)

# Define o padrão de C++ (17 ou superior)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)  

# Indica quais são os fontes que formam o projeto (arquivos .h são adicionados pelos .cpp, não entram aqui)
target_sources(${PROJECT_NAME}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
		Source/Synth.cpp
)

# Configurações de compilação
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Link do plugin com as bibliotecas do JUCE
target_link_libraries(${PROJECT_NAME}
    PRIVATE
	# Biblioteca essencial do JUCE
	juce::juce_audio_utils

	# Biblioteca com funções de DSP
	# juce::juce_dsp             
    PUBLIC
        # configuracoes de link-time optimization e warnings
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
