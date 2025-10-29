"""Support running RL tests via ``python -m rl.tests``."""


import pathlib
import pytest

def main() -> int:
    """Execute pytest over the rl/tests suite."""
    root = pathlib.Path(__file__).parent
    return pytest.main([str(root)])


if __name__ == "__main__":
    raise SystemExit(main())
