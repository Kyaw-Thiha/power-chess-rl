# PowerChess RL — Textual UI (Tokyonight)

Lazygit-style TUI for a 6×6 chess variant with RL agents. Uses your exported `power_chess.engine` types directly—no adapter layer.

## Pages
- **Hotseat** — human vs human (interactive board)
- **VS AI** — human vs AI (press **Step** for AI move)
- **AI v AI** — two AIs; **Step** advances a single move
- **Replay** — step through moves from a JSONL file
- **Exit** — confirm to quit

## Run
```bash
python -m ui.app
```

## Replay format (JSONL)

Each line is a move dictionary. Either from or from_ is accepted:
```json
{"from": 7, "to": 13, "type": 0, "promo_piece": 0, "special_code": 0}
```



