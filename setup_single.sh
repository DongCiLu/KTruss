ssh-keygen
ssh-copy-id lanterns2.eecs.utk.edu

sudo chown zlu12 /mydata

scp lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/regular/* /mydata
scp -r lanterns2.eecs.utk.edu:/local_scratch/Datasets/graph_datasets/testcases /mydata

sudo apt-get update
sudo apt-get -y install vim ssh gcc g++ build-essential 

cd /local
git clone https://github.com/snap-stanford/snap.git

mv /local/snap /local/Snap
mkdir /local/Snap/examples/ktruss
ln -s /mydata /local/Snap/examples/ktruss/datasets
ln -s /local/KTruss/ktruss.cpp /local/Snap/examples/ktruss/ktruss.cpp
ln -s /local/KTruss/Makefile /local/Snap/examples/ktruss/Makefile
ln -s /local/KTruss/Makefile.ex /local/Snap/examples/ktruss/Makefile.ex
ln -s /local/KTruss/simple.sh /local/Snap/examples/ktruss/simple.sh
cd /local/Snap/examples/ktruss
make

echo "You are all set."
