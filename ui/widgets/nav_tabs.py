from __future__ import annotations
from dataclasses import dataclass
from typing import Iterable

from textual.widget import Widget
from textual.reactive import reactive
from textual.message import Message
from textual.containers import Vertical
from textual.app import ComposeResult
from textual.widgets import Button  # ← use Button
from textual import on


@dataclass(slots=True)
class NavItem:
    key: str
    label: str
    icon: str = " "


class NavTabs(Widget):
    """Right-docked vertical navigation with lazygit vibe."""

    items: reactive[list[NavItem]] = reactive(list)
    active_key: reactive[str] = reactive("hotseat")

    class NavSelected(Message):
        """Emitted when a nav item is selected."""

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
                # Buttons give us a reliable Pressed message with .button.id
                yield Button(f"{prefix} {it.label}", id=f"nav-{it.key}", classes=classes)

    def _refresh_visuals(self) -> None:
        """Update label + active class on buttons without remounting."""
        for it in self.items:
            btn = self.query_one(f"#nav-{it.key}", Button)
            is_active = it.key == self.active_key
            prefix = "▶" if is_active else "  "
            btn.label = f"{prefix} {it.label}"
            btn.set_class(is_active, "-active")

    @on(Button.Pressed)
    def _on_pressed(self, event: Button.Pressed) -> None:
        btn_id = event.button.id or ""
        if not btn_id.startswith("nav-"):
            return
        key = btn_id[4:]
        if key == self.active_key:
            return
        self.active_key = key
        self._refresh_visuals()
        self.post_message(self.NavSelected(key))
