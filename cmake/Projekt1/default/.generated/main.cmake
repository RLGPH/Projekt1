include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Projekt1_default_library_list )

set(OUTPUT_SUFFIX elf)
# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(Projekt1_default_default_XC8_FILE_TYPE_assemble)
add_library(Projekt1_default_default_XC8_assemble OBJECT ${Projekt1_default_default_XC8_FILE_TYPE_assemble})
    Projekt1_default_default_XC8_assemble_rule(Projekt1_default_default_XC8_assemble)
    list(APPEND Projekt1_default_library_list "$<TARGET_OBJECTS:Projekt1_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(Projekt1_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(Projekt1_default_default_XC8_assemblePreprocess OBJECT ${Projekt1_default_default_XC8_FILE_TYPE_assemblePreprocess})
    Projekt1_default_default_XC8_assemblePreprocess_rule(Projekt1_default_default_XC8_assemblePreprocess)
    list(APPEND Projekt1_default_library_list "$<TARGET_OBJECTS:Projekt1_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(Projekt1_default_default_XC8_FILE_TYPE_compile)
add_library(Projekt1_default_default_XC8_compile OBJECT ${Projekt1_default_default_XC8_FILE_TYPE_compile})
    Projekt1_default_default_XC8_compile_rule(Projekt1_default_default_XC8_compile)
    list(APPEND Projekt1_default_library_list "$<TARGET_OBJECTS:Projekt1_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(Projekt1_default_default_XC8_FILE_TYPE_objcopy_avr)
add_library(Projekt1_default_default_XC8_objcopy_avr OBJECT ${Projekt1_default_default_XC8_FILE_TYPE_objcopy_avr})
    Projekt1_default_default_XC8_objcopy_avr_rule(Projekt1_default_default_XC8_objcopy_avr)
    list(APPEND Projekt1_default_library_list "$<TARGET_OBJECTS:Projekt1_default_default_XC8_objcopy_avr>")

endif()


# Main target for this project
add_executable(Projekt1_default_image_nRrZCYK3 ${Projekt1_default_library_list})

set_target_properties(Projekt1_default_image_nRrZCYK3 PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${Projekt1_default_output_dir}")
target_link_libraries(Projekt1_default_image_nRrZCYK3 PRIVATE ${Projekt1_default_default_XC8_FILE_TYPE_link})
# Add the link options from the rule file.
Projekt1_default_link_rule( Projekt1_default_image_nRrZCYK3)


#Add objcopy steps
Projekt1_default_objcopy_avr_rule(Projekt1_default_image_nRrZCYK3)

