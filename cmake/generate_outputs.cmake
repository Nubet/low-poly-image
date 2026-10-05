if(NOT DEFINED LOWPOLY_EXECUTABLE OR NOT DEFINED INPUT_DIRECTORY OR
   NOT DEFINED OUTPUT_DIRECTORY)
    message(FATAL_ERROR "LOWPOLY_EXECUTABLE, INPUT_DIRECTORY and OUTPUT_DIRECTORY are required")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")
file(GLOB INPUT_FILES
     LIST_DIRECTORIES false
     "${INPUT_DIRECTORY}/*.jpg"
     "${INPUT_DIRECTORY}/*.jpeg"
     "${INPUT_DIRECTORY}/*.png")
list(SORT INPUT_FILES)

if(NOT INPUT_FILES)
    message(FATAL_ERROR "No supported images found in ${INPUT_DIRECTORY}")
endif()

foreach(INPUT_FILE IN LISTS INPUT_FILES)
    get_filename_component(FILE_STEM "${INPUT_FILE}" NAME_WE)
    set(OUTPUT_FILE "${OUTPUT_DIRECTORY}/${FILE_STEM}.png")

    message(STATUS "Generating ${OUTPUT_FILE}")
    execute_process(
        COMMAND "${LOWPOLY_EXECUTABLE}" "${INPUT_FILE}"
                --output "${OUTPUT_FILE}"
                --points "${POINTS}"
                --seed "${SEED}"
        RESULT_VARIABLE RESULT
    )

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "lowpoly failed for ${INPUT_FILE} with code ${RESULT}")
    endif()
endforeach()
