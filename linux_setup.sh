# Create build folder for logging
mkdir log/files
# Create build folder, should be created outside of social-gaming folder
# *In order for this to work, your social-gaming folder should be inside another folder
mkdir ../build
# Build files will be inside this folder
cd ../build

# Install dependencies
sudo apt-get update
sudo apt install g++ build-essential libboost-system-dev libboost-thread-dev libboost-program-options-dev libboost-test-dev libncurses5-dev libncursesw5-dev libunwind-dev libgflags-dev

# Configuring project
cmake ../social-gaming

# Building project
make

# Printing where binaries can be found
echo "+===========================================================================+"
echo "+                               SETUP SUCCESS                               +"
echo "+===========================================================================+"
echo -e "Executable binaries are now in:\n\tprojectfolder/build/bin"
echo -e "The server is inside bin and named:\n\tchatserver"
echo -e "The client is inside bin and named:\n\tchatclient"
echo -e "For more info on how to use client/server, run the executables inside bin."
