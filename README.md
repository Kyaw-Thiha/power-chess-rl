# Power-Chess RL

## Building C++ Chess Engine
For debugging
```bash
# First-time setup
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
ln -sf build/compile_commands.json compile_commands.json

# After this, just rebuild normally:
cmake --build build -j
```

For production build
```bash
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release
ln -sf build_release/compile_commands.json compile_commands.json
```
