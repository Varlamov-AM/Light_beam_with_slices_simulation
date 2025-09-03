#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 N_IMAGES_TO_GENERATE"
    exit 1
fi

N_IMAGES_TO_GENERATE=$1

if ! [[ "$N_IMAGES_TO_GENERATE" =~ ^[0-9]+$ ]] || [ "$N_IMAGES_TO_GENERATE" -le 0 ]; then
    echo "Error: N_IMAGES_TO_GENERATE must be a positive integer"
    exit 1
fi

mkdir -p batch

for ((i=1; i<=N_IMAGES_TO_GENERATE; i++)); do
    echo "Processing iteration $i of $N_IMAGES_TO_GENERATE"

    if [ -f "../scripts/generate_materials_domain.py" ]; then
        python3 ../scripts/generate_materials_domain.py
        if [ ! -f "materials.txt" ]; then
            echo "Error: materials.txt was not generated"
            exit 1
        fi
    else
        echo "Error: ../scripts/generate_materials_domain.py not found"
        exit 1
    fi

    ./LightSimulation
    if [ ! -f "image_data.txt" ]; then
        echo "Error: image_data.txt was not generated"
        exit 1
    fi

    if [ -f "../scripts/process_image.py" ]; then
        python3 ../scripts/process_image.py
        if [ ! -f "output_image.png" ]; then
            echo "Error: output_image.png was not generated"
            exit 1
        fi
    else
        echo "Error: ../scripts/process_image.py not found"
        exit 1
    fi

    printf -v index "%05d" $i
    mv materials.txt "batch/phys_prop_data_$index.txt"
    mv image_data.txt "batch/phys_res_data_$index.txt"
    mv output_image.png "batch/phys_img_data_$index.png"

    echo "Generated files for iteration $i moved to batch directory"
done

echo "Batch processing complete. Generated $N_IMAGES_TO_GENERATE sets of files in batch/"
