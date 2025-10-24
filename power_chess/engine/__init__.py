try:
    # Re-export symbols from the compiled extension.
    from ._ccore import BOARD_N, MoveType, Move, StepResult, State, Engine  # type: ignore[attr-defined]
except Exception as e:  # ImportError, OSError (bad ABI), etc.
    raise ImportError(
        "power_chess.engine: native extension '_ccore' is not available.\n"
        "Common fixes:\n"
        "  • Build/install the package first: `pip install -e .` (dev) or `pip install .`\n"
        "  • Ensure you build with the SAME Python version/architecture you are running.\n"
        "  • Linux/macOS: make sure a C++17 toolchain and Python dev headers are installed.\n"
        "  • Windows: install 'Build Tools for Visual Studio' (C++), then rebuild.\n"
        f"Original error: {e}"
    ) from e

__all__ = ["BOARD_N", "MoveType", "Move", "StepResult", "State", "Engine"]
