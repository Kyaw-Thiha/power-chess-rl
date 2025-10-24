# Power-Chess RL
A multi-agent reinforcement learning project (MARL) project that benchmarks different reinforcement learning models for playing a 6x6 chess games with power-ups. 

## Building C++ Chess Engine
For debugging
```bash
# First-time setup
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
ln -sf build/compile_commands.json compile_commands.json

# After this, just rebuild normally:
cmake --build build -j
```

For testing after building,
```bash
ctest --test-dir build -V
```

For production build
```bash
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release
ln -sf build_release/compile_commands.json compile_commands.json
```

## Building Python Chess Engine Package
We use `scikit-build` to build the c++ engine as python package based on configs in `pyproject.toml`

Create your virtual environment first.
```bash
python -m venv .env
source .env/bin/activate
```

Then, build the package.
```bash
pip install -e .
```

