content_dir=$(pwd)
game_dir="$(pwd)/../testgame/"
cd ../testgame/bin

./atex -file "${content_dir}/materials/models/brickwall.adf" -out "${game_dir}/materials/models/" &
./atex -file "${content_dir}/materials/models/brickwall_normal.adf" -out "${game_dir}/materials/models/" &
./atex -file "${content_dir}/materials/models/backpack.adf" -out "${game_dir}/materials/models/" &
./atex -file "${content_dir}/materials/models/backpack_normal.adf" -out "${game_dir}/materials/models/" &
./atex -file "${content_dir}/materials/models/backpack_mr.adf" -out "${game_dir}/materials/models/" &
./atex -file "${content_dir}/materials/dev/red.adf" -out "${game_dir}/materials/dev/" &
./atex -file "${content_dir}/materials/dev/green.adf" -out "${game_dir}/materials/dev/" &
./atex -file "${content_dir}/materials/dev/blue.adf" -out "${game_dir}/materials/dev/" &
./atex -file "${content_dir}/materials/dev/channeltest.adf" -out "${game_dir}/materials/dev/" &
./atex -file "${content_dir}/materials/error.adf" -out "${game_dir}/materials/" &

wait
