#!/usr/bin/env bash

DIR=bin/data
DB_URL="https://raw.githubusercontent.com/liuliu/ccv/unstable/samples/image-net-2012.sqlite3"
DB_FILE="$DIR/image-net-2012.sqlite3"
WORDS_URL="https://raw.githubusercontent.com/liuliu/ccv/stable/samples/image-net-2012.words"
WORDS_FILE="$DIR/image-net-2012.words"
mkdir -p $DIR
if [ ! -e $DB_FILE ] ; then
    curl -o $DB_FILE $DB_URL
    echo "Downloading image-net-2012 network (ccv)"
fi
if [ ! -e $WORDS_FILE ] ; then
    curl -o $WORDS_FILE $WORDS_URL
    echo "Downloading image-net-2012 words"
fi
#curl -o $WORDS_FILE $WORDS_URL
echo "Downloaded: image-net-2012 network (ccv) & words file"

