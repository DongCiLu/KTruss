mkdir ~/Snap/examples/ktruss
ln -s /mydata ~/Snap/examples/ktruss/datasets
ln -s ~/Ktruss/common.hpp ~/Snap/examples/ktruss/common.hpp
ln -s ~/Ktruss/decomposer.hpp ~/Snap/examples/ktruss/decomposer.hpp
ln -s ~/Ktruss/induced_graph.hpp ~/Snap/examples/ktruss/induced_graph.hpp
ln -s ~/Ktruss/main.cpp ~/Snap/examples/ktruss/main.cpp
ln -s ~/Ktruss/query.hpp ~/Snap/examples/ktruss/query.hpp
ln -s ~/Ktruss/tcp_index.hpp ~/Snap/examples/ktruss/tcp_index.hpp
ln -s ~/Ktruss/testcase.hpp ~/Snap/examples/ktruss/testcase.hpp
ln -s ~/Ktruss/tree_index.hpp ~/Snap/examples/ktruss/tree_index.hpp
ln -s ~/Ktruss/update.hpp ~/Snap/examples/ktruss/update.hpp
ln -s ~/Ktruss/archiver.hpp ~/Snap/examples/ktruss/archiver.hpp
ln -s ~/Ktruss/unittest.cpp ~/Snap/examples/ktruss/unittest.cpp
ln -s ~/Ktruss/Makefile ~/Snap/examples/ktruss/Makefile
ln -s ~/Ktruss/Makefile.ex ~/Snap/examples/ktruss/Makefile.ex
ln -s ~/Ktruss/simple.sh ~/Snap/examples/ktruss/simple.sh
cd ~/Snap/examples/ktruss
make

echo "You are all set."
