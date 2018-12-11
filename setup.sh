#!/usr/bin/env sh
set -o errexit

archive_dir='archives'
sysc_min_ver=3
systemc='systemc-2.3.3.gz'
sst_ver='v8.0.0_Final/'
sst_core=$sst_ver'sstcore-8.0.0.tar.gz'
sst_elem=$sst_ver'sstelements-8.0.0.tar.gz'

mkdir $archive_dir

wget http://www.accellera.org/images/downloads/standards/systemc/$systemc -O $archive_dir/$systemc
wget https://github.com/sstsimulator/sst-core/releases/download/$sst_core -O $archive_dir/"${sst_core##*/}"
wget https://github.com/sstsimulator/sst-elements/releases/download/$sst_elem -O $archive_dir/"${sst_elem##*/}"
