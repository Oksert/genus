cmake_minimum_required(VERSION 2.6)

set(SRC src/FrequencyMeasure.cpp src/FrequencyMeasureStat.cpp src/StreamDocument.cpp src/ValueListAllocator.cpp src/KSpectrum.cpp src/stree.cpp src/AbstractClasses.cpp src/BinaryDeserializer.cpp src/BinarySerializer.cpp src/DFStatistics.cpp src/IDFStatistics.cpp src/DocumentStatistics.cpp src/RetrainFrequencyMeasure.cpp src/SparseDocument.cpp src/SparseDocumentCollection.cpp)

include_directories(include)

add_library(frequencymeasure STATIC ${SRC})
