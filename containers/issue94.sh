#! /bin/bash 
docker build -f issue94.dockdef -t ioh/issue94 .
docker run -t ioh/issue94 /usr/local/opt/ioh/test_store --gtest_filter=BaseTest.store_properties XDebug
