# install dep
sudo apt-get install nano git-core python-dev bison libasound2-dev python-pyaudio --yes

#make sure .bash_profile exists
touch ~/.bash_profile

#append this to bash ~./bash_profile
export LD_LIBRARY_PATH="/usr/local/lib"
source .bashrc
LD_LIBRARY_PATH="/usr/local/lib"
export LD_LIBRARY_PATH
PATH=$PATH:/usr/local/lib/
export PATH

#at this point, restart

#install pocketsphinx and sphinxbase
sudo apt-get install pocketsphinx python-pocketsphinx

#install dependencies for cmuclmtk
sudo apt-get install subversion autoconf libtool automake gfortran g++ --yes

#navigate to vc repository and fetch from svn
svn co https://svn.code.sf.net/p/cmusphinx/code/trunk/cmuclmtk/
#then make and install
cd cmuclmtk/
./autogen.sh && make && sudo make install
cd ..

#use pip and client/requirements.txt to install python deps from jasper folder
sudo pip install -r "client/requirements.txt"

#fetch needed archives
wget http://distfiles.macports.org/openfst/openfst-1.3.4.tar.gz
wget https://github.com/mitlm/mitlm/releases/download/v0.4.1/mitlm_0.4.1.tar.gz
wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/m2m-aligner/m2m-aligner-1.2.tar.gz
wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/phonetisaurus/is2013-conversion.tgz
#untar those
tar -xvf m2m-aligner-1.2.tar.gz
tar -xvf openfst-1.3.4.tar.gz
tar -xvf is2013-conversion.tgz
tar -xvf mitlm_0.4.1.tar.gz

#build OPENFST
#if building with a new version of g++, no modifications needed
#if building with gcc or old g++, need to force "-std=c++98",
#because coding has been done in compliance with that standard :(
cd openfst-1.3.4/
sudo ./configure --enable-compact-fsts --enable-const-fsts --enable-far --enable-lookahead-fsts --enable-pdt
#this takes really long
sudo make install

#installing phonetisaurus, m2m-aligner, and MITLM
#no issues anticipated here
cd m2m-aligner-1.2/
sudo make
cd ..

cd mitlm-0.4.1/
sudo ./configure
sudo make install
cd..

cd is2013-conversion/phonetisaurus/src
sudo make
#some warnings may pop up, don't worry
cd ..

#move the m2m-aligner compilation units
sudo cp m2m-aligner-1.2/m2m-aligner /usr/local/bin/m2m-aligner

#move the phonetisaurus compilation units
sudo cp is2013-conversion/bin/phonetisaurus-g2p /usr/local/bin/phonetisaurus-g2p
