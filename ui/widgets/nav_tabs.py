from __future__ import annotations
from dataclasses import dataclass
from typing import Iterable

from textual.widget import Widget
from textual.reactive import reactive
from textual.message import Message
from textual.containers import Vertical
from textual.app import ComposeResult
from textual.widgets import Button
from textual.events import Focus
from textual import on


@dataclass(slots=True)
class NavItem:
    key: str
    label: str
    icon: str = " "


class NavTabs(Widget):
    """Right-docked vertical navigation with lazygit vibe."""

    # Keyboard bindings for arrow nav + enter
    BINDINGS = [
        ("up", "nav_up", "Up"),
        ("down", "nav_down", "Down"),
        ("enter", "nav_activate", "Open"),
        ("right", "nav_activate", ""),
        ("k", "nav_up", ""),
        ("j", "nav_down", ""),
        ("l", "nav_activate", ""),
        ("h", "nav_leave", ""),  # go back to main
    ]

    items: reactive[list[NavItem]] = reactive(list)
    active_key: reactive[str] = reactive("hotseat")

    class NavSelected(Message):
        bubble = True

        def __init__(self, key: str) -> None:
            self.key = key
            super().__init__()

    def __init__(self, items: Iterable[NavItem], active_key: str = "hotseat", id: str | None = None) -> None:
        super().__init__(id=id)
        self.items = list(items)
        self.active_key = active_key

    def compose(self) -> ComposeResult:
        with Vertical(id="nav-container"):
            for it in self.items:
                is_active = it.key == self.active_key
                prefix = "▶" if is_active else "  "
                classes = "item"
                if is_active:
                    classes += " -active"
                yield Button(f"{prefix} {it.label}", id=f"nav-{it.key}", classes=classes)

    def _refresh_visuals(self) -> None:
        for it in self.items:
            btn = self.query_one(f"#nav-{it.key}", Button)
            is_active = it.key == self.active_key
            prefix = "▶" if is_active else "  "
            btn.label = f"{prefix} {it.label}"
            btn.set_class(is_active, "-active")

    # Mouse press → select & emit
    @on(Button.Pressed)
    def _on_pressed(self, event: Button.Pressed) -> None:
        btn_id = event.button.id or ""
        if not btn_id.startswith("nav-"):
            return
        key = btn_id[4:]
        self._select_and_emit(key)

    # Keyboard actions
    def action_nav_up(self) -> None:
        idx = self._index_of(self.active_key)
        if idx is None:
            return
        self._select(self.items[(idx - 1) % len(self.items)].key)

    def action_nav_down(self) -> None:
        idx = self._index_of(self.active_key)
        if idx is None:
            return
        self._select(self.items[(idx + 1) % len(self.items)].key)

    def action_nav_leave(self) -> None:
        # Ask the app to focus main pane (safe even if not present)
        try:
            self.app.action_focus_main()  # type: ignore[attr-defined]
        except Exception:
            pass

    def action_nav_activate(self) -> None:
        # If a button is focused, honor it; otherwise fallback to active_key
        try:
            focused = self.screen.focused  # type: ignore[attr-defined]
            if isinstance(focused, Button) and focused.id and focused.id.startswith("nav-"):
                key = focused.id[4:]
                self._select_and_emit(key)
                return
        except Exception:
            pass
        self._select_and_emit(self.active_key)

    # Helpers
    def _index_of(self, key: str) -> int | None:
        for i, it in enumerate(self.items):
            if it.key == key:
                return i
        return None

    def _select(self, key: str) -> None:
        if key == self.active_key:
            return
        self.active_key = key
        self._refresh_visuals()
        # keep focus on the active button for a11y
        try:
            self.query_one(f"#nav-{key}", Button).focus()
        except Exception:
            pass

    def _select_and_emit(self, key: str) -> None:
        self._select(key)
        self.post_message(self.NavSelected(key))

    @on(Focus)
    def _on_focus(self, event: Focus) -> None:
        # put keyboard focus on the active button so Enter works
        try:
            self.query_one(f"#nav-{self.active_key}", Button).focus()
        except Exception:
            pass
