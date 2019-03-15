#!/usr/bin/env sh
set -o errexit

archive_dir='archives'

systemc='systemc-2.3.3.gz'
systemc_url='http://www.accellera.org/images/downloads/standards/systemc/'

sst_ver='v8.0.0_Final/'
sst_core_url='https://github.com/sstsimulator/sst-core/releases/download/'
sst_elem_url='https://github.com/sstsimulator/sst-elements/releases/download/'
sst_core=$sst_ver'sstcore-8.0.0.tar.gz'
sst_elem=$sst_ver'sstelements-8.0.0.tar.gz'

mkdir $archive_dir

wget $systemc_url$systemc -O $archive_dir/$systemc
wget $sst_core_url$sst_core -O $archive_dir/"${sst_core##*/}"
wget $sst_elem_url$sst_elem -O $archive_dir/"${sst_elem##*/}"
