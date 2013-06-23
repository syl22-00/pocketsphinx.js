#############################################
#
# CMakeLists.txt for PocketSphinx
#
#############################################

cmake_minimum_required(VERSION 2.6)

project(pocketsphinx.js)

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
set(fe_src_files fe_interface.c fe_sigproc.c fe_warp_affine.c fe_warp.c fe_warp_inverse_linear.c fe_warp_piecewise_linear.c fixlog.c) 
set(fe_srcs "")
foreach(cfile ${fe_src_files})
    set(fe_srcs ${fe_srcs} "${fe_folder}/${cfile}")
endforeach(cfile)

set(feat_src_files agc.c cmn.c cmn_prior.c feat.c lda.c)
set(feat_srcs "")
foreach(cfile ${feat_src_files})
    set(feat_srcs ${feat_srcs} "${feat_folder}/${cfile}")
endforeach(cfile)

set(util_src_files bio.c bitvec.c case.c ckd_alloc.c cmd_ln.c dtoa.c err.c errno.c f2c_lite.c filename.c genrand.c glist.c hash_table.c heap.c huff_code.c info.c listelem_alloc.c logmath.c matrix.c mmio.c pio.c profile.c sbthread.c strfuncs.c utf8.c )
set(util_srcs "")
foreach(cfile ${util_src_files})
    set(util_srcs ${util_srcs} "${util_folder}/${cfile}")
endforeach(cfile)

set(lm_src_files fsg_model.c jsgf.c jsgf_parser.c jsgf_scanner.c lm3g_model.c ngram_model_arpa.c ngram_model_dmp.c ngram_model_set.c ngram_model.c)
set(lm_srcs "")
foreach(cfile ${lm_src_files})
    set(lm_srcs ${lm_srcs} "${lm_folder}/${cfile}")
endforeach(cfile)

# PocketSphinx folders
include_directories("${sphinx_root}/pocketsphinx/src/libpocketsphinx")
include_directories("${sphinx_root}/pocketsphinx/include")

set(pocketsphinx_folder "${sphinx_root}/pocketsphinx/src/libpocketsphinx")

# PocketSphinx source files
set(pocketsphinx_src_files acmod.c bin_mdef.c blkarray_list.c dict.c dict2pid.c fsg_history.c fsg_lextree.c fsg_search.c hmm.c mdef.c ms_gauden.c ms_mgau.c ms_senone.c ngram_search.c ngram_search_fwdtree.c ngram_search_fwdflat.c phone_loop_search.c pocketsphinx.c ps_lattice.c ps_mllr.c ptm_mgau.c s2_semi_mgau.c tmat.c vector.c )
set(pocketsphinx_srcs "")
foreach(cfile ${pocketsphinx_src_files})
    set(pocketsphinx_srcs ${pocketsphinx_srcs} "${pocketsphinx_folder}/${cfile}")
endforeach(cfile)

set(ps_lib_js "pocketsphinx.js")
set(ps_lib "pocketsphinx")

set(JS_COMPILATION_FLAGS "-s" "\ EXPORTED_FUNCTIONS=\"['_psInitialize','_psGetState','_psGetHyp','_psStartGrammar','_psEndGrammar','_psSwitchGrammar','_psAddWord','_psAddTransition','_psStart','_psStop','_psProcess','_malloc','_free']\"")
set(AM_RESOURCES "am")

# Building a library to be used from JavaScript
add_library(${ps_lib} SHARED "src/pocketsphinxjs.cpp" "src/pocketsphinxjsImpl.cpp" ${pocketsphinx_srcs}  ${fe_srcs} ${feat_srcs} ${lm_srcs} ${util_srcs})

# Copying resources into the build tree
file(COPY ${sphinx_root}/${AM_RESOURCES} DESTINATION ${CMAKE_BINARY_DIR})

# Adding custom target for the HTML for the library
add_custom_target(${ps_lib_js} ALL
  COMMAND ${CMAKE_C_COMPILER} ${CMAKE_SHARED_LIBRARY_PREFIX}${ps_lib}${CMAKE_SHARED_LIBRARY_SUFFIX} -O2 -o ${ps_lib_js} --embed-file ${AM_RESOURCES} ${JS_COMPILATION_FLAGS}
  DEPENDS ${ps_lib}
  )
