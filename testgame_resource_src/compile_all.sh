content_dir=$(pwd)

./compile_models.sh &
cd ${content_dir}
./compile_textures.sh &

wait
