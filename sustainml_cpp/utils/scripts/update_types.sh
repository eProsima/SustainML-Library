#!/usr/bin/env bash

#
#   This script autogenerates the proper DataTypes in the
#   specified locations.
#
#   Limitation: If types were edited with new methods, regenerating
#   will overwrite those
#

#idl location | headers location | sources location
files_needing_output_dir=(
    './include/sustainml_cpp/idl/types.idl|../types|../../../src/cpp/types'
    )


yellow='\E[1;33m'
textreset='\E[1;0m'

if [[ $(ls update_types.sh 2>/dev/null | wc -l) != 1 ]]; then
    echo "Please, execute this script from its directory"
    exit -1
fi

if [[ -z "$(which fastddsgen)" ]]; then
    echo "Cannot find fastddsgen. Please, include it in PATH environment variable"
    exit -1
fi

cd ../..

readarray -d '' idl_files < <(find . -iname \*.idl -print0)

ret_value=0

for idl_file in "${idl_files[@]}"; do
    idl_dir=$(dirname "$idl_file")
    file_from_gen=$(basename "$idl_file")

    echo -e "Processing ${yellow}$idl_file${textreset}"

    cd "${idl_dir}"

    # Detect if needs output directory.
    od_include=""
    od_src=""
    for od_entry in ${files_needing_output_dir[@]}; do
        echo od_entry
        echo $od_entry
        echo idl_file
        echo "$idl_file\|*"
        if [[ $od_entry = $idl_file\|* ]]; then
            echo "HERE"
            od_entry_split=(${od_entry//\|/ })
            echo $od_entry_split
            echo ${od_entry_split[1]}
            echo ${od_entry_split[2]}
            od_include=${od_entry_split[1]}
            od_src=${od_entry_split[2]}
            break
        fi
    done

    echo $file_from_gen
    echo $od_src
    echo $od_include

    fastddsgen $file_from_gen
    mv *.h* ${od_include}
    mv *.ip* ${od_include}

    echo "sdafa "${file_from_gen%.*}.h

    for source in *.cxx; do

        sed_subarg="s:${source%.*}.h:sustainml_cpp/types/${source%.*}.h:g"
        sed_arg='/^#include/'${sed_subarg}
        echo "sed arg "$sed_arg
        sed -i $sed_arg ${source%.*}.cxx

        sed_subarg="s:${file_from_gen%.*}CdrAux:sustainml_cpp/types/${file_from_gen%.*}CdrAux:g"
        sed_arg='/^#include/'${sed_subarg}
        sed -i $sed_arg ${source%.*}.cxx
    done

    mv *.cxx ${od_src}

    if [[ $? != 0 ]]; then
        ret_value=-1
    fi

    cd -
done

cd utils/scripts

exit $ret_value
