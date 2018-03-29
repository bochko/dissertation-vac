#!/bin/bash
# Boyan M. Atanasov
# The University of Stirling, 2018

# dep const
CONST_BASE_DEP_LIST="nano git-core python-dev bison libasound2-dev python-pyaudio"
CONST_SPHINX_DEP_LIST="pocketsphinx python-pocketsphinx"
CONST_CMUCLMTK_DEP_LIST="subversion autoconf libtool automake gfortran g++"
CONST_PYREQUIREMENTS_RELPATH="dissertation-jaspervc/client/requirements.txt"
CONST_OPENFST_URI="http://distfiles.macports.org/openfst/openfst-1.3.4.tar.gz"
CONST_MITLM_URI="https://github.com/mitlm/mitlm/releases/download/v0.4.1/mitlm_0.4.1.tar.gz"
CONST_M2MALIGNER_URI="https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/m2m-aligner/m2m-aligner-1.2.tar.gz"
CONST_PTS_URI="https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/phonetisaurus/is2013-conversion.tgz"
CONST_BASH_PROFILE="${HOME}/.bash_profile"
CONST_TEMPDIR=".autoinstall_temp"

# info, copyright, and license...
echo "FileBash automatic installer"
sleep 0.3
echo "Boyan M. Atanasov"
sleep 0.3
echo "The University of Stirling, 2018"
sleep 0.3
echo
echo -e "\e[1;5;36mThis script will restart your machine several times.\e[0m"
echo -e "\e[1mPlease close all programs and save any files before proceeding with the installation.\e[0m"

# ask before irreversibly damaging the system :D
read -p "Install? [y/N] " -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    exit 1
fi

# register the library install paths in .bash_profile, which are going to be the destination of any custom compilation and install we do in this script.
echo -e "Making sure \"${CONST_BASH_PROFILE}\" exists..."
touch ${CONST_BASH_PROFILE}
echo -e "Registering future lib install paths..."
echo -e "export LD_LIBRARY_PATH=\"usr/local/lib\"\r\nsource .bashrc\r\n\r\nLD_LIBRARY_PATH=\"/usr/local/lib\"\r\nexport LD_LIBRARY_PATH\r\nPATH=\$PATH:/usr/local/lib/\r\nexport PATH\r\n" >> ${CONST_BASH_PROFILE}

# install dependencies
echo
echo -e "\e[1mInstalling base dependencies...\e[0m"
sudo apt install ${CONST_BASE_DEP_LIST} --yes

echo
echo -e "\e[1mInstalling pocketsphinx...\e[0m"
sudo apt install ${CONST_SPHINX_DEP_LIST} --yes

echo
echo -e "\e[1mInstalling cmuclmtk dependencies...\e[0m"
sudo apt install ${CONST_CMUCLMTK_DEP_LIST} --yes

# HERE PYTHON DEPENDENCIES SHOULD BE MET
#####
#

# download openfst, mitlm, m2maligner, and pts
mkdir ${CONST_TEMPDIR}
cd ${CONST_TEMPDIR}
echo
echo -e "\e[1mDownloading OpenFST\e[0m"
wget ${CONST_OPENFST_URI}
echo -e "\e[1mDownloading MITLM\e[0m"
wget ${CONST_MITLM_URI}
echo -e "\e[1mDownloading M2MAligner\e[0m"
wget ${CONST_M2MALIGNER_URI}
echo -e "\e[1mDownloading IS2013-Conversion\e[0m"
wget ${CONST_PTS_URI}
echo -e "\e[1mDownloading FST Model\e[0m"
wget https://www.dropbox.com/s/kfht75czdwucni1/g014b2b.tgz

echo -e "\e[1mUntarring OpenFST\e[0m"
tar -xvf openfst-1.3.4.tar.gz
echo -e "\e[1mUntarring MITLM\e[0m"
tar -xvf mitlm_0.4.1.tar.gz
echo -e "\e[1mUntarring M2MAligner\e[0m"
tar -xvf m2m-aligner-1.2.tar.gz
echo -e "\e[1mUntarring IS2013-Conversion\e[0m"
tar -xvf is2013-conversion.tgz
echo -e "\e[1mUntarring FST Model\e[0m"
tar -xvf g014b2b.tgz




