cmake_minimum_required(VERSION 2.6)

find_path(CPPUNIT_PATH cppunit/extensions/HelperMacros.h)

find_path(BOOST_PATH boost/asio.hpp)

set(SRC src/Binary_Model.cpp src/Classified_Document.cpp src/Decomposition.cpp src/Document.cpp src/Document_Set.cpp src/Kernel_Perceptron.cpp src/ml_classification.cpp src/Model.cpp src/TopicsArray.cpp src/SVM_SMO.cpp src/decompositionMatrix/DecompositionMatrixGenerator.cpp src/decompositionMatrix/DecompositionMatrix.cpp src/helper/VectorHelper.cpp src/helper/MatrixHelper.cpp src/ClassifierTestSuites.cpp src/rankSolver/SimpleTeamRankSolver.cpp src/rankSolver/BTOneVsAllRankSolver.cpp src/rankSolver/BTDGeneralRankSolver.cpp src/rankSolver/BTDOneVsOneRankSolver.cpp src/rankSolver/RankSolverTester.cpp src/rankSolver/probabilisticModel/SimpleProbabilisticModel.cpp)

include_directories(include ../pyBinding/include src/ooqp/include ${CPPUNIT_PATH} ${BOOST_PATH})

add_library(classifier STATIC ${SRC})
