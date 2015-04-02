cmake_minimum_required(VERSION 2.6)

set(SRC src_c/stem_UTF_8_danish.c src_c/stem_UTF_8_dutch.c src_c/stem_UTF_8_english.c src_c/stem_UTF_8_finnish.c src_c/stem_UTF_8_french.c src_c/stem_UTF_8_german.c src_c/stem_UTF_8_hungarian.c src_c/stem_UTF_8_italian.c src_c/stem_UTF_8_norwegian.c src_c/stem_UTF_8_porter.c src_c/stem_UTF_8_portuguese.c src_c/stem_UTF_8_russian.c src_c/stem_UTF_8_spanish.c src_c/stem_UTF_8_swedish.c runtime/api.c runtime/utilities.c libstemmer/libstemmer_utf8.c)

add_library(stemmer STATIC ${SRC})
