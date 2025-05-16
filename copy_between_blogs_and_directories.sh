#!/bin/bash
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   24/10/22 15:35:26
# Desc    :
########################################################################

# set -x
CUR_DIR=$(cd `dirname $0`; pwd)
cd ${CUR_DIR}


local=~/Documents/xuechengyun_sync/Obsidian/blog/
to_pub=${CUR_DIR}/content/post/

if [[ $1 == "pub" ]]; then
    echo "rm $to_pub"
    rm -rf $to_pub
    cd $local
    sh $local/run.sh
    cp -r $local $to_pub
elif [[ $1 == "bak" ]]; then
    cp -r $to_pub $local
else
    echo "[Usage]: sh $0 [pub|bak]"
fi








cd -


