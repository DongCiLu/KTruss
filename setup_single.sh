ssh-keygen
ssh-copy-id lanterns2.eecs.utk.edu

sudo chown zlu12 /mydata

scp lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/regular/* /mydata
scp -r lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/testcases /mydata

sudo apt-get update
sudo apt-get -y install vim ssh gcc g++ build-essential 

git clone https://github.com/snap-stanford/snap.git

mv snap Snap
mkdir Snap/examples/ktruss
ln -s /mydata Snap/examples/ktruss/datasets
ln -s KTruss/ktruss.cpp Snap/examples/ktruss/ktruss.cpp
ln -s KTruss/Makefile Snap/examples/ktruss/Makefile
ln -s KTruss/Makefile.ex Snap/examples/ktruss/Makefile.ex
ln -s KTruss/simple.sh Snap/examples/ktruss/simple.sh
cd Snap/examples/ktruss
make

echo "You are all set."
