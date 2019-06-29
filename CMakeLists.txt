#############################################
#
# CMakeLists.txt for PocketSphinx
#
#############################################

cmake_minimum_required(VERSION 2.6)

project(pocketsphinx.js)

option(HMM_EMBED "Embed the HMM files inside generated JavaScript" ON)
option(WASM "Build to WebAssembly" ON)

SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

# CMakeLists.txt should be alongside pocketsphinx and
# sphinxbase folders
set(sphinx_root ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT EXISTS ${sphinx_root}/sphinxbase OR NOT EXISTS ${sphinx_root}/pocketsphinx)
    message(FATAL_ERROR
      "sphinxbase or pocketsphinx not found in ${sphinx_root}")
endif()

# Sphinxbase folders
set(fe_folder "${sphinx_root}/sphinxbase/src/libsphinxbase/fe")
set(feat_folder "${sphinx_root}/sphinxbase/src/libsphinxbase/feat")
set(lm_folder "${sphinx_root}/sphinxbase/src/libsphinxbase/lm")
set(util_folder "${sphinx_root}/sphinxbase/src/libsphinxbase/util")

include_directories(${fe_folder} ${feat_folder} ${lm_folder} ${util_folder})
include_directories("${sphinx_root}/sphinxbase/include")
# Platform-specific config
include_directories("${sphinx_root}/platform/include")

# Sphinxbase source files
set(fe_src_files fe_interface.c fe_sigproc.c fe_warp_affine.c fe_warp.c fe_warp_inverse_linear.c fe_warp_piecewise_linear.c fixlog.c fe_noise.c fe_prespch_buf.c)
set(fe_srcs "")
foreach(cfile ${fe_src_files})
    set(fe_srcs ${fe_srcs} "${fe_folder}/${cfile}")
endforeach(cfile)

set(feat_src_files agc.c cmn.c cmn_live.c feat.c lda.c)
set(feat_srcs "")
foreach(cfile ${feat_src_files})
    set(feat_srcs ${feat_srcs} "${feat_folder}/${cfile}")
endforeach(cfile)

set(util_src_files bio.c bitvec.c case.c ckd_alloc.c cmd_ln.c dtoa.c err.c errno.c f2c_lite.c filename.c genrand.c glist.c hash_table.c heap.c listelem_alloc.c logmath.c matrix.c mmio.c pio.c profile.c sbthread.c strfuncs.c bitarr.c priority_queue.c)
set(util_srcs "")
foreach(cfile ${util_src_files})
    set(util_srcs ${util_srcs} "${util_folder}/${cfile}")
endforeach(cfile)

set(lm_src_files fsg_model.c jsgf.c jsgf_parser.c jsgf_scanner.c ngrams_raw.c lm_trie.c lm_trie_quant.c ngram_model_set.c ngram_model_trie.c ngram_model.c)
set(lm_srcs "")
foreach(cfile ${lm_src_files})
    set(lm_srcs ${lm_srcs} "${lm_folder}/${cfile}")
endforeach(cfile)

# PocketSphinx folders
include_directories("${sphinx_root}/pocketsphinx/src/libpocketsphinx")
include_directories("${sphinx_root}/pocketsphinx/include")

set(pocketsphinx_folder "${sphinx_root}/pocketsphinx/src/libpocketsphinx")

# PocketSphinx source files
set(pocketsphinx_src_files acmod.c bin_mdef.c blkarray_list.c dict.c dict2pid.c fsg_history.c fsg_lextree.c fsg_search.c hmm.c mdef.c ms_gauden.c ms_mgau.c ms_senone.c ngram_search.c ngram_search_fwdtree.c ngram_search_fwdflat.c phone_loop_search.c pocketsphinx.c ps_lattice.c ps_mllr.c ptm_mgau.c s2_semi_mgau.c tmat.c vector.c kws_search.c kws_detections.c allphone_search.c)
set(pocketsphinx_srcs "")
foreach(cfile ${pocketsphinx_src_files})
    set(pocketsphinx_srcs ${pocketsphinx_srcs} "${pocketsphinx_folder}/${cfile}")
endforeach(cfile)

add_definitions(-DHAVE_CONFIG_H)

set(ps_lib_js "pocketsphinx.js")
set(ps_lib "pocketsphinx")

# We are using the C++ binding utility of emscripten, this needs to be added to the compilation command
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Oz -DMODELDIR=\"\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Oz --bind")

# Add include dir in build tree as we'll place config header files there
include_directories("${CMAKE_BINARY_DIR}/include")

# Building a shared library to be converted to JavaScript
add_library(${ps_lib} SHARED "src/psRecognizer.cpp" ${pocketsphinx_srcs}  ${fe_srcs} ${feat_srcs} ${lm_srcs} ${util_srcs})

if(HMM_EMBED)
  if ((NOT HMM_BASE) OR (NOT HMM_FOLDERS))
    set(HMM_BASE "${sphinx_root}/am")
    set(HMM_FOLDERS "rm1_200")
  endif()
  # Copying acoustic models into the build tree
  foreach(model ${HMM_FOLDERS})
    message("Copying ${HMM_BASE}/${model} to binary dir")
    file(COPY ${HMM_BASE}/${model} DESTINATION ${CMAKE_BINARY_DIR})
  endforeach()

  foreach(model ${HMM_FOLDERS})
    set(EMBED "${EMBED}" "--embed-file" "${model}" " ")
  endforeach()
endif(HMM_EMBED)

# Package dictionaries if needed
if(DICT_BASE AND DICT_FILES)
  foreach(dict ${DICT_FILES})
    message("Copying ${DICT_BASE}/${dict} to binary dir")
    file(COPY ${DICT_BASE}/${dict} DESTINATION ${CMAKE_BINARY_DIR})
    set(EMBED "${EMBED}" "--embed-file" "${dict}" " ")
  endforeach()
endif()

# Package language models if needed
if(LM_BASE AND LM_FILES)
  foreach(lm ${LM_FILES})
    message("Copying ${LM_BASE}/${lm} to binary dir")
    file(COPY ${LM_BASE}/${lm} DESTINATION ${CMAKE_BINARY_DIR})
    set(EMBED "${EMBED}" "--embed-file" "${lm}" " ")
  endforeach()
endif()

# Compile to WebAssembly?
if(WASM)
  separate_arguments(binaryen UNIX_COMMAND "-s WASM=1")
else(WASM)
  separate_arguments(binaryen UNIX_COMMAND "-s WASM=0")
endif(WASM)

# Adding custom target for the JavaScript library.
add_custom_target(${ps_lib_js} ALL
  COMMAND ${CMAKE_C_COMPILER} -Oz ${binaryen} -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s TOTAL_MEMORY=100663296 --bind --memory-init-file 0 ${CMAKE_SHARED_LIBRARY_PREFIX}${ps_lib}${CMAKE_SHARED_LIBRARY_SUFFIX} -o ${ps_lib_js} ${EMBED}
  DEPENDS ${ps_lib}
  )

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pocketsphinxjs-config.h.in"
  "${CMAKE_BINARY_DIR}/include/pocketsphinxjs-config.h"
  )
