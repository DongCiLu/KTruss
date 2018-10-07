ssh-keygen
ssh-copy-id lanterns2.eecs.utk.edu

sudo chown zlu12 /mydata

scp lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/regular/* /mydata
scp -r lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/testcase_truss /mydata
scp lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/checkpoints /mydata

sudo apt-get update
sudo apt-get -y install vim ssh gcc g++ build-essential

cd ~/
git clone https://github.com/snap-stanford/snap.git

mv ~/snap ~/Snap
mkdir ~/Snap/examples/ktruss
ln -s /mydata ~/Snap/examples/ktruss/datasets
ln -s ~/KTruss/common.hpp ~/Snap/examples/ktruss/common.hpp
ln -s ~/KTruss/decomposer.hpp ~/Snap/examples/ktruss/decomposer.hpp
ln -s ~/KTruss/induced_graph.hpp ~/Snap/examples/ktruss/induced_graph.hpp
ln -s ~/KTruss/main.cpp ~/Snap/examples/ktruss/main.cpp
ln -s ~/KTruss/query.hpp ~/Snap/examples/ktruss/query.hpp
ln -s ~/KTruss/tcp_index.hpp ~/Snap/examples/ktruss/tcp_index.hpp
ln -s ~/KTruss/testcase.hpp ~/Snap/examples/ktruss/testcase.hpp
ln -s ~/KTruss/tree_index.hpp ~/Snap/examples/ktruss/tree_index.hpp
ln -s ~/KTruss/update.hpp ~/Snap/examples/ktruss/update.hpp
ln -s ~/KTruss/archiver.hpp ~/Snap/examples/ktruss/archiver.hpp
ln -s ~/KTruss/unittest.cpp ~/Snap/examples/ktruss/unittest.cpp
ln -s ~/KTruss/Makefile ~/Snap/examples/ktruss/Makefile
ln -s ~/KTruss/Makefile.ex ~/Snap/examples/ktruss/Makefile.ex
ln -s ~/KTruss/simple.sh ~/Snap/examples/ktruss/simple.sh
cd ~/Snap/examples/ktruss
make

echo "You are all set."
