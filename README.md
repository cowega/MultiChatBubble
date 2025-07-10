# MultiChatBubble
**MultiChatBubble** enhances the SA:MP chat system by allowing multiple chat bubbles to appear above players instead of replacing the previous one.  
Each bubble fades out independently based on its lifetime and the distance to the local player.

## Installation
1. Install any ASI loader
2. Place the plugin into the game's root directory

## Build
After cloning the repository, you need to select the desired version of SAMP: R1 or R3.  
For example: `-DBUILD_SAMP_R1` or `-DBUILD_SAMP_R3`
```bash
git clone https://github.com/cowega/MultiChatBubble.git
cd MultiChatBubble
cmake -B build -A Win32 -DBUILD_SAMP_R1
```
After creating CMake cache
```bash
cmake --build build
```
The .asi file will be placed in build/bin and also in the Debug or Release folder, depending on the configuration.

## Supported Versions
* SAMP 0.3.7 R1
* SAMP 0.3.7 R3

## License
MIT License