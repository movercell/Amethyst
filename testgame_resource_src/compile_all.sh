content_dir=$(pwd)
game_dir="$(pwd)/../testgame/"
cd ../testgame/bin

./amdl -file "${content_dir}/models/error.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/backpack.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/cube.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/sphere.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/multimesh.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/floor_plane.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/machine_base.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/machine_blade.adf" -out "${game_dir}/models/"
./amdl -file "${content_dir}/models/machine_support.adf" -out "${game_dir}/models/"
